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
#include <ft2build.h>
#include FT_FREETYPE_H
#include "edtaa3func.h"
#include "shader.h"
#include "texture-font.h"
#include "texture-atlas.h"
#include "platform.h"

#if defined(__APPLE__)
    #include <Glut/glut.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <GLUT/glut.h>
	// INFINITY only defined in C99
	#define INFINITY 99999999999
#else
    #include <GL/glut.h>
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif


// ------------------------------------------------------- global variables ---
float angle = 0;
GLuint program = 0;
texture_font_t * font = 0;
texture_atlas_t * atlas = 0;

// ------------------------------------------------------ make_distance_map ---
void
distance_map( double *data, unsigned int width, unsigned int height )
{
    short * xdist = (short *)  malloc( width * height * sizeof(short) );
    short * ydist = (short *)  malloc( width * height * sizeof(short) );
    double * gx   = (double *) calloc( width * height, sizeof(double) );
    double * gy      = (double *) calloc( width * height, sizeof(double) );
    double * outside = (double *) calloc( width * height, sizeof(double) );
    double * inside  = (double *) calloc( width * height, sizeof(double) );
    int i;

    // Compute outside = edtaa3(bitmap); % Transform background (0's)
    computegradient( data, width, height, gx, gy);
    edtaa3(data, gx, gy, width, height, xdist, ydist, outside);
    for( i=0; i<width*height; ++i)
    {
        if( outside[i] < 0.0 )
        {
            outside[i] = 0.0;
        }
    }

    // Compute inside = edtaa3(1-bitmap); % Transform foreground (1's)
    memset( gx, 0, sizeof(double)*width*height );
    memset( gy, 0, sizeof(double)*width*height );
    for( i=0; i<width*height; ++i)
        data[i] = 1 - data[i];
    computegradient( data, width, height, gx, gy );
    edtaa3( data, gx, gy, width, height, xdist, ydist, inside );
    for( i=0; i<width*height; ++i )
    {
        if( inside[i] < 0 )
        {
            inside[i] = 0.0;
        }
    }

    // distmap = outside - inside; % Bipolar distance field
    float vmin = +INFINITY;
    for( i=0; i<width*height; ++i)
    {
        outside[i] -= inside[i];
        if( outside[i] < vmin )
        {
            vmin = outside[i];
        }
    }
    vmin = abs(vmin);
    for( i=0; i<width*height; ++i)
    {
        float v = outside[i];
        if     ( v < -vmin) outside[i] = -vmin;
        else if( v > +vmin) outside[i] = +vmin;
        data[i] = (outside[i]+vmin)/(2*vmin);
    }

    free( xdist );
    free( ydist );
    free( gx );
    free( gy );
    free( outside );
    free( inside );
}


// ------------------------------------------------------ MitchellNetravali ---
// Mitchell Netravali reconstruction filter
float
MitchellNetravali( float x )
{
    const float B = 1/3.0, C = 1/3.0; // Recommended
    // const float B =   1.0, C =   0.0; // Cubic B-spline (smoother results)
    // const float B =   0.0, C = 1/2.0; // Catmull-Rom spline (sharper results)
    x = fabs(x);
    if( x < 1 )
         return ( ( 12 -  9 * B - 6 * C) * x * x * x
                + (-18 + 12 * B + 6 * C) * x * x
                + (  6 -  2 * B) ) / 6;
    else if( x < 2 )
        return ( (     -B -  6 * C) * x * x * x
               + (  6 * B + 30 * C) * x * x
               + (-12 * B - 48 * C) * x
               + (  8 * B + 24 * C) ) / 6;
    else
        return 0;
}


// ------------------------------------------------------------ interpolate ---
float
interpolate( float x, float y0, float y1, float y2, float y3 )
{
    float c0 = MitchellNetravali(x-1);
    float c1 = MitchellNetravali(x  );
    float c2 = MitchellNetravali(x+1);
    float c3 = MitchellNetravali(x+2);
    float r =  c0*y0 + c1*y1 + c2*y2 + c3*y3;
    return min( max( r, 0.0 ), 1.0 );
}


