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
#include "vector.h"
#include "text-markup.h"
#include "font-manager.h"
#include "texture-font.h"
#include "texture-glyph.h"
#include "vertex-buffer.h"


VertexBuffer *buffer;
FontManager *manager;
wchar_t *text =
    L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789 "
    L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789 "
    L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789 ";
int line_count = 83;

void init( void )
{
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glDisable( GL_DEPTH_TEST ); 
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
}

void generate_buffer( void)
{
    size_t i, j;
    Pen pen;
    TextMarkup markup = { "Monotype", 12, 0, 0, 0.0, 0.0,
                          {0,0,0,1}, {1,1,1,0},
                          0, {0,0,0,1}, 0, {0,0,0,1},
                          0, {0,0,0,1}, 0, {0,0,0,1} };
    TextureFont *font= font_manager_get_from_markup( manager, &markup );
    TextureGlyph *glyph;
    vertex_buffer_clear( buffer );
    pen.y = 5.0;
    for( j=0; j<line_count; ++j )
    {
        pen.x = 10.0;
        glyph = texture_font_get_glyph( font, text[0] );
        texture_glyph_add_to_vertex_buffer( glyph, buffer, &markup, &pen );
        for( i=1; i<wcslen(text); ++i )
        {
            glyph = texture_font_get_glyph( font, text[i] );
            if( text[i] == L' ')
            {
                pen.x += glyph->advance_x;
            }
            else
            {
                pen.x += texture_glyph_get_kerning( glyph, text[i-1] );
                texture_glyph_add_to_vertex_buffer( glyph, buffer, &markup, &pen );
            }
        }
        pen.y += 12;
    }
}


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
        generate_buffer( );
    }
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glColor4f( 0,0,0,1 );
    glBindTexture( GL_TEXTURE_2D, manager->atlas->texid );
    vertex_buffer_render( buffer, GL_TRIANGLES, "vtc" );
    glutSwapBuffers( );
}


void reshape( int width, int height )
{
    glViewport(0, 0, width, height);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
}

void keyboard( unsigned char key, int x, int y )
{
    if ( key == 27 )
    {
        exit( EXIT_SUCCESS );
    }
}

void idle( void )
{
    glutPostRedisplay();
}


int main( int argc, char **argv )
{
    manager = font_manager_new();
    buffer = vertex_buffer_new( "v3i:t2f:c4f" ); 
    glutInit( &argc, argv );
    glutInitWindowSize( 1000, 1000 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Vertex buffer cube" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutIdleFunc( idle );
    init( );
    generate_buffer( );
    glutMainLoop( );
    return EXIT_SUCCESS;
}
