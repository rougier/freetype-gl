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
#include <stdarg.h>
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
    float r, g, b, a;
} vertex_t;


// ------------------------------------------------------- global variables ---
texture_atlas_t * atlas;
vertex_buffer_t * buffer;
GLuint shader;
mat4 model, view, projection;


// ---------------------------------------------------------------- display ---
void display( void )
{
    static GLuint texture = 0;
    if( !texture )
    {
        texture = glGetUniformLocation( shader, "texture" );
    }

    glClearColor( 0.40, 0.40, 0.45, 1.00 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_BLEND );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_COLOR_MATERIAL );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
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
void add_text( vertex_buffer_t * buffer, vec2 * pen, ... )
{
    markup_t *markup;
    wchar_t *text;
    va_list args;
    va_start ( args, pen );

    do {
        markup = va_arg( args, markup_t * );
        if( markup == NULL )
        {
            break;
        }
        text = va_arg( args, wchar_t * );

        size_t i;
        texture_font_t * font = markup->font;
        float r = markup->foreground_color.red;
        float g = markup->foreground_color.green;
        float b = markup->foreground_color.blue;
        float a = markup->foreground_color.alpha;

        for( i=0; i<wcslen(text); ++i )
        {
            texture_glyph_t *glyph = texture_font_get_glyph( font, text[i] );

            if( glyph != NULL )
            {
                float kerning = 0.0f;
                if( i > 0)
                {
                    kerning = texture_glyph_get_kerning( glyph, text[i-1] );
                }
                pen->x += kerning;

                /* Actual glyph */
                float x0  = ( pen->x + glyph->offset_x );
                float y0  = (int)( pen->y + glyph->offset_y );
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
                    { (int)x0,y0,0,  s0,t0,  r,g,b,a },
                    { (int)x0,y1,0,  s0,t1,  r,g,b,a },
                    { (int)x1,y1,0,  s1,t1,  r,g,b,a },
                    { (int)x1,y0,0,  s1,t0,  r,g,b,a } };
                vertex_buffer_push_back_indices( buffer, indices, 6 );
                vertex_buffer_push_back_vertices( buffer, vertices, 4 );
                pen->x += glyph->advance_x;
            }

        }
    } while( markup != 0 );
    va_end ( args );
}

// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    size_t i;
    size_t width = 600, height = 250;

    glutInit( &argc, argv );
    glutInitWindowSize( width, height );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Glyph Outline" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf( stderr, "Error: %s\n", glewGetErrorString(err) );
        exit( EXIT_FAILURE );
    }
    fprintf( stderr, "Using GLEW %s\n", glewGetString(GLEW_VERSION) );
#endif
    atlas = texture_atlas_new( 512, 512, 1 );
    buffer = vertex_buffer_new( "vertex:3f,tex_coord:2f,color:4f" );

    vec4 white  = {{1.0, 1.0, 1.0, 1.0}};
    vec4 none   = {{1.0, 1.0, 1.0, 0.0}};

	markup_t markup;
    markup.family  = "fonts/Vera.ttf";
    markup.size    = 80.0;
    markup.bold    = 0;
    markup.italic  = 0;
    markup.rise    = 0.0;
    markup.spacing = 0.0;
    markup.gamma   = 1.5;
    markup.foreground_color    = white;
    markup.background_color    = none;
    markup.underline           = 0;
    markup.underline_color     = white;
    markup.overline            = 0;
    markup.overline_color      = white;
    markup.strikethrough       = 0;
    markup.strikethrough_color = white;
    markup.font = 0;

    markup.font = texture_font_new_from_file( atlas, markup.size, "fonts/Vera.ttf" );

    markup.font->outline_type = 1;

    vec2 pen;
    pen.x = 40;
    pen.y = 190;
    for( i=0; i< 10; ++i)
    {
        markup.font->outline_thickness = 2*((i+1)/10.0);
        add_text( buffer, &pen, &markup, L"g", NULL );
    }

    pen.x = 40;
    pen.y  = 110;
    markup.font->outline_type = 2;
    for( i=0; i< 10; ++i)
    {
        markup.font->outline_thickness = 2*((i+1)/10.0);
        add_text( buffer, &pen, &markup, L"g", NULL );
    }

    pen.x = 40;
    pen.y  = 30;
    markup.font->outline_type = 3;
    for( i=0; i< 10; ++i)
    {
        markup.font->outline_thickness = 1*((i+1)/10.0);
        add_text( buffer, &pen, &markup, L"g", NULL );
    }
    shader = shader_load("shaders/v3f-t2f-c4f.vert",
                         "shaders/v3f-t2f-c4f.frag");
    mat4_set_identity( &projection );
    mat4_set_identity( &model );
    mat4_set_identity( &view );

    glutMainLoop( );
    return 0;
}
