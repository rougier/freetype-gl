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
#include <GL/glew.h>
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "font-manager.h"
#include "texture-font.h"
#include "texture-atlas.h"
#include "edtaa3func.h"


// ----------------------------------------------------------------- macros ---
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

// ------------------------------------------------------- typedef & struct ---
typedef struct { float x, y, zoom; } Viewport;

// ------------------------------------------------------- global variables ---
TextureAtlas *atlas = 0;
Viewport viewport = {0,0,1};
GLuint program = 0;
GLuint texid = 0;


// ----------------------------------------------------------- build_shader ---
char *
load_shader( const char *filename )
{
    FILE * file;
    char * buffer;
	size_t size;

    file = fopen( filename, "rb" );
    if( !file )
    {
        fprintf( stderr, "Unable to open file \"%s\".\n", filename );
		return 0;
    }
	fseek( file, 0, SEEK_END );
	size = ftell( file );
	fseek(file, 0, SEEK_SET );
    buffer = (char *) malloc( (size+1) * sizeof( char *) );
	fread( buffer, sizeof(char), size, file );
    fclose( file );
    return buffer;
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
GLuint build_program( const char * vertex_source,
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

// ---------------------------------------------------------------- display ---
void
display( void )
{
    int v[4];
    glGetIntegerv( GL_VIEWPORT, v );
    GLuint width  = v[2];
    GLuint height = v[3];
    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texid);
    glBindTexture(GL_TEXTURE_2D, atlas->texid);
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    //glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

    GLuint handle;
    handle = glGetUniformLocation( program, "texture" );
    glUniform1i( handle, 0);
    handle = glGetUniformLocation( program, "AlphaTest" );
    glUniform1f( handle, 2.0);
    handle = glGetUniformLocation( program, "GlyphColor" );
    glUniform3f( handle, 1.0, 0.0, 0.0);

    int x = viewport.x;
    int y = viewport.y;
    width *= viewport.zoom;
    height *= viewport.zoom;
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

// --------------------------------------------------------------- TGA_read ---
unsigned char *
TGA_read( char *filename,
          unsigned int * width,
          unsigned int * height,
          unsigned int * depth )
{
	FILE *file;
	unsigned char type[4],  info[6];
    file = fopen( filename, "rb" );
    if( !file )
    {
        fprintf( stderr, "Unable to open file.\n" );
		return 0;
    }
	fread( &type, sizeof (char), 3, file );
	fseek( file, 12, SEEK_SET );
	fread( &info, sizeof (char), 6, file );

	/* Only image type (color) or 3 (greyscale) */
	if( type[1] != 0 || (type[2] != 2 && type[2] != 3) )
	{
        fprintf( stderr, "Unhandled image type.\n" );
		fclose( file );
		return 0;
	}

	*width  = info[0] + info[1] * 256;
	*height = info[2] + info[3] * 256;
	*depth  = info[4] / 8;

    if( *depth != 1 && *depth != 3 && *depth != 4 )
    {
        fprintf( stderr, "Unknown color depth (%d).\n", *depth*8 );
		fclose(file);
		return 0;
	}

	// Allocate memory for image data
	unsigned long size =  (*height) * (*width) * (*depth);
	unsigned char *data = (unsigned char *) malloc( size * sizeof(unsigned char) );

	// Read in image data
	fread( data, sizeof(unsigned char), size, file );
	fclose( file );

	return data;
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

    int bold   = 0;
    int italic = 0;
    char * family = "Bitstream Vera Sans";
    float minsize = 64, maxsize = 65;
    size_t count = maxsize - minsize;
    wchar_t *cache = L" !\"#$%&'()*+,-./0123456789:;<=>?"
                     L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                     L"`abcdefghijklmnopqrstuvwxyz{|}~";
    TextureFont *font;
    char * filename;
    size_t i, missed = 0;
    atlas = texture_atlas_new( 512, 512 );
    for( i=0; i < count; ++i)
    {
        filename = font_manager_match_description( 0, family, minsize+i, bold, italic );
        font = texture_font_new( atlas, filename, minsize+i );
        font->border = 5;
        missed += texture_font_cache_glyphs( font, cache );
        texture_font_delete(font);
    }

    printf( "Matched font               : %s\n", filename );
    printf( "Number of fonts            : %ld\n", count );
    printf( "Number of glyphs per font  : %ld\n", wcslen(cache) );
    printf( "Number of missed glyphs    : %ld\n", missed );
    printf( "Total number of glyphs     : %ld/%ld\n",
            wcslen(cache)*count - missed, wcslen(cache)*count );
    printf( "Texture size               : %ldx%ld\n", atlas->width, atlas->height );
    printf( "Texture occupancy          : %.2f%%\n\n", 
            100.0*atlas->used/(float)(atlas->width*atlas->height) );


    printf( "Generating distance map...\n" );
    unsigned char *map = make_distance_map(atlas->data, atlas->width, atlas->height);
    printf( "done !\n");
    memcpy(atlas->data, map, atlas->width*atlas->height*sizeof(unsigned char));
    free(map);
    texture_atlas_upload(atlas);


    // glew initialization and OpenGL version checking
	glewInit( );
	if( ! glewIsSupported("GL_VERSION_2_0") )
    {
		fprintf( stderr, "OpenGL 2.0 not supported\n" );
		exit( EXIT_FAILURE );
	}

    // Create the GLSL program
    char * vertex_shader_source   = load_shader("./distance-field.vert");
    char * fragment_shader_source = load_shader("./distance-field.frag");
    program = build_program( vertex_shader_source, fragment_shader_source );
    glUseProgram( program );

    glutMainLoop( );
    return 0;
}
