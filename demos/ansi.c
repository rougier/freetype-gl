/* ============================================================================
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
 */
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include "freetype-gl.h"
#include "font-manager.h"
#include "vertex-buffer.h"
#include "text-buffer.h"
#include "markup.h"
#include "shader.h"
#include "mat4.h"

#include <GLFW/glfw3.h>


/* ------------------------------------------------------ global variables - */
text_buffer_t * buffer;
mat4   model, view, projection;


/* --------------------------------------------------------------- display - */
void display( GLFWwindow* window )
{
    glClearColor(1.00,1.00,1.00,1.00);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glColor4f(1.00,1.00,1.00,1.00);
    glUseProgram( buffer->shader );
    {
        glUniformMatrix4fv( glGetUniformLocation( buffer->shader, "model" ),
                            1, 0, model.data);
        glUniformMatrix4fv( glGetUniformLocation( buffer->shader, "view" ),
                            1, 0, view.data);
        glUniformMatrix4fv( glGetUniformLocation( buffer->shader, "projection" ),
                            1, 0, projection.data);
        text_buffer_render( buffer );
    }

    glfwSwapBuffers( window );
}


/* -------------------------------------------------------------- reshape - */
void reshape( GLFWwindow* window, int width, int height )
{
    glViewport(0, 0, width, height);
    mat4_set_orthographic( &projection, 0, width, 0, height, -1, 1);
}


/* -------------------------------------------------------------- keyboard - */
void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose( window, GL_TRUE );
    }
}


/* ----------------------------------------------------------- init_colors - */
void
init_colors( vec4 *colors )
{
    vec4 defaults[16] = 
        {
            {{ 46/256.0f,  52/256.0f,  54/256.0f, 1.0f}},
            {{204/256.0f,   0/256.0f,   0/256.0f, 1.0f}},
            {{ 78/256.0f, 154/256.0f,   6/256.0f, 1.0f}},
            {{196/256.0f, 160/256.0f,   0/256.0f, 1.0f}},
            {{ 52/256.0f, 101/256.0f, 164/256.0f, 1.0f}},
            {{117/256.0f,  80/256.0f, 123/256.0f, 1.0f}},
            {{  6/256.0f, 152/256.0f, 154/256.0f, 1.0f}},
            {{211/256.0f, 215/256.0f, 207/256.0f, 1.0f}},
            {{ 85/256.0f,  87/256.0f,  83/256.0f, 1.0f}},
            {{239/256.0f,  41/256.0f,  41/256.0f, 1.0f}},
            {{138/256.0f, 226/256.0f,  52/256.0f, 1.0f}},
            {{252/256.0f, 233/256.0f,  79/256.0f, 1.0f}},
            {{114/256.0f, 159/256.0f, 207/256.0f, 1.0f}},
            {{173/256.0f, 127/256.0f, 168/256.0f, 1.0f}},
            {{ 52/256.0f, 226/256.0f, 226/256.0f, 1.0f}},
            {{238/256.0f, 238/256.0f, 236/256.0f, 1.0f}}
        };
    size_t i = 0;
    /* Default 16 colors */
    for( i=0; i< 16; ++i )
    {
        colors[i] = defaults[i];
    }
    /* Color cube */
    for( i=0; i<6*6*6; i++ )
    {
        vec4 color = {{ (i/6/6)/5.0f, ((i/6)%6)/5.0f, (i%6)/5.0f, 1.0f}};
        colors[i+16] = color;
    }
    /* Grascale ramp (24 tones) */
    for( i=0; i<24; i++ )
    {
        vec4 color ={{i/24.0f, i/24.0f, i/24.0f, 1.0f}};
        colors[232+i] = color;
    }
}


