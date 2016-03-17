/* ============================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         https://github.com/rougier/freetype-gl
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
 */
#include <stdio.h>
#include <string.h>

#include "freetype-gl.h"
#include "distance-field.h"
#include "vertex-buffer.h"
#include "text-buffer.h"
#include "markup.h"
#include "shader.h"
#include "mat4.h"

#include <GLFW/glfw3.h>


#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif


// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
} vertex_t;

typedef struct {
    float x, y, zoom;
} viewport_t;


// ------------------------------------------------------- global variables ---
GLuint shader;
vertex_buffer_t *buffer;
texture_atlas_t * atlas = 0;
mat4  model, view, projection;
viewport_t viewport = {0,0,1};


// ------------------------------------------------------------------- init ---
void init( void )
{
    unsigned char *map;
    texture_font_t * font;
    const char *filename = "fonts/Vera.ttf";
    const char * cache = " !\"#$%&'()*+,-./0123456789:;<=>?"
                         "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                         "`abcdefghijklmnopqrstuvwxyz{|}~";

    atlas = texture_atlas_new( 512, 512, 1 );
    font = texture_font_new_from_file( atlas, 72, filename );
    texture_font_load_glyphs( font, cache );
    texture_font_delete( font );

    fprintf( stderr, "Generating distance map...\n" );
    map = make_distance_mapb(atlas->data, atlas->width, atlas->height);
    fprintf( stderr, "done !\n");

    memcpy( atlas->data, map, atlas->width*atlas->height*sizeof(unsigned char) );
    free(map);
    texture_atlas_upload( atlas );

    GLuint indices[6] = {0,1,2, 0,2,3};
    vertex_t vertices[4] = { { 0,0,0,  0,1,  1,1,1,1 },
                             { 0,1,0,  0,0,  1,1,1,1 },
                             { 1,1,0,  1,0,  1,1,1,1 },
                             { 1,0,0,  1,1,  1,1,1,1 } };
    buffer = vertex_buffer_new( "vertex:3f,tex_coord:2f,color:4f" );
    vertex_buffer_push_back( buffer, vertices, 4, indices, 6 );

    shader = shader_load( "shaders/distance-field.vert",
                           "shaders/distance-field.frag" );
    mat4_set_identity( &projection );
    mat4_set_identity( &model );
    mat4_set_identity( &view );
}


// ---------------------------------------------------------------- display ---
void display( GLFWwindow* window )
{
    glClearColor(0.5,0.5,0.5,1.00);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas->id);
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    GLint v[4];
    glGetIntegerv( GL_VIEWPORT, v );
    GLint width  = v[2];
    GLint height = v[3];

    vec4 color = {{1.0, 1.0, 1.0, 1.0 }};

    mat4_set_identity( &model );
    mat4_scale( &model, width * viewport.zoom, height * viewport.zoom, 0 );
    mat4_translate( &model, viewport.x, viewport.y, 0);

    glUseProgram( shader );
    {
        glUniform1i( glGetUniformLocation( shader, "u_texture" ),
                     0);
        glUniform4f( glGetUniformLocation( shader, "u_color" ),
                     color.r, color.g, color.b, color.a);
        glUniformMatrix4fv( glGetUniformLocation( shader, "u_model" ),
                            1, 0, model.data);
        glUniformMatrix4fv( glGetUniformLocation( shader, "u_view" ),
                            1, 0, view.data);
        glUniformMatrix4fv( glGetUniformLocation( shader, "u_projection" ),
                            1, 0, projection.data);

        vertex_buffer_render( buffer, GL_TRIANGLES );
    }

    glfwSwapBuffers( window );
}


// ---------------------------------------------------------- cursor_motion ---
void cursor_motion( GLFWwindow* window, double x, double y )
{
    int v[4];
    static int _x=-1, _y=-1;

    if( GLFW_PRESS == glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) )
    {
        if( (_x == -1) && (_y == -1) )
        {
            _x = x; _y = y;
            return;
        }
        int dy = y - _y;
        if (dy < 0)
        {
            viewport.zoom *= 1.05;
        }
        else
        {
            viewport.zoom /= 1.05;
        }
        _x = x; _y = y;
    }

    glGetIntegerv( GL_VIEWPORT, v );
    GLfloat width = v[2], height = v[3];
    float nx = min( max( x/width, 0.0), 1.0 );
    float ny = 1-min( max( y/height, 0.0), 1.0 );
    viewport.x = nx*width*(1-viewport.zoom);
    viewport.y = ny*height*(1-viewport.zoom);
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


// --------------------------------------------------------- error-callback ---
void error_callback( int error, const char* description )
{
    fputs( description, stderr );
}


// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    GLFWwindow* window;

    glfwSetErrorCallback( error_callback );

    if (!glfwInit( ))
    {
        exit( EXIT_FAILURE );
    }

    glfwWindowHint( GLFW_VISIBLE, GL_FALSE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

    window = glfwCreateWindow( 1, 1, "Freetype OpenGL width shaders", NULL, NULL );

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
    glfwSetCursorPosCallback( window, cursor_motion );

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

    init();

    glfwSetWindowSize( window, 512, 512 );
    glfwShowWindow( window );

    while(!glfwWindowShouldClose( window ))
    {
        display( window );
        glfwPollEvents( );
    }

    glfwDestroyWindow( window );
    glfwTerminate( );

    return EXIT_SUCCESS;
}
