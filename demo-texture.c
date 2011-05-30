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
#include <stdio.h>
#include <math.h>
#include <wchar.h>
#include <getopt.h>
#include "font.h"
#include "vec234.h"
#include "vector.h"
#include "vertex-buffer.h"
#include "texture-font.h"
#include "texture-glyph.h"
#include "texture-atlas.h"



void display( void )
{
    int viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    GLuint width  = viewport[2];
    GLuint height = viewport[3];

    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_TEXTURE_2D );
    glColor4f(0,0,0,1);
    glBegin(GL_QUADS);
    glTexCoord2f( 0, 1 ); glVertex2i( 0, 0 );
    glTexCoord2f( 0, 0 ); glVertex2i( 0, height );
    glTexCoord2f( 1, 0 ); glVertex2i( width, height );
    glTexCoord2f( 1, 1 ); glVertex2i( width, 0 );
    glEnd();
    glutSwapBuffers( );
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}

void keyboard( unsigned char key, int x, int y )
{
    if ( key == 27 )
    {
        exit( 1 );
    }
}

int main( int argc, char **argv )
{
    int bold   = 0;
    int italic = 0;
    char * font_description = "Bitstream Vera Sans";
    size_t font_minsize = 10;
    size_t font_maxsize = 28;
    size_t font_count = font_maxsize - font_minsize;
    float gamma = 1.5;
    wchar_t *font_cache = L" !\"#$%&'()*+,-./0123456789:;<=>?"
                          L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                          L"`abcdefghijklmnopqrstuvwxyz{|}~";
    char * font_filename = "./Vera.ttf";
    TextureAtlas *atlas;
    TextureFont *font;
    size_t i, missed = 0;

    glutInit( &argc, argv );
    glutInitWindowSize( 512, 512 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    atlas = texture_atlas_new( 512, 512 );
    for( i=0; i < font_count; ++i)
    {
        font_filename = font_find( font_description, bold, italic );
        font = texture_font_new( atlas, font_filename, font_minsize+i, gamma );
        missed += texture_font_cache_glyphs( font, font_cache );
        texture_font_delete(font);
    }
    glBindTexture( GL_TEXTURE_2D, atlas->texid );
    printf( "Matched font               : %s\n", font_filename );
    printf( "Number of fonts            : %ld\n", font_count );
    printf( "Number of glyphs per font  : %ld\n", wcslen(font_cache) );
    printf( "Number of missed glyphs    : %ld\n", missed );
    printf( "Total number of glyphs     : %ld/%ld\n",
            wcslen(font_cache)*font_count - missed, wcslen(font_cache)*font_count );
    printf( "Texture size               : %ldx%ld\n", atlas->width, atlas->height );
    printf( "Texture occupancy          : %.2f%%\n", 
            100.0*atlas->used/(float)(atlas->width*atlas->height) );
    glutMainLoop( );
    return 0;
}
