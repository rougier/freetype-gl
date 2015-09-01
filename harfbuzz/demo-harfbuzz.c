/* ===========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * ----------------------------------------------------------------------------
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
 * ============================================================================
 *
 * All credits go to https://github.com/lxnt/ex-sdl-freetype-harfbuzz
 *
 * ============================================================================ */

#include "freetype-gl.h"
#include "mat4.h"
#include "shader.h"
#include "vertex-buffer.h"
#include "texture-font.h"
#include <math.h>

#include <GLFW/glfw3.h>


/* google this */
#ifndef unlikely
#define unlikely
#endif


// ------------------------------------------------------- global variables ---
GLuint shader;
vertex_buffer_t * vbuffer;
mat4 model, view, projection;

const char *text = "صِف خَلقَ خَودِ كَمِثلِ الشَمسِ إِذ بَزَغَت — يَحظى الضَجيعُ بِها نَجلاءَ مِعطارِ";
const char *font_filename      = "fonts/amiri-regular.ttf";
const hb_direction_t direction = HB_DIRECTION_RTL;
const hb_script_t script       = HB_SCRIPT_ARABIC;
const char *language           = "ar";

// ---------------------------------------------------------------- display ---
void display( GLFWwindow* window )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glUseProgram( shader );
    {
        glUniform1i( glGetUniformLocation( shader, "texture" ), 0 );
        glUniform3f( glGetUniformLocation( shader, "pixel" ), 1/512., 1/512., 1.0f );

        glUniformMatrix4fv( glGetUniformLocation( shader, "model" ),
                            1, 0, model.data);
        glUniformMatrix4fv( glGetUniformLocation( shader, "view" ),
                            1, 0, view.data);
        glUniformMatrix4fv( glGetUniformLocation( shader, "projection" ),
                            1, 0, projection.data);
        vertex_buffer_render( vbuffer, GL_TRIANGLES );
    }

    glfwSwapBuffers( window );
}


// ---------------------------------------------------------------- reshape ---
void reshape( GLFWwindow* window, int width, int height )
{
    glViewport(0, 0, width, height);
    mat4_set_orthographic( &projection, 0, width, 0, height, -1, 1);
}


// --------------------------------------------------------------- keyboard ---
void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose( window, GL_TRUE );
    }
}


/* -------------------------------------------------------- error-callback - */
void error_callback( int error, const char* description )
{
    fputs( description, stderr );
}


// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    size_t i, j;
    GLFWwindow* window;

    glfwSetErrorCallback( error_callback );

    if (!glfwInit( ))
    {
        exit( EXIT_FAILURE );
    }

    glfwWindowHint( GLFW_VISIBLE, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

    window = glfwCreateWindow( 1, 1, argv[0], NULL, NULL );

    if (!window)
    {
        glfwTerminate( );
        exit( EXIT_FAILURE );
    }

    glfwMakeContextCurrent( window );
    glfwSwapInterval( 1 );

    glfwSetFramebufferSizeCallback( window, reshape );
    glfwSetWindowRefreshCallback( window, display );
    glfwSetKeyCallback( window, keyboard );

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

    texture_atlas_t * atlas = texture_atlas_new( 512, 512, 3 );
    texture_font_t *fonts[20];
    for ( i=0; i< 20; ++i )
    {
        fonts[i] =  texture_font_new_from_file(atlas, 12+i, font_filename),
        texture_font_load_glyphs(fonts[i], text, direction, language, script );
    }


    typedef struct { float x,y,z, u,v, r,g,b,a, shift, gamma; } vertex_t;
    vbuffer = vertex_buffer_new( "vertex:3f,tex_coord:2f,"
                                "color:4f,ashift:1f,agamma:1f" );

    /* Create a buffer for harfbuzz to use */
    hb_buffer_t *buffer = hb_buffer_create();

    for (i=0; i < 20; ++i)
    {
        hb_buffer_set_direction( buffer, direction );
        hb_buffer_set_script( buffer, script );
        hb_buffer_set_language( buffer,
                                hb_language_from_string(language, strlen(language)) );
        hb_buffer_add_utf8( buffer, text, strlen(text), 0, strlen(text) );
        hb_shape( fonts[i]->hb_ft_font, buffer, NULL, 0 );

        unsigned int         glyph_count;
        hb_glyph_info_t     *glyph_info =
            hb_buffer_get_glyph_infos(buffer, &glyph_count);
        hb_glyph_position_t *glyph_pos =
            hb_buffer_get_glyph_positions(buffer, &glyph_count);

        texture_font_load_glyphs( fonts[i], text,
                                  direction, language, script );

        float gamma = 1.0;
        float shift = 0.0;
        float x = 0;
        float y = 600 - i * (10+i) - 15;
        float width = 0.0;
        float hres = fonts[i]->hres;
        for (j = 0; j < glyph_count; ++j)
        {
            int codepoint = glyph_info[j].codepoint;
            float x_advance = glyph_pos[j].x_advance/(float)(hres*64);
            float x_offset = glyph_pos[j].x_offset/(float)(hres*64);
            texture_glyph_t *glyph = texture_font_get_glyph(fonts[i], codepoint);
            if( i < (glyph_count-1) )
                width += x_advance + x_offset;
            else
                width += glyph->offset_x + glyph->width;
        }

        x = 800 - width - 10 ;
        for (j = 0; j < glyph_count; ++j)
        {
            int codepoint = glyph_info[j].codepoint;
            // because of vhinting trick we need the extra 64 (hres)
            float x_advance = glyph_pos[j].x_advance/(float)(hres*64);
            float x_offset = glyph_pos[j].x_offset/(float)(hres*64);
            float y_advance = glyph_pos[j].y_advance/(float)(64);
            float y_offset = glyph_pos[j].y_offset/(float)(64);
            texture_glyph_t *glyph = texture_font_get_glyph(fonts[i], codepoint);

            float r = 0.0;
            float g = 0.0;
            float b = 0.0;
            float a = 1.0;
            float x0 = x + x_offset + glyph->offset_x;
            float x1 = x0 + glyph->width;
            float y0 = floor(y + y_offset + glyph->offset_y);
            float y1 = floor(y0 - glyph->height);
            float s0 = glyph->s0;
            float t0 = glyph->t0;
            float s1 = glyph->s1;
            float t1 = glyph->t1;
            vertex_t vertices[4] =  {
                {x0,y0,0, s0,t0, r,g,b,a, shift, gamma},
                {x0,y1,0, s0,t1, r,g,b,a, shift, gamma},
                {x1,y1,0, s1,t1, r,g,b,a, shift, gamma},
                {x1,y0,0, s1,t0, r,g,b,a, shift, gamma} };
            GLushort indices[6] = { 0,1,2, 0,2,3 };
            vertex_buffer_push_back( vbuffer, vertices, 4, indices, 6 );
            x += x_advance;
            y += y_advance;
        }
        /* clean up the buffer, but don't kill it just yet */
        hb_buffer_reset(buffer);
    }

    glClearColor(1,1,1,1);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glBindTexture( GL_TEXTURE_2D, atlas->id );
    texture_atlas_upload( atlas );
    vertex_buffer_upload( vbuffer );
    shader = shader_load("shaders/text.vert", "shaders/text.frag");
    mat4_set_identity( &projection );
    mat4_set_identity( &model );
    mat4_set_identity( &view );

    glfwSetWindowSize( window, 800, 600 );
    glfwShowWindow( window );

    while(!glfwWindowShouldClose( window ))
    {
        display( window );
        glfwPollEvents( );
    }

    glfwDestroyWindow( window );
    glfwTerminate( );

    return 0;
}
