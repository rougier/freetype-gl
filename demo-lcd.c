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
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "vector.h"
#include "font-manager.h"
#include "texture-font.h"
#include "texture-atlas.h"
#include "vertex-buffer.h"

VertexBuffer *buffer;

void display( void )
{
    int viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    float alpha = 1;
    glEnable( GL_COLOR_MATERIAL );
    glBlendFunc( GL_CONSTANT_COLOR_EXT,
                 GL_ONE_MINUS_SRC_COLOR );
    glEnable( GL_BLEND );
    glColor3f( alpha, alpha, alpha );
    glBlendColor( 1-alpha, 1-alpha, 1-alpha, 1 );
    glEnable( GL_TEXTURE_2D );
    glPushMatrix();
    glTranslatef(5, viewport[3], 0);
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
    size_t minsize = 8;
    size_t maxsize = 27;
    size_t count = maxsize - minsize;
    wchar_t *text = L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789";
    Pen pen ;
    TextureFont *font;
    TextureGlyph *glyph;
    FontManager *manager = font_manager_new( 512, 512, 3 );
    buffer= vertex_buffer_new( "v3f:t2f:c4f" ); 
    Markup markup = { "Verdana", minsize, 0, 0, 0.0, 0.0,
                      {1,1,1,1}, {0,0,0,0},
                      0, {0,0,0,1}, 0, {0,0,0,1},
                      0, {0,0,0,1}, 0, {0,0,0,1}, 0 };

    glutInit( &argc, argv );
    glutInitWindowSize( 800, 400 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    pen.x = 0; pen.y = 0;
    font = font_manager_get_from_markup( manager, &markup );
    pen.y -= font->ascender;

    for( i=0; i < count; ++i)
    {
        markup.size = minsize+i;
        font = font_manager_get_from_markup( manager, &markup );
        glyph = texture_font_get_glyph( font, text[0] );
        if( !glyph )
        {
            continue;
        }
        texture_glyph_add_to_vertex_buffer( glyph, buffer, &markup, &pen, 0 );
        for( j=1; j<wcslen(text); ++j )
        {
            glyph = texture_font_get_glyph( font, text[j] );
            int kx = texture_glyph_get_kerning( glyph, text[j-1] );
            texture_glyph_add_to_vertex_buffer( glyph, buffer, &markup, &pen, kx );
        }
        pen.y -= font->height - font->linegap;
        pen.x = 0;
    }

    glBindTexture( GL_TEXTURE_2D, manager->atlas->texid );
    glutMainLoop( );
    return 0;
}
