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
 *     documentation and/or other materials proided with the distribution.
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
#include <time.h>
#include "freetype-gl.h"
#include "shader.h"
#include "vertex-buffer.h"

#if defined(__APPLE__)
    #include <Glut/glut.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif



// ------------------------------------------------------- global variables ---
GLuint shader;
vertex_buffer_t * cube;



// ------------------------------------------------------------------- init ---
void init( void )
{
    glPolygonOffset( 1, 1 );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_LINE_SMOOTH );
}


// ---------------------------------------------------------------- display ---
void display( void )
{
    static float theta=0, phi=0;
    static GLuint Color = 0;
    float seconds_elapsed = (float)clock() / CLOCKS_PER_SEC;

    if( !Color )
    {
        Color = glGetUniformLocation( shader, "Color" );
    }

    theta = .5f * seconds_elapsed / 0.016f;
    phi = .5f * seconds_elapsed / 0.016f;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPushMatrix();
    glRotatef( theta, 0,0,1 );
    glRotatef( phi,   0,1,0 );
    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_POLYGON_OFFSET_FILL );
    glUseProgram( shader );
    glUniform4f( Color, 1, 1, 1, 1 );
    vertex_buffer_render( cube, GL_QUADS );
    glDisable( GL_POLYGON_OFFSET_FILL );
    glEnable( GL_BLEND );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glDepthMask( GL_FALSE );
    glUniform4f( Color, 0, 0, 0, .5 );
    vertex_buffer_render( cube, GL_QUADS );
    glUseProgram( 0 );
    glDepthMask( GL_TRUE );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glPopMatrix();

    glutSwapBuffers( );
}


// ---------------------------------------------------------------- reshape ---
void reshape( int width, int height )
{
    glViewport(0, 0, width, height);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    gluPerspective( 45.0, width/(float) height, 2.0, 10.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    glTranslatef( 0.0, 0.0, -5.0 );
}


// --------------------------------------------------------------- keyboard ---
void keyboard( unsigned char key, int x, int y )
{
    if ( key == 27 )
    {
        exit( EXIT_SUCCESS );
    }
}


// ------------------------------------------------------------------ timer ---
void timer( int dt )
{
    glutPostRedisplay();
    glutTimerFunc( dt, timer, dt );
}

// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitWindowSize( 400, 400 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( argv[0] );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    //glutTimerFunc( 1000/60, timer, 1000/60 ); // not working on some systems (bug in GLUT)
    glutIdleFunc(display);
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

    typedef struct { float x,y,z;} xyz;
    typedef struct { float r,g,b,a;} rgba;
    typedef struct { xyz position, normal; rgba color;} vertex;
    xyz v[] = { { 1, 1, 1},  {-1, 1, 1},  {-1,-1, 1}, { 1,-1, 1},
                { 1,-1,-1},  { 1, 1,-1},  {-1, 1,-1}, {-1,-1,-1} };
    xyz n[] = { { 0, 0, 1},  { 1, 0, 0},  { 0, 1, 0} ,
                {-1, 0, 1},  { 0,-1, 0},  { 0, 0,-1} };
    rgba c[] = { {1, 1, 1, 1},  {1, 1, 0, 1},  {1, 0, 1, 1},  {0, 1, 1, 1},
                 {1, 0, 0, 1},  {0, 0, 1, 1},  {0, 1, 0, 1},  {0, 0, 0, 1} };
    vertex vertices[24] =  {
      {v[0],n[0],c[0]}, {v[1],n[0],c[1]}, {v[2],n[0],c[2]}, {v[3],n[0],c[3]},
      {v[0],n[1],c[0]}, {v[3],n[1],c[3]}, {v[4],n[1],c[4]}, {v[5],n[1],c[5]},
      {v[0],n[2],c[0]}, {v[5],n[2],c[5]}, {v[6],n[2],c[6]}, {v[1],n[2],c[1]},
      {v[1],n[3],c[1]}, {v[6],n[3],c[6]}, {v[7],n[3],c[7]}, {v[2],n[3],c[2]},
      {v[7],n[4],c[7]}, {v[4],n[4],c[4]}, {v[3],n[4],c[3]}, {v[2],n[4],c[2]},
      {v[4],n[5],c[4]}, {v[7],n[5],c[7]}, {v[6],n[5],c[6]}, {v[5],n[5],c[5]} };
    GLuint indices[24] = { 0, 1, 2, 3,    4, 5, 6, 7,   8, 9,10,11,
                           12,13,14,15,  16,17,18,19,  20,21,22,23 };

    cube = vertex_buffer_new( "vertex:3f,normal:3f,color:4f" );
    vertex_buffer_push_back( cube, vertices, 24, indices, 24 );
    shader = shader_load("shaders/cube.vert","shaders/cube.frag");

    init( );
    glutMainLoop( );
    return EXIT_SUCCESS;
}
