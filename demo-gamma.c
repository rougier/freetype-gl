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
#include "freetype-gl.h"
#include "vertex-buffer.h"
#include "markup.h"


// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, z;
    float u, v;
    float r, g, b, a;
    float shift, gamma;
} vertex_t;


// ------------------------------------------------------- global variables ---
texture_atlas_t * atlas;
vertex_buffer_t * buffer;
GLuint program = 0;
GLuint texture_location;
GLuint pixel_location;

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
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glDisable( GL_BLEND );
    glDisable( GL_TEXTURE_2D );
    glColor4f(1,1,1,1);
    glBegin(GL_QUADS);
    glVertex2i(0,0);
    glVertex2i(512,0);
    glVertex2i(512,256);
    glVertex2i(0,256);
    glEnd();

    glEnable( GL_BLEND );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_COLOR_MATERIAL );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

    glUseProgram( program );
    glUniform1i( texture_location, 0 );
    glUniform2f( pixel_location, 1.0/atlas->width, 1.0/atlas->height );
    vertex_buffer_render( buffer, GL_TRIANGLES, "vtc" );
    glUseProgram( 0 );
    glutSwapBuffers( );
}


// --------------------------------------------------------------- reshape ---
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
void keyboard( unsigned char key, int x, int y )
{
    if ( key == 27 )
    {
        exit( EXIT_SUCCESS );
    }
}


// --------------------------------------------------------------- add_text ---
void add_text( vertex_buffer_t * buffer, vec2 * pen, ... )
{
    markup_t *markup;
    wchar_t *text;
    va_list args;
    va_start ( args, pen ); 

    do {
        markup = va_arg( args, markup_t * );
        if( markup == NULL )
        {
            break;
        }
        text = va_arg( args, wchar_t * );

        size_t i;
        texture_font_t * font = markup->font;
        float r = markup->foreground_color.red;
        float g = markup->foreground_color.green;
        float b = markup->foreground_color.blue;
        float a = markup->foreground_color.alpha;
        float gamma = markup->gamma;

        for( i=0; i<wcslen(text); ++i )
        {
            texture_glyph_t *glyph = texture_font_get_glyph( font, text[i] );

            if( glyph != NULL )
            {
                float kerning = 0;
                if( i > 0)
                {
                    kerning = texture_glyph_get_kerning( glyph, text[i-1] );
                }
                pen->x += kerning;

                /* Actual glyph */
                float x0  = ( pen->x + glyph->offset_x );
                float y0  = (int)( pen->y + glyph->offset_y );
                float x1  = ( x0 + glyph->width );
                float y1  = (int)( y0 - glyph->height );
                float s0 = glyph->s0;
                float t0 = glyph->t0;
                float s1 = glyph->s1;
                float t1 = glyph->t1;
                GLuint index = buffer->vertices->size;
                GLuint indices[] = {index, index+1, index+2,
                                    index, index+2, index+3};
                vertex_t vertices[] = {
                    { (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma },
                    { (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma },
                    { (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma },
                    { (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma } };

                vertex_buffer_push_back_indices( buffer, indices, 6 );
                vertex_buffer_push_back_vertices( buffer, vertices, 4 );
                pen->x += glyph->advance_x;
            }

        }
    } while( markup != 0 );
    va_end ( args );
}


// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitWindowSize( 512, 512 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    size_t i;
    vec4 black  = {{0.0, 0.0, 0.0, 1.0}};
    vec4 white  = {{1.0, 1.0, 1.0, 1.0}};
    vec4 none   = {{1.0, 1.0, 1.0, 0.0}};
    markup_t markup = {
        .family  = "Bitstream Vera Sans",
        .size    = 15.0,
        .bold    = 0,
        .italic  = 0,
        .rise    = 0.0,
        .spacing = 0.0,
        .gamma   = 1.5,
        .foreground_color    = white,
        .background_color    = none,
        .underline           = 0,
        .underline_color     = white,
        .overline            = 0,
        .overline_color      = white,
        .strikethrough       = 0,
        .strikethrough_color = white,
        .font = 0,
    };

    atlas = texture_atlas_new( 512, 512, 3 );
    buffer = vertex_buffer_new( "v3f:t2f:c4f:1g1f:2g1f" ); 
    markup.font = texture_font_new( atlas, "./Vera.ttf", markup.size );

    vec2 pen;
    pen.y = 512.0 - markup.font->ascender - 5;
    for( i=0; i < 14; ++i )
    {
        pen.x = 25.0;
        markup.gamma = 0.75 + 1.5*i*(1.0/14);
        add_text( buffer, &pen,
                  &markup, L"The quick brown fox jumps over the lazy dog. ",
                  &markup, L"0123456789.", NULL);
        pen.y -= markup.font->height;
    }

    markup.foreground_color = black;
    pen.y = 256.0 - markup.font->ascender - 5;
    for( i=0; i < 14; ++i )
    {
        pen.x = 25.0;
        markup.gamma = 0.75 + 1.5*i*(1.0/14);
        add_text( buffer, &pen,
                  &markup, L"The quick brown fox jumps over the lazy dog. ",
                  &markup, L"0123456789.", NULL);
        pen.y -= markup.font->height;

    }



    // Create the GLSL program
    char * vertex_shader_source   = read_shader("./markup.vert");
    char * fragment_shader_source = read_shader("./markup.frag");
    program = build_program( vertex_shader_source, fragment_shader_source );
    texture_location = glGetUniformLocation(program, "texture");
    pixel_location   = glGetUniformLocation(program, "pixel");

    glutMainLoop( );
    return 0;
}
