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


VertexBuffer *text_buffer, *line_buffer, *point_buffer;
FontManager *manager;

void display( void )
{
    int viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_POINT_SMOOTH );
    glColor4f(1,1,1,1);

    glEnable( GL_TEXTURE_2D );
    vertex_buffer_render( text_buffer, GL_TRIANGLES, "vtc" );

    glDisable( GL_TEXTURE_2D );
    vertex_buffer_render( line_buffer, GL_LINES, "vc" );

    glPointSize( 10.0f );
    vertex_buffer_render( point_buffer, GL_POINTS, "vc" );

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
    int width  = 600;
    int height = 600;

    glutInit( &argc, argv );
    glutInitWindowSize( width, height );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    typedef struct { float x,y; float r,g,b,a; } Point;

    Markup normal = { "Liberation Sans", 18, 0, 0, 0.0, 0.0,
                      {0,0,0,1}, {1,1,1,0},
                      0, {0,0,0,1}, 0, {0,0,0,1},
                      0, {0,0,0,1}, 0, {0,0,0,1} };
    Markup blue = normal; blue.foreground_color.b = 1;
    Markup big    = { "Bitstream Vera Serif", 400, 0, 0, 0.0, 0.0,
                      {0,0,0,1}, {1,1,1,0},
                      0, {0,0,0,1}, 0, {0,0,0,1},
                      0, {0,0,0,1}, 0, {0,0,0,1} };
    Markup title  = { "Bitstream Vera Serif", 32, 0, 0, 0.0, 0.0,
                      {.2,.2,.2,1}, {1,1,1,0},
                      0, {0,0,0,1}, 0, {0,0,0,1},
                      0, {0,0,0,1}, 0, {0,0,0,1} };
    
    manager = font_manager_new();
    text_buffer  = vertex_buffer_new( "v3i:t2f:c4f" ); 
    line_buffer  = vertex_buffer_new( "v2f:c4f" ); 
    point_buffer = vertex_buffer_new( "v2f:c4f" ); 

    Pen   pen, origin;
    Point p1, p2;

    p1.r = p1.g = p1.b = 0; p1.a = 1;
    p2.r = p2.g = p2.b = 0; p2.a = 1;

    TextureFont *font = font_manager_get_from_markup( manager, &big );
    TextureGlyph *glyph  = texture_font_get_glyph( font, L'g' );
    origin.x = width/2  - glyph->offset_x - glyph->width/2;
    origin.y = height/2 - glyph->offset_y + glyph->height/2;
    texture_glyph_add_to_vertex_buffer( glyph, text_buffer, &big, &origin );

    // title
    pen.x = 50;
    pen.y = 560;
    add_text( L"Glyph metrics", text_buffer, &title, &pen );

    // Baseline
    p1.x = 0.1*width;
    p1.y = origin.y;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = 0.9*width;
    p2.y = p1.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );

    // Top line
    p1.x = 0.1*width;
    p1.y = origin.y + glyph->offset_y;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = 0.9*width;
    p2.y = p1.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );

    // Bottom line
    p1.x = 0.1*width;
    p1.y = origin.y + glyph->offset_y - glyph->height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = 0.9*width;
    p2.y = p1.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );

    // Origin
    p1.x = width/2  - glyph->offset_x - glyph->width/2;
    p1.y = height/2 - glyph->offset_y + glyph->height/2;
    vertex_buffer_push_back_vertex( point_buffer, &p1 );
    pen.x = p1.x - 54;
    pen.y = p1.y - 20;
    add_text( L"Origin", text_buffer, &normal, &pen );

    // Advance point
    p1.x = width/2  - glyph->offset_x - glyph->width/2 + glyph->advance_x;
    p1.y = height/2 - glyph->offset_y + glyph->height/2;
    vertex_buffer_push_back_vertex( point_buffer, &p1 );

    // Left line at origin
    p1.x = width/2  - glyph->offset_x - glyph->width/2;
    p1.y = .1*height;
    p2.x = p1.x;
    p2.y = .9*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    vertex_buffer_push_back_vertex( line_buffer, &p2 );

    // Left line
    p1.x = width/2 - glyph->width/2;
    p1.y = .3*height;
    p2.x = p1.x;
    p2.y = .9*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    vertex_buffer_push_back_vertex( line_buffer, &p2 );

    // Right line
    p1.x = width/2 + glyph->width/2;
    p1.y = .3*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = p1.x;
    p2.y = .9*height;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );

    // Right line
    p1.x = width/2  - glyph->offset_x - glyph->width/2 + glyph->advance_x;
    p1.y = .1*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = p1.x;
    p2.y = .7*height;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    
    // Change line color
    p1.b = p2.b = 1;

    // width
    p1.x = width/2 - glyph->width/2;
    p1.y = .8*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = width/2 + glyph->width/2;
    p2.y = p1.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = width/2 - 20;
    pen.y = .8*height + 3;
    add_text( L"width", text_buffer, &blue, &pen );

    // advance_x
    p1.x = width/2 - glyph->width/2 - glyph->offset_x;
    p1.y = .2*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = width/2 - glyph->width/2 - glyph->offset_x + glyph->advance_x;
    p2.y = p1.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = width/2 - 48;
    pen.y = .2*height - 18;
    add_text( L"advance_x", text_buffer, &blue, &pen );

    // Offset_x
    p1.x = width/2 - glyph->width/2 -glyph->offset_x;
    p1.y = .85*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = width/2 - glyph->width/2;
    p2.y = p1.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = width/2 - glyph->width/2 + 5;
    pen.y = .85*height-8;
    add_text( L"offset_x", text_buffer, &blue, &pen );

    // Height
    p1.x = 0.3*width/2;
    p1.y = origin.y + glyph->offset_y - glyph->height;
    p2.x = 0.3*width/2;
    p2.y = origin.y + glyph->offset_y;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = 0.2*width/2-30;
    pen.y = origin.y + glyph->offset_y - glyph->height/2;
    add_text( L"height", text_buffer, &blue, &pen );


    // Offset y
    p1.x = 0.8*width;
    p1.y = origin.y + glyph->offset_y;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = p1.x;
    p2.y = origin.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = 0.8*width+3;
    pen.y = origin.y + glyph->offset_y/2 -6;
    add_text( L"offset_y", text_buffer, &blue, &pen );


    glutMainLoop( );
    return 0;
}
