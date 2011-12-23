/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * -------------------------------------------------------------------------
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
 * ========================================================================= */
#include "freetype-gl.h"
#include "vertex-buffer.h"
#include "markup.h"


// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
} vertex_t;

typedef struct {
    float x, y;        // position
    float r, g, b, a; // color
} point_t;


// ------------------------------------------------------- global variables ---
vertex_buffer_t * text_buffer;
vertex_buffer_t * line_buffer;
vertex_buffer_t * point_buffer;


// ---------------------------------------------------------------- display ---
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


// ---------------------------------------------------------------- reshape ---
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
void add_text( vertex_buffer_t * buffer, texture_font_t * font,
               wchar_t *  text, vec4 * color, vec2 * pen )
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


// ------------------------------------------------------------------- main ---
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

    vec4 blue  = {{{0,0,1,1}}};
    vec4 black = {{{0,0,0,1}}};

    texture_atlas_t * atlas = texture_atlas_new( 512, 512, 1);
    texture_font_t * big = texture_font_new( atlas, "./Vera.ttf", 400);
    texture_font_t * small = texture_font_new( atlas, "./Vera.ttf", 18);
    texture_font_t * title = texture_font_new( atlas, "./Vera.ttf", 32);

    text_buffer  = vertex_buffer_new( "v3f:t2f:c4f" ); 
    line_buffer  = vertex_buffer_new( "v2f:c4f" ); 
    point_buffer = vertex_buffer_new( "v2f:c4f" ); 

    vec2  pen, origin;
    point_t p1, p2;

    p1.r = p1.g = p1.b = 0; p1.a = 1;
    p2.r = p2.g = p2.b = 0; p2.a = 1;

    texture_glyph_t *glyph  = texture_font_get_glyph( big, L'g' );
    origin.x = width/2  - glyph->offset_x - glyph->width/2;
    origin.y = height/2 - glyph->offset_y + glyph->height/2;
    add_text( text_buffer, big, L"g", &black, &origin );

    // title
    pen.x = 50;
    pen.y = 560;
    add_text( text_buffer, title, L"Glyph metrics", &black, &pen );


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
    pen.x = p1.x - 58;
    pen.y = p1.y - 20;
    add_text( text_buffer, small, L"Origin", &black, &pen );


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
    add_text( text_buffer, small, L"width", &blue, &pen );

    // advance_x
    p1.x = width/2 - glyph->width/2 - glyph->offset_x;
    p1.y = .2*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = width/2 - glyph->width/2 - glyph->offset_x + glyph->advance_x;
    p2.y = p1.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = width/2 - 48;
    pen.y = .2*height - 18;
    add_text( text_buffer, small, L"advance_x", &blue, &pen );

    // Offset_x
    p1.x = width/2 - glyph->width/2 -glyph->offset_x;
    p1.y = .85*height;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = width/2 - glyph->width/2;
    p2.y = p1.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = width/2 - glyph->width/2 + 5;
    pen.y = .85*height-8;
    add_text( text_buffer, small, L"offset_x", &blue, &pen );

    // Height
    p1.x = 0.3*width/2;
    p1.y = origin.y + glyph->offset_y - glyph->height;
    p2.x = 0.3*width/2;
    p2.y = origin.y + glyph->offset_y;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = 0.2*width/2-30;
    pen.y = origin.y + glyph->offset_y - glyph->height/2;
    add_text( text_buffer, small, L"height", &blue, &pen );


    // Offset y
    p1.x = 0.8*width;
    p1.y = origin.y + glyph->offset_y;
    vertex_buffer_push_back_vertex( line_buffer, &p1 );
    p2.x = p1.x;
    p2.y = origin.y;
    vertex_buffer_push_back_vertex( line_buffer, &p2 );
    pen.x = 0.8*width+3;
    pen.y = origin.y + glyph->offset_y/2 -6;
    add_text( text_buffer, small, L"offset_y", &blue, &pen );

    glutMainLoop( );
    return 0;
}
