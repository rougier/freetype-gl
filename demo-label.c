/* -*- coding: utf-8 -*- */
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
#include <wchar.h>
#include <locale.h>

#include "vec234.h"
#include "vector.h"
#include "text-markup.h"
#include "vertex-buffer.h"
#include "font-manager.h"
#include "texture-font.h"
#include "texture-glyph.h"
#include "texture-atlas.h"


VertexBuffer *buffer;
FontManager *manager;

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
    glTranslatef(5, viewport[3]-50, 0);
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


void add_text( wchar_t *      text,
               VertexBuffer * buffer,
               TextMarkup *   markup,
               vec2 *         pen )
{
    size_t i;
    TextureFont *font    = font_manager_get_from_markup( manager, markup );
    TextureGlyph *glyph  = texture_font_get_glyph( font, text[0] );
    texture_glyph_add_to_vertex_buffer( glyph, buffer, pen );
    for( i=1; i<wcslen(text); ++i )
    {
        TextureGlyph *glyph = texture_font_get_glyph( font, text[i] );
        pen->x += texture_glyph_get_kerning( glyph, text[i-1] );
        texture_glyph_add_to_vertex_buffer( glyph, buffer, pen );
    }
    glBindTexture( GL_TEXTURE_2D, manager->atlas->texid );
}

int main( int argc, char **argv )
{
    //setlocale(LC_ALL, "");
    glutInit( &argc, argv );
    glutInitWindowSize( 600, 400 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    Color red = {1,0,0,1};
    TextMarkup normal = { "Bitstream Vera Sans", 16, 0, 0, 0.0, 0.0,
                          {0,0,0,1}, {1,1,1,1},
                          0, {0,0,0,1}, 0, {0,0,0,1},
                          0, {0,0,0,1}, 0, {0,0,0,1} };
    TextMarkup title  = { "Zapfino", 32, 0, 0, 0.0, 0.0,
                          {0,0,0,1}, {1,1,1,1},
                          0, {0,0,0,1}, 0, {0,0,0,1},
                          0, {0,0,0,1}, 0, {0,0,0,1} };
    TextMarkup bold        = normal; bold.bold = 1;
    TextMarkup italic      = normal; italic.italic = 1;
    TextMarkup bold_italic = bold;   bold_italic.italic = 1;
    TextMarkup bold_red    = bold; bold.foreground_color = red;

    vec2 pen = {0.0, 0.0} ;
    manager = font_manager_new();
    buffer= vertex_buffer_new( "v3i:t2f:c4f" ); 

    add_text( L"Freetype OpenGL™", buffer, &title, &pen );
    pen.y -= 56; pen.x = 0;

    add_text( L"Freetype text handling using a single vertex buffer !",
              buffer, &normal, &pen);
    pen.y -= 16; pen.x = 0;

    add_text( L"Font glyphs are stored within a ", buffer, &normal, &pen );
    add_text( L"single texture", buffer, &bold_red, &pen );
    add_text( L" !", buffer, &normal, &pen );
    pen.y -= 16; pen.x = 0;

    add_text( L"Unicode characters handling : ²³€¥∑∫∞√©Ω¿«»", buffer, &normal, &pen );
    pen.y -= 16; pen.x = 0;

    glutMainLoop( );
    return 0;
}
