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
#include <wchar.h>
#include "vector.h"
#include "markup.h"
#include "font-manager.h"
#include "texture-font.h"
#include "texture-glyph.h"
#include "vertex-buffer.h"


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
    glTranslatef(5, viewport[3]-32, 0);
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
        exit( EXIT_SUCCESS );
    }
}


void add_text( wchar_t *      text,
               VertexBuffer * buffer,
               Markup *       markup,
               Pen *          pen )
{
    size_t i;
    TextureFont *font    = font_manager_get_from_markup( manager, markup );
    TextureGlyph *glyph  = texture_font_get_glyph( font, text[0] );
    texture_glyph_add_to_vertex_buffer( glyph, buffer, markup, pen );
    for( i=1; i<wcslen(text); ++i )
    {
        TextureGlyph *glyph = texture_font_get_glyph( font, text[i] );
        pen->x += texture_glyph_get_kerning( glyph, text[i-1] );
        texture_glyph_add_to_vertex_buffer( glyph, buffer, markup, pen );
    }
    glBindTexture( GL_TEXTURE_2D, manager->atlas->texid );
}

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitWindowSize( 700, 400 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

/*
    TextMarkup markup = { "Liberation Mono", 15, 0, 0, 0.0, 0.0,
                          {0,0,0,1}, {1,1,1,0},
                          0, {0,0,0,1}, 0, {0,0,0,1},
                          0, {0,0,0,1}, 0, {0,0,0,1} };

    Pen pen = {0.0, 16} ;
    manager = font_manager_new();
    buffer= vertex_buffer_new( "v3i:t2f:c4f" ); 

	FILE *fp = fopen( "demo-label.c", "r" );
    char   line[1024];
    wchar_t wline[1024];
	if (fp == NULL)
    {
        exit( EXIT_FAILURE );
    }

    while( fgets( line, sizeof(line), fp ) != NULL )
    {
      int len = strlen(line)-1;
      if( line[len] == '\n' )
      {
          line[len] = '\0';
      }
      if( len > 0)
      {
          swprintf( wline, strlen(line)+1, L"%s", line );
          add_text( wline, buffer, &markup, &pen );
      }
      pen.y -= 18;
      pen.x = 0;
    }
    fclose(fp);
*/

    Markup normal = { "Liberation Sans", 18, 0, 0, 0.0, 0.0,
                          {0,0,0,1}, {1,1,1,0},
                          0, {0,0,0,1}, 0, {0,0,0,1},
                          0, {0,0,0,1}, 0, {0,0,0,1} };
    Markup title  = normal; title.size = 32;
    Markup bold   = normal; bold.bold = 1;
    Markup italic = normal; italic.italic = 1;
    Markup condensed = normal; condensed.spacing = -2.0;
    Markup expanded = normal; expanded.spacing   = +2.0;
    Markup subscript   = normal; subscript.rise   =-2; subscript.size = 12;
    Markup superscript = normal; superscript.rise = 2; superscript.size = 12;
    Color color_red  = {1,0,0,1};
    Color color_green= {0,1,0,1};
    Color color_blue = {0,0,1,1};
    Markup red   = normal; red.foreground_color   = color_red;
    Markup green = normal; green.foreground_color = color_green;
    Markup blue  = normal; blue.foreground_color  = color_blue;
    Markup zapfino = normal; zapfino.family = "Zapfino";


    Pen pen = {0.0, 0.0} ;
    manager = font_manager_new();
    buffer= vertex_buffer_new( "v3i:t2f:c4f" ); 

    add_text( L"Freetype OpenGL™", buffer, &title, &pen );
    pen.y -= 36; pen.x = 0;

    add_text( L"Freetype text handling using a single"
              L"vertex buffer and a single texture featuring:",
              buffer, &normal, &pen);
    pen.y -= 24; pen.x = 0;

    add_text( L" • Unicode characters: ²³€¥∑∫∞√©Ω¿«»", buffer, &normal, &pen );
    pen.y -= 24; pen.x = 0;

    add_text( L" • ", buffer, &normal, &pen );
    add_text( L"superscript ", buffer, &superscript, &pen );
    add_text( L" and ", buffer, &normal, &pen );
    add_text( L"subscript ", buffer, &subscript, &pen );
    pen.y -= 24; pen.x = 0;

    add_text( L" • Any ", buffer, &green, &pen );
    add_text( L"color ", buffer, &blue, &pen );
    add_text( L"you like", buffer, &red, &pen );
    pen.y -= 40; pen.x = 0;

    add_text( L" • ", buffer, &normal, &pen );
    add_text( L"Any (installed) font", buffer, &zapfino, &pen );
    pen.y -= 42; pen.x = 0;

    add_text( L" • ", buffer, &normal, &pen );
    add_text( L"condensed", buffer, &condensed, &pen );
    add_text( L" or ", buffer, &normal, &pen );
    add_text( L"expanded", buffer, &expanded, &pen );
    pen.y -= 24; pen.x = 0;

    glutMainLoop( );
    return 0;
}
