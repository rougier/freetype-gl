// ============================================================================
// Freetype GL - A C OpenGL Freetype engine
// Platform:    Any
// API version: 1.0
// WWW:         http://code.google.com/p/freetype-gl/
// ----------------------------------------------------------------------------
// Copyright (c) 2011 Nicolas P. Rougier <Nicolas.Rougier@inria.fr>
// 
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program. If not, see <http://www.gnu.org/licenses/>.
// ============================================================================
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "vec234.h"
#include "vector.h"
#include "vertex-buffer.h"
#include "font-manager.h"
#include "text-markup.h"
#include "texture-font.h"
#include "texture-glyph.h"
#include "texture-atlas.h"


VertexBuffer *buffer;

void display( void )
{
    int viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_TEXTURE_2D );
    glColor4f(1,1,1,1);
    glPushMatrix();
    glTranslatef(5, viewport[3]-12, 0);
    vertex_buffer_render( buffer, GL_TRIANGLES, "vtc" );
    glPopMatrix();
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
    size_t i, j = 0;
    int bold   = 0;
    int italic = 0;
    char * family = "Bitstream Vera Sans";
    size_t minsize = 8;
    size_t maxsize = 28;
    size_t count = maxsize - minsize;
    wchar_t *text = L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789";
    vec2 pen ;
    TextureFont *font;
    TextureGlyph *glyph;
    TextMarkup markup = { "Bitstream Vera Sans", 16, 0, 0, 0.0, 0.0,
                          {0,0,0,1}, {1,1,1,1},
                          0, {0,0,0,1}, 0, {0,0,0,1},
                          0, {0,0,0,1}, 0, {0,0,0,1} };

    FontManager *manager = font_manager_new( );
    buffer= vertex_buffer_new( "v3i:t2f:c4f" ); 

    glutInit( &argc, argv );
    glutInitWindowSize( 800, 400 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    pen.x = 0; pen.y = 0;
    for( i=0; i < count; ++i)
    {
        font = font_manager_get_from_description( manager, family, minsize+i, bold, italic);
        glyph = texture_font_get_glyph( font, text[0] );
        if( !glyph )
        {
            continue;
        }
        texture_glyph_add_to_vertex_buffer( glyph, buffer, &markup, &pen );
        for( j=1; j<wcslen(text); ++j )
        {
            glyph = texture_font_get_glyph( font, text[j] );
            pen.x += texture_glyph_get_kerning( glyph, text[j-1] );
            texture_glyph_add_to_vertex_buffer( glyph, buffer, &markup, &pen );
        }
        pen.x = 0; pen.y -= (minsize+i+2);
    }

    glBindTexture( GL_TEXTURE_2D, manager->atlas->texid );
    glutMainLoop( );
    return 0;
}
