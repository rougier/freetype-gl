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
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "vertex-buffer.h"


// --------------------------------------------------- typedef & structures ---
typedef struct {
    float x, y;
    float u, v;
    float r, g, b, a;
    float thickness;}
vertex_t;


// ------------------------------------------------------- global variables ---
vertex_buffer_t * lines;
GLuint program = 0;

const char *vertex_shader_source = 
    "attribute float thickness;                                        "
    "varying float t;                                                  "
    "void main()                                                       "
    "{                                                                 "
    "    gl_FrontColor = gl_Color;                                     "
    "    gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;                     "
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;       "
    "    t = thickness;                                                "
    "}                                                                 ";

const char *fragment_shader_source = 
    "varying float t;                                                  "
    "void main()                                                       "
    "{                                                                 "
    "    vec4 color   = gl_Color;                                      "
    "    vec2 p       = gl_TexCoord[0].st;                             "
    "    float dist   = sqrt(p.x*p.x + p.y*p.y);                       "
    "    float width;                                                  "
    "    if (t < 1.0) width = dist;                                    "
    "    else         width = fwidth(dist);                            "
    "    float alpha  = smoothstep(0.5-width,0.5+width, dist);         "
    "    gl_FragColor = vec4(color.rgb, color.a*min((1.0-alpha), t) ); "
    "}                                                                 ";


// ---------------------------------------------------------------- display ---
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );
    glColor4f( 0.0f, 0.0f, 0.0f, 1.0f);
    glUseProgram( program );
    vertex_buffer_render( lines, GL_TRIANGLES, "vtc" );
    glUseProgram( 0 );
    glutSwapBuffers( );
}


// ---------------------------------------------------------------- reshape ---
void reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    glOrtho( 0, width, 0, height, -1, 1 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
}


// --------------------------------------------------------------- keyboard ---
void
keyboard( unsigned char key, int x, int y )
{
    if ( key == 27 )
    {
        exit( EXIT_SUCCESS );
    }
}


// ----------------------------------------------------------- build_shader ---
GLuint
build_shader( const char* source, GLenum type )
{
    GLuint handle = glCreateShader( type );
    glShaderSource( handle, 1, &source, 0 );
    glCompileShader( handle );

    GLint compile_status;
    glGetShaderiv( handle, GL_COMPILE_STATUS, &compile_status );
    if( compile_status == GL_FALSE )
    {
        GLchar messages[256];
        glGetShaderInfoLog( handle, sizeof(messages), 0, &messages[0] );
        fprintf( stderr, "%s\n", messages );
        exit( EXIT_FAILURE );
    }
    return handle;
}


// ---------------------------------------------------------- build_program ---
GLuint
build_program( const char * vertex_source,
               const char * fragment_source )
{
    GLuint vertex_shader   = build_shader( vertex_source, GL_VERTEX_SHADER);
    GLuint fragment_shader = build_shader( fragment_source, GL_FRAGMENT_SHADER);
    
    GLuint handle = glCreateProgram( );
    glAttachShader( handle, vertex_shader);
    glAttachShader( handle, fragment_shader);
    glLinkProgram( handle);
    
    GLint link_status;
    glGetProgramiv( handle, GL_LINK_STATUS, &link_status );
    if (link_status == GL_FALSE)
    {
        GLchar messages[256];
        glGetProgramInfoLog( handle, sizeof(messages), 0, &messages[0] );
        fprintf( stderr, "%s\n", messages );
        exit(1);
    }
    
    return handle;
}


// ----------------------------------------------------------- make_segment ---
void make_segment( vertex_buffer_t * buffer,
                   float x0, float y0,
                   float x1, float y1,
                   float thickness,
                   float r, float g, float b, float a )
{

    assert( (x1 != x0) || (y1 != y0) );
    assert( thickness != 0 );

    float d = sqrt( (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) );
    float dx, dy, t;

    if (thickness < 2.0)
    {
        dx = (x1-x0)/d*0.5*2;
        dy = (y1-y0)/d*0.5*2;
        d = 2.0 - thickness/2.0;
        t = thickness/2.0;
    }
    else if (thickness < 3.0)
    {
        dx = (x1-x0)/d*0.5*3;
        dy = (y1-y0)/d*0.5*3;
        d = 2.5 - thickness/2.0;
        t = 1.0;
    }
    else
    {
        dx = (x1-x0)/d*0.5*thickness;
        dy = (y1-y0)/d*0.5*thickness;
        d = 1.0;
        t = 1.0;
    }

    vertex_t body[6] = { {x0+dy,y0-dx, 0,-d, r,g,b,a, t},
                         {x0-dy,y0+dx, 0,+d, r,g,b,a, t},
                         {x1-dy,y1+dx, 0,+d, r,g,b,a, t},

                         {x0+dy,y0-dx, 0,-d, r,g,b,a, t},
                         {x1-dy,y1+dx, 0,+d, r,g,b,a, t},
                         {x1+dy,y1-dx, 0,-d, r,g,b,a, t} };
    vertex_buffer_push_back_vertices ( buffer, body, 6);

    // If line is too thin, no caps
    if( thickness < 1.0 )
    {
        return;
    }

    // Line Cap 0
    vertex_t cap0[6] = { {x0+dy,   y0-dx,     0,-1, r,g,b,a, t},
                         {x0-dy,   y0+dx,     0,+1, r,g,b,a, t},
                         {x0-dy-dx,y0+dx-dy, +1,+1, r,g,b,a, t},

                         {x0+dy,   y0-dx,     0,-1, r,g,b,a, t},
                         {x0-dy-dx,y0+dx-dy, +1,+1, r,g,b,a, t},
                         {x0+dy-dx,y0-dx-dy, +1,-1, r,g,b,a, t} };
    vertex_buffer_push_back_vertices ( buffer, cap0, 6);

    // Line Cap 1
    vertex_t cap1[6] = { {x1-dy,    y1+dx,     0,-1, r,g,b,a, t},
                         {x1+dy,    y1-dx,     0,+1, r,g,b,a, t},
                         {x1+dy+dx, y1-dx+dy, +1,+1, r,g,b,a, t},
                         
                         {x1-dy,    y1+dx,     0,-1, r,g,b,a, t},
                         {x1+dy+dx, y1-dx+dy, +1,+1, r,g,b,a, t},
                         {x1-dy+dx, y1+dx+dy, +1,-1, r,g,b,a, t} };
    vertex_buffer_push_back_vertices ( buffer, cap1, 6);
}



// ------------------------------------------------------------------- init ---
void init( void )
{
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glDisable( GL_DEPTH_TEST ); 
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // Make program
    program = build_program( vertex_shader_source, fragment_shader_source );
    //GLuint attrib = glGetAttribLocation(program, "thickness");
    //printf("%d\n", attrib);

    // Make lines
    lines = vertex_buffer_new( "v2f:t2f:c4f:1g1f" );
    float r=0.0f, g=0.0f, b=0.0f, a=1.0f;
    size_t i;
    for( i=0; i<57; ++i)
    {
        float thickness = (i+1)*0.2;
        float x0 = 2+i*10+0.315;
        float y0 = 5+0.315;
        float x1 = 35+i*10+0.315;
        float y1 = 170+0.315;
        make_segment(lines, x0,y0, x1,y1, thickness, r,g,b,a);
    }
}


// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitWindowSize( 600, 175 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "OpenGL antialiased lines" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    init();
    glutMainLoop( );

    return EXIT_SUCCESS;
}
