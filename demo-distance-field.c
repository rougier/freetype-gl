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
#include "freetype-gl.h"

#include "edtaa3func.h"
#include "font-manager.h"
#include "vertex-buffer.h"
#include "text-buffer.h"
#include "markup.h"
#include "shader.h"

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))


// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, zoom;
} viewport_t;

// ------------------------------------------------------- global variables ---
texture_atlas_t * atlas = 0;
viewport_t viewport = {0,0,1};
GLuint program = 0;


// ---------------------------------------------------------------- display ---
void
display( void )
{
    int v[4];
    glGetIntegerv( GL_VIEWPORT, v );
    GLuint width  = v[2];
    GLuint height = v[3];
    glClearColor(0.5,0.5,0.5,1.00);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas->id);
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    GLuint handle;
    handle = glGetUniformLocation( program, "texture" );
    glUniform1i( handle, 0);

    int x = viewport.x;
    int y = viewport.y;
    width *= viewport.zoom;
    height *= viewport.zoom;

    glColor4f( 1.0, 1.0, 1.0, 1.0 );
    glPushMatrix();
    glBegin(GL_QUADS);
    glTexCoord2f( 0, 1 ); glVertex2i( x, y );
    glTexCoord2f( 0, 0 ); glVertex2i( x, y+height );
    glTexCoord2f( 1, 0 ); glVertex2i( x+width, y+height );
    glTexCoord2f( 1, 1 ); glVertex2i( x+width, y );
    glEnd();

    glPopMatrix();
    glutSwapBuffers( );
}


// ----------------------------------------------------------- mouse_motion ---
void
mouse_motion( int x, int y )
{
    int v[4];
    glGetIntegerv( GL_VIEWPORT, v );
    GLfloat width = v[2], height = v[3];
    float nx = min( max( x/width, 0.0), 1.0 );
    float ny = 1-min( max( y/height, 0.0), 1.0 );
    viewport.x = nx*width*(1-viewport.zoom);
    viewport.y = ny*height*(1-viewport.zoom);
    glutPostRedisplay();
}


// ------------------------------------------------------------- mouse_drag ---
void
mouse_drag( int x, int y )
{
    static int _x=-1, _y=-1;
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
    mouse_motion(x,y);
}


// ---------------------------------------------------------------- reshape ---
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}


// --------------------------------------------------------------- keyboard ---
void
keyboard( unsigned char key, int x, int y )
{
    if ( key == 27 )
    {
        exit( 1 );
    }
}


// ------------------------------------------------------ make_distance_map ---
unsigned char *
make_distance_map( unsigned char *img,
                   unsigned int width, unsigned int height )
{
    short * xdist = (short *)  malloc( width * height * sizeof(short) );
    short * ydist = (short *)  malloc( width * height * sizeof(short) );
    double * gx   = (double *) calloc( width * height, sizeof(double) );
    double * gy      = (double *) calloc( width * height, sizeof(double) );
    double * data    = (double *) calloc( width * height, sizeof(double) );
    double * outside = (double *) calloc( width * height, sizeof(double) );
    double * inside  = (double *) calloc( width * height, sizeof(double) );
    int i;

    // Convert img into double (data)
    double img_min = 255, img_max = -255;
    for( i=0; i<width*height; ++i)
    {
        double v = img[i];
        data[i] = v;
        if (v > img_max) img_max = v;
        if (v < img_min) img_min = v;
    }
    // Rescale image levels between 0 and 1
    for( i=0; i<width*height; ++i)
    {
        data[i] = (img[i]-img_min)/img_max;
    }

    // Compute outside = edtaa3(bitmap); % Transform background (0's)
    computegradient( data, height, width, gx, gy);
    edtaa3(data, gx, gy, height, width, xdist, ydist, outside);
    for( i=0; i<width*height; ++i)
        if( outside[i] < 0 )
            outside[i] = 0.0;

    // Compute inside = edtaa3(1-bitmap); % Transform foreground (1's)
    memset(gx, 0, sizeof(double)*width*height );
    memset(gy, 0, sizeof(double)*width*height );
    for( i=0; i<width*height; ++i)
        data[i] = 1 - data[i];
    computegradient( data, height, width, gx, gy);
    edtaa3(data, gx, gy, height, width, xdist, ydist, inside);
    for( i=0; i<width*height; ++i)
        if( inside[i] < 0 )
            inside[i] = 0.0;

    // distmap = outside - inside; % Bipolar distance field
    unsigned char *out = (unsigned char *) malloc( width * height * sizeof(unsigned char) );
    for( i=0; i<width*height; ++i)
    {
        outside[i] -= inside[i];
        outside[i] = 128+outside[i]*16;
        if( outside[i] < 0 ) outside[i] = 0;
        if( outside[i] > 255 ) outside[i] = 255;
        out[i] = 255 - (unsigned char) outside[i];
        //out[i] = (unsigned char) outside[i];
    }

    free( xdist );
    free( ydist );
    free( gx );
    free( gy );
    free( data );
    free( outside );
    free( inside );
    return out;
}




// ------------------------------------------------------------------- main ---
int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitWindowSize( 512, 512 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL width shaders" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutMotionFunc( mouse_drag );
    glutPassiveMotionFunc( mouse_motion );
    glutKeyboardFunc( keyboard );


    unsigned char *map;
    texture_font_t * font;
    const char *filename = "fonts/Vera.ttf";
    const wchar_t *cache = L" !\"#$%&'()*+,-./0123456789:;<=>?"
                           L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                           L"`abcdefghijklmnopqrstuvwxyz{|}~";

    atlas = texture_atlas_new( 512, 512, 1 );
    font = texture_font_new( atlas, filename, 72 );
    texture_font_load_glyphs( font, cache );
    texture_font_delete( font );

    fprintf( stderr, "Generating distance map...\n" );
    map = make_distance_map(atlas->data, atlas->width, atlas->height);
    fprintf( stderr, "done !\n");

    memcpy( atlas->data, map, atlas->width*atlas->height*sizeof(unsigned char) );
    free(map);
    texture_atlas_upload( atlas );

    // Create the GLSL program
    program = shader_load( "shaders/distance-field.vert",
                           "shaders/distance-field.frag" );
    glUseProgram( program );

    glutMainLoop( );
    return 0;
}
