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
#include "vertex-buffer.h"
#include "edtaa3func.h"
#include "markup.h"


// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
} vertex_t;


// ------------------------------------------------------- global variables ---
vertex_buffer_t *buffer;
texture_atlas_t * atlas = 0;
texture_atlas_t * atlas_smooth = 0;
GLuint program = 0;
GLuint smooth = 1;


// ------------------------------------------------------------ read_shader ---
char *
read_shader( const char *filename )
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
    buffer[size] = 0;
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
void display( void )
{
    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glActiveTexture(GL_TEXTURE0);
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    if( smooth )
    {
        glBindTexture(GL_TEXTURE_2D, atlas_smooth->id);
        glUseProgram( program );
        GLuint handle = glGetUniformLocation( program, "texture" );
        glUniform1i( handle, 0);
        vertex_buffer_render( buffer, GL_TRIANGLES, "vtc" );
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, atlas->id);
        vertex_buffer_render( buffer, GL_TRIANGLES, "vtc" );
    }
    glUseProgram( 0 );
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
    else if ( key == 32 )
    {
        smooth = 1 - smooth;
        glutPostRedisplay();
    }
}


// --------------------------------------------------------------- add_text ---
void add_text( vertex_buffer_t * buffer, wchar_t * text,
               texture_font_t *font, vec2 *pen, vec4 * color, float scale )
{
    size_t i;
    float r = color->red;
    float g = color->green;
    float b = color->blue;
    float a = color->alpha;

    for( i=0; i<wcslen(text); ++i )
    {
        if( text[i] == L'\n' )
        {
            continue;
        }

        texture_glyph_t *glyph = texture_font_get_glyph( font, text[i] );

        if( glyph != NULL )
        {
            float kerning = 0;
            if( i > 0)
            {
                kerning = texture_glyph_get_kerning( glyph, text[i-1] );
            }
            pen->x += kerning*scale;

            /* Actual glyph */
            float x0  = (int)( pen->x + glyph->offset_x*scale );
            float y0  = (int)( pen->y + glyph->offset_y*scale );
            float x1  = (int)( x0 + glyph->width*scale );
            float y1  = (int)( y0 - glyph->height*scale );
            float s0 = glyph->s0;
            float t0 = glyph->t0;
            float s1 = glyph->s1;
            float t1 = glyph->t1;
            GLuint index = buffer->vertices->size;
            GLuint indices[] = {index, index+1, index+2,
                                index, index+2, index+3};
            vertex_t vertices[] = {
                { (int)x0,y0,0,  s0,t0,  r,g,b,a },
                { (int)x0,y1,0,  s0,t1,  r,g,b,a },
                { (int)x1,y1,0,  s1,t1,  r,g,b,a },
                { (int)x1,y0,0,  s1,t0,  r,g,b,a } };

            vertex_buffer_push_back_indices( buffer, indices, 6 );
            vertex_buffer_push_back_vertices( buffer, vertices, 4 );
            pen->x += glyph->advance_x*scale;
        }
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
    glutInitWindowSize( 800, 800 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL width shaders" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    float fontsize = 20.0;
    atlas = texture_atlas_new( 512, 512, 1 );
    atlas_smooth = texture_atlas_new( 512, 512, 1 );
    texture_font_t *font = texture_font_new( atlas, "./Vera.ttf", fontsize );
    buffer = vertex_buffer_new( "v3f:t2f:c4f" );

    vec4 black = {{0,0,0,1}};
    vec4 red = {{1,0,0,1}};
    const wchar_t *cache = L" !\"#$%&'()*+,-./0123456789:;<=>?"
                           L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                           L"`abcdefghijklmnopqrstuvwxyz{|}~";
    texture_font_load_glyphs( font, cache );

    unsigned char *map;
    map = make_distance_map(atlas->data, atlas->width, atlas->height);
    memcpy( atlas_smooth->data, map, atlas->width*atlas->height*sizeof(unsigned char) );
    free(map);
    texture_atlas_upload( atlas_smooth );

    size_t i;
    vec2 pen = {{5.0, 785.0}};
    wchar_t *text = L"Type space to toggle distance fields rendering";
    vec4 color;
    for( i=0; i < 17; ++i)
    {
        float scale = (16+i*4)/fontsize;
        color = black;
        if( scale == 1.0 )
        {
            color = red;
        }
        add_text( buffer, text, font, &pen, &color, scale);
        pen.x = 5;
        pen.y -= font->height*scale;
    }
    // Create the GLSL program
    char * vertex_shader_source   = read_shader("./distance-field.vert");
    char * fragment_shader_source = read_shader("./distance-field-2.frag");
    program = build_program( vertex_shader_source, fragment_shader_source );

    glutMainLoop( );
    return 0;
}