/* -------------------------------------------------------- ansi_to_markup - */
void
ansi_to_markup( wchar_t *sequence, size_t length, markup_t *markup )
{
    size_t i;
    int code = 0;
    int set_bg = -1;
    int set_fg = -1;
    vec4 none = {{0,0,0,0}};
    static vec4 * colors = 0;

    if( colors == 0 )
    {
        colors = (vec4 *) malloc( sizeof(vec4) * 256 );
        init_colors( colors );
    }

    if( length <= 1 )
    {
        markup->foreground_color = colors[0];
        markup->underline_color = markup->foreground_color;
        markup->overline_color = markup->foreground_color;
        markup->strikethrough_color = markup->foreground_color;
        markup->outline_color = markup->foreground_color;
        markup->background_color = none;
        markup->underline = 0;
        markup->overline = 0;
        markup->bold = 0;
        markup->italic = 0;
        markup->strikethrough = 0;
        return;
    }

    for( i=0; i<length; ++i)
    {
        wchar_t c = *(sequence+i);
        if( c >= '0' && c <= '9' )
        {
            code = code * 10 + (c-'0');
        }
        else if( (c == ';') || (i == (length-1)) )
        {
            if( set_fg == 1 )
            {
                markup->foreground_color = colors[code];
                set_fg = -1;
            }
            else if( set_bg == 1 )
            {
                markup->background_color = colors[code];
                set_bg = -1;
            }
            else if( (set_fg == 0) && (code == 5) )
            {
                set_fg = 1;
                code = 0;
            }
            else if( (set_bg == 0) && (code == 5) )
            {
                set_bg = 1;
                code = 0;
            }
            /* Set fg color (30 + x, where x is the index of the color) */
            else if( (code >= 30) && (code < 38 ) )
            {
                markup->foreground_color = colors[code-30];
            }
            /* Set bg color (40 + x, where x is the index of the color) */
            else if( (code >= 40) && (code < 48 ) )
            {
                markup->background_color = colors[code-40];
            }
            else 
            {
                switch (code)
                {
                case 0:
                    markup->foreground_color = colors[0];
                    markup->background_color = none;
                    markup->underline = 0;
                    markup->overline = 0;
                    markup->bold = 0;
                    markup->italic = 0;
                    markup->strikethrough = 0;
                    break;
                case 1: markup->bold = 1; break;
                case 21: markup->bold = 0; break;
                case 2: markup->foreground_color.alpha = 0.5; break;
                case 22: markup->foreground_color.alpha = 1.0; break;
                case 3:  markup->italic = 1; break;
                case 23: markup->italic = 0; break;
                case 4:  markup->underline = 1; break;
                case 24: markup->underline = 0; break;
                case 8: markup->foreground_color.alpha = 0.0; break;
                case 28: markup->foreground_color.alpha = 1.0; break;
                case 9:  markup->strikethrough = 1; break;
                case 29: markup->strikethrough = 0; break;
                case 53: markup->overline = 1; break;
                case 55: markup->overline = 0; break;
                case 39: markup->foreground_color = colors[0]; break;
                case 49: markup->background_color = none; break;
                case 38: set_fg = 0; break;
                case 48: set_bg = 0; break;
                default: break;
                }
            }
            code = 0;
        }
    }
    markup->underline_color = markup->foreground_color;
    markup->overline_color = markup->foreground_color;
    markup->strikethrough_color = markup->foreground_color;
    markup->outline_color = markup->foreground_color;
}

/* ----------------------------------------------------------------- print - */
void
print( text_buffer_t * buffer, vec2 * pen,
       wchar_t *text, markup_t *markup )
{
    wchar_t *seq_start = text, *seq_end = text;
    wchar_t *p;
    for( p=text; p<(text+wcslen(text)); ++p )
    {
        wchar_t *start = wcsstr( p, L"\033[" );
        wchar_t *end = NULL;
        if( start) 
        {
            end = wcsstr( start+1, L"m");
        }
        if( (start == p) && (end > start) )
        {
            seq_start = start+2;
            seq_end = end;
            p = end;
        }
        else
        {
            int seq_size = (seq_end-seq_start)+1;
            wchar_t * text_start = p;
            int text_size = 0;
            if( start )
            {
                text_size = start-p;
                p = start-1;
            }
            else
            {
                text_size = text+wcslen(text)-p;
                p = text+wcslen(text);
            }                
            ansi_to_markup(seq_start, seq_size, markup );
            markup->font = font_manager_get_from_markup( buffer->manager, markup );
            text_buffer_add_text( buffer, pen, markup, text_start, text_size );
        }
    }
}

/* -------------------------------------------------------- error-callback - */
void error_callback( int error, const char* description )
{
    fputs( description, stderr );
}

/* ------------------------------------------------------------------ main - */
int main( int argc, char **argv )
{
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

    buffer = text_buffer_new( LCD_FILTERING_OFF );
    vec4 black = {{0.0, 0.0, 0.0, 1.0}};
    vec4 none  = {{1.0, 1.0, 1.0, 0.0}};

    markup_t markup;
    markup.family  = "fonts/VeraMono.ttf",
    markup.size    = 15.0;
    markup.bold    = 0;
    markup.italic  = 0;
    markup.rise    = 0.0;
    markup.spacing = 0.0;
    markup.gamma   = 1.0;
    markup.foreground_color    = black;
    markup.background_color    = none;
    markup.underline           = 0;   
    markup.underline_color     = black;
    markup.overline            = 0;  
    markup.overline_color      = black;
    markup.strikethrough       = 0;   
    markup.strikethrough_color = black;
    markup.font = 0;

    vec2 pen = {{10.0, 480.0}};
    FILE *file = fopen ( "data/256colors.txt", "r" );
    if ( file != NULL )
    {
        wchar_t line[1024];
        while( fgetws ( line, sizeof(line), file ) != NULL )
        {
            print( buffer, &pen, line, &markup );
        }
        fclose ( file );
    }

    mat4_set_identity( &projection );
    mat4_set_identity( &model );
    mat4_set_identity( &view );

    glfwSetWindowSize( window, 800, 500 );
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
