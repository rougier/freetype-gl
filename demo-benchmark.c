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
#include "freetype-gl.h"
#include "vertex-buffer.h"


// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
} vertex_t;


// ------------------------------------------------------- global variables ---
texture_atlas_t * atlas;
texture_font_t * font;
vertex_buffer_t * buffer;
wchar_t *text =
    L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789 "
    L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789 ";
int line_count = 42;



// --------------------------------------------------------------- add_text ---
void add_text( vertex_buffer_t * buffer, texture_font_t * font,
               wchar_t * text, vec4 * color, vec2 * pen )
{
    size_t i;
    float r = color->red, g = color->green, b = color->blue, a = color->alpha;
    for( i=0; i<wcslen(text); ++i )
    {
        texture_glyph_t *glyph = texture_font_get_glyph( font, text[i] );
        if( glyph != NULL )
        {
            int kerning = 0;
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
            GLuint index = buffer->vertices->size;
            GLuint indices[] = {index, index+1, index+2,
                                index, index+2, index+3};
            vertex_t vertices[] = { { x0,y0,0,  s0,t0,  r,g,b,a },
                                    { x0,y1,0,  s0,t1,  r,g,b,a },
                                    { x1,y1,0,  s1,t1,  r,g,b,a },
                                    { x1,y0,0,  s1,t0,  r,g,b,a } };
            vertex_buffer_push_back_indices( buffer, indices, 6 );
            vertex_buffer_push_back_vertices( buffer, vertices, 4 );
            pen->x += glyph->advance_x;
        }
    }
}


// ---------------------------------------------------------------- display ---
void display( void )
{
    static int frame=0, time, timebase=0;
    static int count = 0;

    if( count == 0 && frame == 0 )
    {
        printf(
            "Computing FPS with text generation and rendering at each frame...\n" );
        printf(
            "Number of glyphs: %d\n", (int)wcslen(text)*line_count );
    }

	frame++;
	time = glutGet( GLUT_ELAPSED_TIME );

    if( time-timebase > (2500) )
    {
        printf( "FPS : %.2f (%d frames in %.2f second, %.1f glyph/second)\n",
                frame*1000.0/(time-timebase), frame, (time-timebase)/1000.0,
                frame*1000.0/(time-timebase) * wcslen(text)*line_count );
        timebase = time;
        frame = 0;
        ++count;
        if( count == 5 )
        {
            printf( "\nComputing FPS with text rendering at each frame...\n" );
            printf( "Number of glyphs: %d\n", (int)wcslen(text)*line_count );
        }
        if( count > 9 )
        {
            exit( EXIT_SUCCESS );
        }
    }
    if( count < 5 )
    {
        size_t i;
        vec4 color = {{0,0,0,1}};
        vec2 pen = {{0,0}};
        vertex_buffer_clear( buffer );

        pen.y = -font->descender;
        for( i=0; i<line_count; ++i )
        {
            pen.x = 10.0;
            add_text( buffer, font, text, &color, &pen );
            pen.y += font->height - font->linegap;
        }
    }
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glColor4f( 0,0,0,1 );
    glBindTexture( GL_TEXTURE_2D, atlas->id );
    vertex_buffer_render( buffer, GL_TRIANGLES, "vtc" );
    glutSwapBuffers( );
}


// ---------------------------------------------------------------- reshape ---
void reshape( int width, int height )
{
    glViewport(0, 0, width, height);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
}


// --------------------------------------------------------------- keyboard ---
void keyboard( unsigned char key, int x, int y )
{
    if ( key == 27 )
    {
        exit( EXIT_SUCCESS );
    }
}


// ------------------------------------------------------------------- idle ---
void idle( void )
{
    glutPostRedisplay();
}


// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    size_t i;
    vec4 color = {{0,0,0,1}};
    vec2 pen = {{0,0}};

    glutInit( &argc, argv );
    glutInitWindowSize( 800, 600 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL benchmark" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutIdleFunc( idle );

    atlas  = texture_atlas_new( 512, 512, 1 );
    font = texture_font_new( atlas, "fonts/VeraMono.ttf", 12 );
    buffer = vertex_buffer_new( "v3f:t2f:c4f" ); 

    pen.y = -font->descender;
    for( i=0; i<line_count; ++i )
    {
        pen.x = 10.0;
        add_text( buffer, font, text, &color, &pen );
        pen.y += font->height - font->linegap;
    }

    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glDisable( GL_DEPTH_TEST ); 
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    glutMainLoop( );

    return EXIT_SUCCESS;
}
