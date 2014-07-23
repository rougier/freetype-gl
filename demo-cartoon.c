/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * -------------------------------------------------------------------------
 * Copyright 2011,2012 Nicolas P. Rougier. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Nicolas P. Rougier.
 * ========================================================================= */
#include <stdio.h>
#include <wchar.h>

#include "freetype-gl.h"

#include "vertex-buffer.h"
#include "markup.h"
#include "shader.h"
#include "mat4.h"

#if defined(__APPLE__)
    #include <Glut/glut.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif


// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, z;
    float u, v;
    vec4 color;
} vertex_t;


// ------------------------------------------------------- global variables ---
texture_atlas_t * atlas;
vertex_buffer_t * buffer;
GLuint shader;
mat4   model, view, projection;



// ---------------------------------------------------------------- display ---
void display( void )
{
    glClearColor( 0.40, 0.40, 0.45, 1.00 );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glUseProgram( shader );
    {
        glUniform1i( glGetUniformLocation( shader, "texture" ),
                     0 );
        glUniformMatrix4fv( glGetUniformLocation( shader, "model" ),
                            1, 0, model.data);
        glUniformMatrix4fv( glGetUniformLocation( shader, "view" ),
                            1, 0, view.data);
        glUniformMatrix4fv( glGetUniformLocation( shader, "projection" ),
                            1, 0, projection.data);
        vertex_buffer_render( buffer, GL_TRIANGLES );
    }

    glutSwapBuffers( );
}


// ---------------------------------------------------------------- reshape ---
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    mat4_set_orthographic( &projection, 0, width, 0, height, -1, 1);
}

// --------------------------------------------------------------- keyboard ---
void keyboard( unsigned char key, int x, int y )
{
    if ( key == 27 )
    {
        exit( EXIT_SUCCESS );
    }
}


// --------------------------------------------------------------- add_text ---
void add_text( vertex_buffer_t * buffer, texture_font_t * font,
               wchar_t *text, vec2 pen, vec4 fg_color_1, vec4 fg_color_2 )
{
    size_t i;
    for( i=0; i<wcslen(text); ++i )
    {
        texture_glyph_t *glyph = texture_font_get_glyph( font, text[i] );
        float kerning = 0.0f;
        if( i > 0)
        {
            kerning = texture_glyph_get_kerning( glyph, text[i-1] );
        }
        pen.x += kerning;

        /* Actual glyph */
        float x0  = ( pen.x + glyph->offset_x );
        float y0  = (int)( pen.y + glyph->offset_y );
        float x1  = ( x0 + glyph->width );
        float y1  = (int)( y0 - glyph->height );
        float s0 = glyph->s0;
        float t0 = glyph->t0;
        float s1 = glyph->s1;
        float t1 = glyph->t1;
        GLuint index = buffer->vertices->size;
        GLuint indices[] = {index, index+1, index+2,
                            index, index+2, index+3};
        vertex_t vertices[] = {
            { (int)x0,y0,0,  s0,t0,  fg_color_1 },
            { (int)x0,y1,0,  s0,t1,  fg_color_2 },
            { (int)x1,y1,0,  s1,t1,  fg_color_2 },
            { (int)x1,y0,0,  s1,t0,  fg_color_1 } };
        vertex_buffer_push_back_indices( buffer, indices, 6 );
        vertex_buffer_push_back_vertices( buffer, vertices, 4 );
        pen.x += glyph->advance_x;
    }
}

// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    size_t width = 800, height = 200;

    glutInit( &argc, argv );
    glutInitWindowSize( width, height );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Glyph Cartoon" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf( stderr, "Error: %s\n", glewGetErrorString(err) );
        exit( EXIT_FAILURE );
    }
    fprintf( stderr, "Using GLEW %s\n", glewGetString(GLEW_VERSION) );

    atlas = texture_atlas_new( 1024, 1024, 1 );
    buffer = vertex_buffer_new( "vertex:3f,tex_coord:2f,color:4f" ); 
    texture_font_t *font =
		texture_font_new_from_file( atlas, 128, "fonts/ObelixPro.ttf" );

    vec2 pen    = {{50, 50}};
    vec4 black  = {{0.0, 0.0, 0.0, 1.0}};
    vec4 yellow = {{1.0, 1.0, 0.0, 1.0}};
    vec4 orange1 = {{1.0, 0.9, 0.0, 1.0}};
    vec4 orange2 = {{1.0, 0.6, 0.0, 1.0}};


    font->outline_type = 2;
    font->outline_thickness = 7;
    add_text( buffer, font, L"Freetype GL", pen, black, black );

    font->outline_type = 2;
    font->outline_thickness = 5;
    add_text( buffer, font, L"Freetype GL", pen, yellow, yellow );

    font->outline_type = 1;
    font->outline_thickness = 3;
    add_text( buffer, font, L"Freetype GL", pen, black, black );

    font->outline_type = 0;
    font->outline_thickness = 0;
    add_text( buffer, font, L"Freetype GL", pen, orange1, orange2 );

    shader = shader_load("shaders/v3f-t2f-c4f.vert",
                         "shaders/v3f-t2f-c4f.frag");
    mat4_set_identity( &projection );
    mat4_set_identity( &model );
    mat4_set_identity( &view );

    glutMainLoop( );
    return 0;
}