// ------------------------------------------------------------------ scale ---
int
resize( double *src_data, size_t src_width, size_t src_height,
        double *dst_data, size_t dst_width, size_t dst_height )
{
    if( (src_width == dst_width) && (src_height == dst_height) )
    {
        memcpy( dst_data, src_data, src_width*src_height*sizeof(double));
        return 0;
    }
    size_t i,j;
    float xscale = src_width / (float) dst_width;
    float yscale = src_height / (float) dst_height;
    for( j=0; j < dst_height; ++j )
    {
        for( i=0; i < dst_width; ++i )
        {
            int src_i = (int) floor( i * xscale );
            int src_j = (int) floor( j * yscale );
            int i0 = min( max( 0, src_i-1 ), src_width-1 );
            int i1 = min( max( 0, src_i   ), src_width-1 );
            int i2 = min( max( 0, src_i+1 ), src_width-1 );
            int i3 = min( max( 0, src_i+2 ), src_width-1 );
            int j0 = min( max( 0, src_j-1 ), src_height-1 );
            int j1 = min( max( 0, src_j   ), src_height-1 );
            int j2 = min( max( 0, src_j+1 ), src_height-1 );
            int j3 = min( max( 0, src_j+2 ), src_height-1 );
            float t0 = interpolate( i / (float) dst_width,
                                    src_data[j0*src_width+i0],
                                    src_data[j0*src_width+i1],
                                    src_data[j0*src_width+i2],
                                    src_data[j0*src_width+i3] );
            float t1 = interpolate( i / (float) dst_width,
                                    src_data[j1*src_width+i0],
                                    src_data[j1*src_width+i1],
                                    src_data[j1*src_width+i2],
                                    src_data[j1*src_width+i3] );
            float t2 = interpolate( i / (float) dst_width,
                                    src_data[j2*src_width+i0],
                                    src_data[j2*src_width+i1],
                                    src_data[j2*src_width+i2],
                                    src_data[j2*src_width+i3] );
            float t3 = interpolate( i / (float) dst_width,
                                    src_data[j3*src_width+i0],
                                    src_data[j3*src_width+i1],
                                    src_data[j3*src_width+i2],
                                    src_data[j3*src_width+i3] );
            float y =  interpolate( j / (float) dst_height, t0, t1, t2, t3 );
            dst_data[j*dst_width+i] = y;
        }
    }
    return 0;
}






// ---------------------------------------------------------------- display ---
void
display( void )
{
    glClearColor(1.0,1.0,1.0,1.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas->id);
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    GLuint handle = glGetUniformLocation( program, "texture" );
    glUniform1i( handle, 0);

    texture_glyph_t * glyph = texture_font_get_glyph( font, L'@');

    float s0 = glyph->s0;
    float t0 = glyph->t0;
    float s1 = glyph->s1;
    float t1 = glyph->t1;

    int width = 512;
    int height = 512;
    if( glyph->width > glyph->height )
        height = glyph->height * width/(float)glyph->width;
    else
        width = glyph->width * height/(float)glyph->height;
    int x = 0 - width/2;
    int y = 0 - height/2;

    glPushMatrix();
    glTranslatef(256,256,0);
    glRotatef(angle, 0,0,1);
    float s = .025+.975*(1+cos(angle/100.0))/2.;
    glScalef(s,s,s);

    glBegin(GL_QUADS);
    glTexCoord2f( s0, t1 ); glVertex2f( x, y );
    glTexCoord2f( s0, t0 ); glVertex2f( x, y+height );
    glTexCoord2f( s1, t0 ); glVertex2f( x+width, y+height );
    glTexCoord2f( s1, t1 ); glVertex2f( x+width, y );
    glEnd();
    glPopMatrix();

    glutSwapBuffers( );
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
    glutPostRedisplay();
}

// ------------------------------------------------------------------ timer ---
void timer( int fps )
{
    glutPostRedisplay();
    glutTimerFunc( 1000.0/fps, timer, fps );
    angle += .5;
}


