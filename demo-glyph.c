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

#include "font-manager.h"
#include "vertex-buffer.h"
#include "text-buffer.h"
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
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
} vertex_t;

typedef struct {
    float x, y, z;
    vec4 color;
} point_t;


// ------------------------------------------------------- global variables ---
vertex_buffer_t * text_buffer;
vertex_buffer_t * line_buffer;
vertex_buffer_t * point_buffer;
GLuint shader, text_shader;
mat4 model, view, projection;


// ---------------------------------------------------------------- display ---
void display( void )
{
    int viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_POINT_SMOOTH );

    glEnable( GL_TEXTURE_2D );

    glUseProgram( text_shader );
    {
        glUniform1i( glGetUniformLocation( text_shader, "texture" ),
                     0 );
        glUniformMatrix4fv( glGetUniformLocation( text_shader, "model" ),
                            1, 0, model.data);
        glUniformMatrix4fv( glGetUniformLocation( text_shader, "view" ),
                            1, 0, view.data);
        glUniformMatrix4fv( glGetUniformLocation( text_shader, "projection" ),
                            1, 0, projection.data);
        vertex_buffer_render( text_buffer, GL_TRIANGLES );
    }

    glDisable( GL_TEXTURE_2D );
    glPointSize( 10.0f );

    glUseProgram( shader );
    {
        glUniformMatrix4fv( glGetUniformLocation( shader, "model" ),
                            1, 0, model.data);
        glUniformMatrix4fv( glGetUniformLocation( shader, "view" ),
                            1, 0, view.data);
        glUniformMatrix4fv( glGetUniformLocation( shader, "projection" ),
                            1, 0, projection.data);
        vertex_buffer_render( line_buffer, GL_LINES );
        vertex_buffer_render( point_buffer, GL_POINTS );
    }
    glUseProgram( 0 );

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
               wchar_t *  text, vec4 * color, vec2 * pen )
{
    size_t i;
    float r = color->red, g = color->green, b = color->blue, a = color->alpha;
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
            int x0  = (int)( pen->x + glyph->offset_x );
            int y0  = (int)( pen->y + glyph->offset_y );
            int x1  = (int)( x0 + glyph->width );
            int y1  = (int)( y0 - glyph->height );
            float s0 = glyph->s0;
            float t0 = glyph->t0;
            float s1 = glyph->s1;
            float t1 = glyph->t1;
            GLuint indices[] = {0,1,2,0,2,3};
            vertex_t vertices[] = { { x0,y0,0,  s0,t0,  r,g,b,a },
                                    { x0,y1,0,  s0,t1,  r,g,b,a },
                                    { x1,y1,0,  s1,t1,  r,g,b,a },
                                    { x1,y0,0,  s1,t0,  r,g,b,a } };
            vertex_buffer_push_back( buffer, vertices, 4, indices, 6 );
            pen->x += glyph->advance_x;
        }
    }
}


// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    int width  = 600;
    int height = 600;

    glutInit( &argc, argv );
    glutInitWindowSize( width, height );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf( stderr, "Error: %s\n", glewGetErrorString(err) );
        exit( EXIT_FAILURE );
    }
    fprintf( stderr, "Using GLEW %s\n", glewGetString(GLEW_VERSION) );

    vec4 blue  = {{0,0,1,1}};
    vec4 black = {{0,0,0,1}};

    texture_atlas_t * atlas = texture_atlas_new( 512, 512, 1);
    texture_font_t * big = texture_font_new_from_file( atlas, 400, "fonts/Vera.ttf");
    texture_font_t * small = texture_font_new_from_file( atlas, 18, "fonts/Vera.ttf");
    texture_font_t * title = texture_font_new_from_file( atlas, 32, "fonts/Vera.ttf");

    text_buffer  = vertex_buffer_new( "vertex:3f,tex_coord:2f,color:4f" ); 
    line_buffer  = vertex_buffer_new( "vertex:3f,color:4f" ); 
    point_buffer = vertex_buffer_new( "vertex:3f,color:4f" ); 

    vec2 pen, origin;

    texture_glyph_t *glyph  = texture_font_get_glyph( big, L'g' );
    origin.x = width/2  - glyph->offset_x - glyph->width/2;
    origin.y = height/2 - glyph->offset_y + glyph->height/2;
    add_text( text_buffer, big, L"g", &black, &origin );

    // title
    pen.x = 50;
    pen.y = 560;
    add_text( text_buffer, title, L"Glyph metrics", &black, &pen );

    point_t vertices[] = 
        {   // Baseline
            {0.1*width, origin.y, 0, black},
            {0.9*width, origin.y, 0, black}, 

            // Top line
            {0.1*width, origin.y + glyph->offset_y, 0, black},
            {0.9*width, origin.y + glyph->offset_y, 0, black},

            // Bottom line
            {0.1*width, origin.y + glyph->offset_y - glyph->height, 0, black},
            {0.9*width, origin.y + glyph->offset_y - glyph->height, 0, black},

            // Left line at origin
            {width/2-glyph->offset_x-glyph->width/2, 0.1*height, 0, black},
            {width/2-glyph->offset_x-glyph->width/2, 0.9*height, 0, black},

            // Left line
            {width/2 - glyph->width/2, .3*height, 0, black},
            {width/2 - glyph->width/2, .9*height, 0, black},

            // Right line
            {width/2 + glyph->width/2, .3*height, 0, black},
            {width/2 + glyph->width/2, .9*height, 0, black},

            // Right line at origin
            {width/2-glyph->offset_x-glyph->width/2+glyph->advance_x, 0.1*height, 0, black},
            {width/2-glyph->offset_x-glyph->width/2+glyph->advance_x, 0.7*height, 0, black},

            // Width
            {width/2 - glyph->width/2, 0.8*height, 0, blue},
            {width/2 + glyph->width/2, 0.8*height, 0, blue},

            // Advance_x
            {width/2-glyph->width/2-glyph->offset_x, 0.2*height, 0, blue},
            {width/2-glyph->width/2-glyph->offset_x+glyph->advance_x, 0.2*height, 0, blue},
            
            // Offset_x
            {width/2-glyph->width/2-glyph->offset_x, 0.85*height, 0, blue},
            {width/2-glyph->width/2, 0.85*height, 0, blue},

            // Height
            {0.3*width/2, origin.y + glyph->offset_y - glyph->height, 0, blue},
            {0.3*width/2, origin.y + glyph->offset_y, 0, blue},

            // Offset y
            {0.8*width, origin.y + glyph->offset_y, 0, blue},
            {0.8*width, origin.y , 0, blue},

        };
    GLuint indices [] = {  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,
                          13,14,15,16,17,18,19,20,21,22,23,24,25};
    vertex_buffer_push_back( line_buffer, vertices, 26, indices, 26 );



    pen.x = width/2 - 48;
    pen.y = .2*height - 18;
    add_text( text_buffer, small, L"advance_x", &blue, &pen );

    pen.x = width/2 - 20;
    pen.y = .8*height + 3;
    add_text( text_buffer, small, L"width", &blue, &pen );

    pen.x = width/2 - glyph->width/2 + 5;
    pen.y = .85*height-8;
    add_text( text_buffer, small, L"offset_x", &blue, &pen );

    pen.x = 0.2*width/2-30;
    pen.y = origin.y + glyph->offset_y - glyph->height/2;
    add_text( text_buffer, small, L"height", &blue, &pen );

    pen.x = 0.8*width+3;
    pen.y = origin.y + glyph->offset_y/2 -6;
    add_text( text_buffer, small, L"offset_y", &blue, &pen );

    pen.x = width/2  - glyph->offset_x - glyph->width/2 - 58;
    pen.y = height/2 - glyph->offset_y + glyph->height/2 - 20;
    add_text( text_buffer, small, L"Origin", &black, &pen );


    GLuint i = 0;
    point_t p;
    p.color = black;

    // Origin point
    p.x = width/2  - glyph->offset_x - glyph->width/2;
    p.y = height/2 - glyph->offset_y + glyph->height/2;
    vertex_buffer_push_back( point_buffer, &p, 1, &i, 1 );

    // Advance point
    p.x = width/2  - glyph->offset_x - glyph->width/2 + glyph->advance_x;
    p.y = height/2 - glyph->offset_y + glyph->height/2;
    vertex_buffer_push_back( point_buffer, &p, 1, &i, 1 );

    
    text_shader = shader_load( "shaders/v3f-t2f-c4f.vert",
                               "shaders/v3f-t2f-c4f.frag" );
    shader = shader_load( "shaders/v3f-c4f.vert",
                          "shaders/v3f-c4f.frag" );
    mat4_set_identity( &projection );
    mat4_set_identity( &model );
    mat4_set_identity( &view );

    glutMainLoop( );
    return 0;
}
