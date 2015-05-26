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
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "freetype-gl.h"
#include "edtaa3func.h"
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


// ------------------------------------------------------- global variables ---
GLuint shader;
vertex_buffer_t *buffer;
texture_atlas_t * atlas = 0;
mat4  model, view, projection;


// ---------------------------------------------------------------- display ---
void display( void )
{
    glClearColor( 1, 1, 1, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    GLint width  = viewport[2];
    GLint height = viewport[3];

    srand(4);
    vec4 color = {{0.067,0.333, 0.486, 1.0}};
    size_t i;
    for( i=0; i<40; ++i)
    {
        float scale = .25 + 4.75 * pow(rand()/(float)(RAND_MAX),2);
        float angle = 90*(rand()%2);
        float x = (.05 + .9*(rand()/(float)(RAND_MAX)))*width;
        float y = (-.05 + .9*(rand()/(float)(RAND_MAX)))*height;
        float a =  0.1+.8*(pow((1.0-scale/5),2));

        mat4_set_identity( &model );
        mat4_rotate( &model, angle,0,0,1);
        mat4_scale( &model, scale, scale, 1);
        mat4_translate( &model, x, y, 0);

        glUseProgram( shader );
        {
            glUniform1i( glGetUniformLocation( shader, "texture" ),
                         0 );
            glUniform4f( glGetUniformLocation( shader, "Color" ),
                         color.r, color.g, color.b, a);
            glUniformMatrix4fv( glGetUniformLocation( shader, "model" ),
                                1, 0, model.data);
            glUniformMatrix4fv( glGetUniformLocation( shader, "view" ),
                                1, 0, view.data);
            glUniformMatrix4fv( glGetUniformLocation( shader, "projection" ),
                                1, 0, projection.data);
            vertex_buffer_render( buffer, GL_TRIANGLES );
        }
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
        exit( 1 );
    }
}


// --------------------------------------------------------------- add_text ---
vec4
add_text( vertex_buffer_t * buffer, texture_font_t * font,
          wchar_t * text, vec4 * color, vec2 * pen )
{
    vec4 bbox = {{0,0,0,0}};
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
            GLuint indices[6] = {0,1,2, 0,2,3};
            vertex_t vertices[4] = { { x0,y0,0,  s0,t0,  r,g,b,a },
                                     { x0,y1,0,  s0,t1,  r,g,b,a },
                                     { x1,y1,0,  s1,t1,  r,g,b,a },
                                     { x1,y0,0,  s1,t0,  r,g,b,a } };
            vertex_buffer_push_back( buffer, vertices, 4, indices, 6 );
            pen->x += glyph->advance_x;

            if  (x0 < bbox.x)                bbox.x = x0;
            if  (y1 < bbox.y)                bbox.y = y1;
            if ((x1 - bbox.x) > bbox.width)  bbox.width  = x1-bbox.x;
            if ((y0 - bbox.y) > bbox.height) bbox.height = y0-bbox.y;
        }
    }
    return bbox;
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
    computegradient( data, width, height, gx, gy);
    edtaa3(data, gx, gy, width, height, xdist, ydist, outside);
    for( i=0; i<width*height; ++i)
        if( outside[i] < 0 )
            outside[i] = 0.0;

    // Compute inside = edtaa3(1-bitmap); % Transform foreground (1's)
    memset(gx, 0, sizeof(double)*width*height );
    memset(gy, 0, sizeof(double)*width*height );
    for( i=0; i<width*height; ++i)
        data[i] = 1 - data[i];
    computegradient( data, width, height, gx, gy);
    edtaa3(data, gx, gy, width, height, xdist, ydist, inside);
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
    glutInitWindowSize( 800, 600 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Signed Distance Field" );
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
    texture_font_t *font = 0;
    texture_atlas_t *atlas = texture_atlas_new( 512, 512, 1 );
    const char * filename = "fonts/Vera.ttf";
    wchar_t *text = L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789";
    buffer = vertex_buffer_new( "vertex:3f,tex_coord:2f,color:4f" );
    vec2 pen = {{0,0}};
    vec4 black = {{1,1,1,1}};
    font = texture_font_new_from_file( atlas, 48, filename );
    vec4 bbox = add_text( buffer, font, text, &black, &pen );
    size_t i;
    vector_t * vertices = buffer->vertices;
    for( i=0; i< vector_size(vertices); ++i )
    {
        vertex_t * vertex = (vertex_t *) vector_get(vertices,i);
        vertex->x -= (int)(bbox.x + bbox.width/2);
        vertex->y -= (int)(bbox.y + bbox.height/2);
    }


    glBindTexture( GL_TEXTURE_2D, atlas->id );

    fprintf( stderr, "Generating distance map...\n" );
    unsigned char *map = make_distance_map(atlas->data, atlas->width, atlas->height);
    fprintf( stderr, "done !\n");

    memcpy( atlas->data, map, atlas->width*atlas->height*sizeof(unsigned char) );
    free(map);
    texture_atlas_upload( atlas );

    shader = shader_load( "shaders/distance-field-2.vert",
                          "shaders/distance-field-2.frag" );
    mat4_set_identity( &projection );
    mat4_set_identity( &model );
    mat4_set_identity( &view );

    glutMainLoop( );
    return 0;
}