// ------------------------------------------------------------- load_glyph ---
texture_glyph_t *
load_glyph( const char *  filename,     const wchar_t charcode,
            const float   highres_size, const float   lowres_size,
            const float   padding )
{
    size_t i, j;
    FT_Library library;
    FT_Face face;

    FT_Init_FreeType( &library );
    FT_New_Face( library, filename, 0, &face );
    FT_Select_Charmap( face, FT_ENCODING_UNICODE );
    FT_UInt glyph_index = FT_Get_Char_Index( face, charcode );

    // Render glyph at high resolution (highres_size points)
    FT_Set_Char_Size( face, highres_size*64, 0, 72, 72 );
    FT_Load_Glyph( face, glyph_index,
                   FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT);
    FT_GlyphSlot slot = face->glyph;
    FT_Bitmap bitmap = slot->bitmap;

    // Allocate high resolution buffer
    size_t highres_width  = bitmap.width + 2*padding*highres_size;
    size_t highres_height = bitmap.rows + 2*padding*highres_size;
    double * highres_data = (double *) malloc( highres_width*highres_height*sizeof(double) );
    memset( highres_data, 0, highres_width*highres_height*sizeof(double) );

    // Copy high resolution bitmap with padding and normalize values
    for( j=0; j < bitmap.rows; ++j )
    {
        for( i=0; i < bitmap.width; ++i )
        {
            int x = i + padding;
            int y = j + padding;
            highres_data[y*highres_width+x] = bitmap.buffer[j*bitmap.width+i]/255.0;
        }
    }

    // Compute distance map
    distance_map( highres_data, highres_width, highres_height );

    // Allocate low resolution buffer
    size_t lowres_width  = round(highres_width * lowres_size/highres_size);
    size_t lowres_height = round(highres_height * lowres_width/(float) highres_width);
    double * lowres_data = (double *) malloc( lowres_width*lowres_height*sizeof(double) );
    memset( lowres_data, 0, lowres_width*lowres_height*sizeof(double) );

    // Scale down highres buffer into lowres buffer
    resize( highres_data, highres_width, highres_height,
            lowres_data,  lowres_width,  lowres_height );

    // Convert the (double *) lowres buffer into a (unsigned char *) buffer and
    // rescale values between 0 and 255.
    unsigned char * data =
        (unsigned char *) malloc( lowres_width*lowres_height*sizeof(unsigned char) );
    for( j=0; j < lowres_height; ++j )
    {
        for( i=0; i < lowres_width; ++i )
        {
            double v = lowres_data[j*lowres_width+i];
            data[j*lowres_width+i] = (int) (255*(1-v));
        }
    }

    // Compute new glyph information from highres value
    float ratio = lowres_size / highres_size;
    size_t pitch  = lowres_width * sizeof( unsigned char );

    // Create glyph
    texture_glyph_t * glyph = texture_glyph_new( );
    glyph->offset_x = (slot->bitmap_left + padding*highres_width) * ratio;
    glyph->offset_y = (slot->bitmap_top + padding*highres_height) * ratio;
    glyph->width    = lowres_width;
    glyph->height   = lowres_height;
    glyph->charcode = charcode;
    /*
    printf( "Glyph width:  %ld\n", glyph->width );
    printf( "Glyph height: %ld\n", glyph->height );
    printf( "Glyph offset x: %d\n", glyph->offset_x );
    printf( "Glyph offset y: %d\n", glyph->offset_y );
    */
    ivec4 region = texture_atlas_get_region( atlas, glyph->width, glyph->height );
    /*
    printf( "Region x : %d\n", region.x );
    printf( "Region y : %d\n", region.y );
    printf( "Region width : %d\n", region.width );
    printf( "Region height : %d\n", region.height );
    */
    texture_atlas_set_region( atlas, region.x, region.y, glyph->width, glyph->height, data, pitch );
    glyph->s0       = region.x/(float)atlas->width;
    glyph->t0       = region.y/(float)atlas->height;
    glyph->s1       = (region.x + glyph->width)/(float)atlas->width;
    glyph->t1       = (region.y + glyph->height)/(float)atlas->height;

    FT_Load_Glyph( face, glyph_index,
                   FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT);
    glyph->advance_x = ratio * face->glyph->advance.x/64.0;
    glyph->advance_y = ratio * face->glyph->advance.y/64.0;
    /*
    printf( "Advance x : %f\n", glyph->advance_x );
    printf( "Advance y : %f\n", glyph->advance_y );
    */
    free( highres_data );
    free( lowres_data );
    free( data );

    return glyph;
}


// ------------------------------------------------------------------- main ---
int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitWindowSize( 512, 512 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Distance fields demo" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutTimerFunc( 1000.0/60, timer, 60 );
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
    program = shader_load( "shaders/distance-field.vert",
                           "shaders/distance-field-3.frag" );
    glUseProgram( program );
    atlas = texture_atlas_new( 512, 512, 1 );
    font = texture_font_new_from_file( atlas, 32, "fonts/Vera.ttf" );

    texture_glyph_t *glyph;

    // Generate the glyp at 512 points, compute distance field and scale it
    // back to 32 points
    // Just load another glyph if you want to see difference (draw render a '@')
    glyph = load_glyph( "fonts/Vera.ttf", L'@', 512, 64, 0.1);
    vector_push_back( font->glyphs, &glyph );

    texture_atlas_upload( atlas );

    glutMainLoop( );
    return 0;
}
