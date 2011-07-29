/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * -------------------------------------------------------------------------
 * Copyright 2011 Nicolas P. Rougier. All rights reserved.
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
#include <stdlib.h>
#include "vector.h"
#include "text-markup.h"
#include "font-manager.h"
#include "texture-font.h"
#include "texture-glyph.h"
#include "vertex-buffer.h"


VertexBuffer *cube, *text_buffer;
FontManager *manager;


void init( void )
{
    GLfloat ambient[]  = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat position[] = {0.0f, 1.0f, 2.0f, 1.0f};
    GLfloat specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glPolygonOffset( 1, 1 );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glEnable( GL_DEPTH_TEST ); 
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_LIGHT0 );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
    glLightfv( GL_LIGHT0, GL_POSITION, position );
    glEnable( GL_LINE_SMOOTH );
}

void display( void )
{
    static float theta=0, phi=0;
    int viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    GLuint width  = viewport[2];
    GLuint height = viewport[3];
    static int frame=0, time, timebase=0;

    theta += .5; phi += .5;
	frame++;
	time = glutGet( GLUT_ELAPSED_TIME );

    if (time - timebase > 1000)
    {
        wchar_t fps[64]; 
        size_t i;
        Pen pen = {5.0, 5.0};
        TextMarkup markup = { "Arial", 64, 0, 0, 0.0, 0.0,
                              {.5,.5,.5,.5}, {1,1,1,0},
                              0, {0,0,0,1}, 0, {0,0,0,1},
                              0, {0,0,0,1}, 0, {0,0,0,1} };
        TextureFont *font= font_manager_get_from_markup( manager, &markup );
        TextureGlyph *glyph;
        swprintf( fps, 64, L"%.2f", frame*1000.0/(time-timebase) );
        timebase = time;
        frame = 0;

        vertex_buffer_clear( text_buffer );
        glyph = texture_font_get_glyph( font, fps[0] );
        texture_glyph_add_to_vertex_buffer( glyph, text_buffer, &markup, &pen );
        for( i=1; i<wcslen(fps); ++i )
        {
            glyph = texture_font_get_glyph( font, fps[i] );
            pen.x += texture_glyph_get_kerning( glyph, fps[i-1] );
            texture_glyph_add_to_vertex_buffer( glyph, text_buffer, &markup, &pen );
        }
    }

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPushMatrix();
    glRotatef( theta, 0,0,1 );
    glRotatef( phi,   0,1,0 );
    glDisable( GL_BLEND );
    glEnable( GL_LIGHTING );
    glEnable( GL_DEPTH_TEST );
    glColor4f( 1, 1, 1, 1 );
    glEnable( GL_POLYGON_OFFSET_FILL );

    /* Use vertice (v), normal(n) and color (c) */
    vertex_buffer_render( cube, GL_QUADS, "vnc" );
    glDisable( GL_POLYGON_OFFSET_FILL );
    glEnable( GL_BLEND );
    glDisable( GL_LIGHTING );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glDepthMask( GL_FALSE );

    /* Use vertice (v) only */
    glColor4f( 0, 0, 0, .5 );
    vertex_buffer_render( cube, GL_QUADS, "v" );
    glDepthMask( GL_TRUE );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glPopMatrix();

    /* Display FPS */
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, manager->atlas->texid );
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    vertex_buffer_render( text_buffer, GL_TRIANGLES, "vtc" );
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glDisable( GL_TEXTURE_2D );

    glutSwapBuffers( );
}

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

void keyboard( unsigned char key, int x, int y )
{
    if ( key == 27 )
    {
        exit( EXIT_SUCCESS );
    }
}

void timer( int dt )
{
    glutPostRedisplay();
    glutTimerFunc( dt, timer, dt );
}

void idle( void )
{
    glutPostRedisplay();
}


int main( int argc, char **argv )
{
    int fps = 50;
    typedef struct { float x,y,z; } vec3;
    typedef struct { vec3 position, normal, color;} vertex;
    vec3 v[] = { { 1, 1, 1},  {-1, 1, 1},  {-1,-1, 1}, { 1,-1, 1},
                 { 1,-1,-1},  { 1, 1,-1},  {-1, 1,-1}, {-1,-1,-1} };
    vec3 n[] = { { 0, 0, 1},  { 1, 0, 0},  { 0, 1, 0} ,
                 {-1, 0, 1},  { 0,-1, 0},  { 0, 0,-1} };
    vec3 c[] = { {1, 1, 1},  {1, 1, 0},  {1, 0, 1},  {0, 1, 1},
                 {1, 0, 0},  {0, 0, 1},  {0, 1, 0},  {0, 0, 0} };
    vertex vertices[24] =  {
      {v[0],n[0],c[0]}, {v[1],n[0],c[1]}, {v[2],n[0],c[2]}, {v[3],n[0],c[3]},
      {v[0],n[1],c[0]}, {v[3],n[1],c[3]}, {v[4],n[1],c[4]}, {v[5],n[1],c[5]},
      {v[0],n[2],c[0]}, {v[5],n[2],c[5]}, {v[6],n[2],c[6]}, {v[1],n[2],c[1]},
      {v[1],n[3],c[1]}, {v[6],n[3],c[6]}, {v[7],n[3],c[7]}, {v[2],n[3],c[2]},
      {v[7],n[4],c[7]}, {v[4],n[4],c[4]}, {v[3],n[4],c[3]}, {v[2],n[4],c[2]},
      {v[4],n[5],c[4]}, {v[7],n[5],c[7]}, {v[6],n[5],c[6]}, {v[5],n[5],c[5]} };
    GLuint indices[24] = { 0, 1, 2, 3,    4, 5, 6, 7,   8, 9,10,11,
                           12,13,14,15,  16,17,18,19,  20,21,22,23 };
    cube = vertex_buffer_new_from_data( "v3f:n3f:c3f",
                                        24, vertices, 24, indices );
    manager = font_manager_new();
    text_buffer = vertex_buffer_new( "v3i:t2f:c4f" ); 

    glutInit( &argc, argv );
    glutInitWindowSize( 400, 400 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Vertex buffer cube" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutTimerFunc( 1000/fps, timer, 1000/fps );
    //glutIdleFunc( idle );
    init( );
    glutMainLoop( );
    return EXIT_SUCCESS;
}
