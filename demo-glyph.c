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
#include <wchar.h>
#include "vector.h"
#include "text-markup.h"
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
               TextMarkup *   markup,
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

    TextMarkup normal = { "Liberation Sans", 18, 0, 0, 0.0, 0.0,
                          {0,0,0,1}, {1,1,1,0},
                          0, {0,0,0,1}, 0, {0,0,0,1},
                          0, {0,0,0,1}, 0, {0,0,0,1} };
    TextMarkup blue = normal; blue.foreground_color.b = 1;
    TextMarkup big    = { "Bitstream Vera Serif", 400, 0, 0, 0.0, 0.0,
                          {0,0,0,1}, {1,1,1,0},
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


    // Baseline
    p1.x = 0.1*width; p1.y = origin.y;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = 0.9*width; p2.y = p1.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );

    // Top line
    p1.x = 0.1*width; p1.y = origin.y + glyph->offset_y;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = 0.9*width; p2.y = p1.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );

    // Bottom line
    p1.x = 0.1*width; p1.y = origin.y + glyph->offset_y - glyph->height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = 0.9*width; p2.y = p1.y;
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
    p2.x = p1.x;
    p2.y = .9*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    vertex_buffer_push_back_vertex( line_buffer, &p2 );

    // Right line
    p1.x = width/2  - glyph->offset_x - glyph->width/2 + glyph->advance_x;
    p1.y = .1*height;
    p2.x = p1.x;
    p2.y = .7*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    
    // Change line color
    p1.b = p2.b = 1;

    // width
    p1.x = width/2 - glyph->width/2;
    p1.y = .8*height;
    p2.x = width/2 + glyph->width/2;
    p2.y = .8*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = width/2 - 20;
    pen.y = .8*height + 3;
    add_text( L"width", text_buffer, &blue, &pen );

    // advance_x
    p1.x = width/2 - glyph->width/2 - glyph->offset_x;
    p1.y = .2*height;
    p2.x = width/2 - glyph->width/2 - glyph->offset_x + glyph->advance_x;
    p2.y = .2*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = width/2 - 48;
    pen.y = .2*height - 18;
    add_text( L"advance_x", text_buffer, &blue, &pen );

    // offset_x
    p1.x = width/2 - glyph->width/2 -glyph->offset_x;
    p1.y = .85*height;
    p2.x = width/2 - glyph->width/2;
    p2.y = .85*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = width/2 - glyph->width/2 + 5;
    pen.y = .85*height-8;
    add_text( L"offset_x", text_buffer, &blue, &pen );

    // height
    p1.x = 0.3*width/2;
    p1.y = origin.y + glyph->offset_y - glyph->height;
    p2.x = 0.3*width/2;
    p2.y = origin.y + glyph->offset_y;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = 0.2*width/2-30;
    pen.y = origin.y + glyph->offset_y - glyph->height/2;
    add_text( L"height", text_buffer, &blue, &pen );


    // height
    p1.x = 0.8*width;
    p1.y = origin.y + glyph->offset_y;
    p2.x = 0.8*width;
    p2.y = origin.y;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = 0.8*width+3;
    pen.y = origin.y + glyph->offset_y/2 -6;
    add_text( L"offset_y", text_buffer, &blue, &pen );


    glutMainLoop( );
    return 0;
}
