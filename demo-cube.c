/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * API version: 1.0
 * WWW:         http://code.google.com/p/freetype-gl/
 * -------------------------------------------------------------------------
 * Copyright (c) 2011 Nicolas P. Rougier <Nicolas.Rougier@inria.fr>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 * ========================================================================= */
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <stdlib.h>
#include "window.h"
#include "vertex-buffer.h"

VertexBuffer *cube;
float theta=0, phi=0, fps=50;


void on_draw( Window *window )
{
    window_clear( window );

    theta += .5; phi += .5;
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
}

void on_resize( Window *window, int width, int height )
{
    glViewport(0, 0, width, height);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    gluPerspective( 45.0, width/(float) height, 2.0, 10.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    glTranslatef( 0.0, 0.0, -5.0 );
}

int main( int argc, char **argv )
{
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
    cube = vertex_buffer_new_from_data("v3f:n3f:c3f",
                                       24, vertices, 24, indices );
    Window *window = window_new( 400, 400, "Cube", 50.0 );

    window->on_draw = on_draw;
    window->on_resize = on_resize;
    window_run( window );

    return 0;
}
