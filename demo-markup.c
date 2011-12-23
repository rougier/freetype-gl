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
#include "font-manager.h"
#include "vertex-buffer.h"
#include "markup.h"


// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
} vertex_t;


// ------------------------------------------------------- global variables ---
vertex_buffer_t *buffer;


// ---------------------------------------------------------------- display ---
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
    glTranslatef(5, viewport[3], 0);
    vertex_buffer_render( buffer, GL_TRIANGLES, "vtc" );
    glPopMatrix();
    glutSwapBuffers( );
}


// --------------------------------------------------------------- reshape ---
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
void add_text( vertex_buffer_t * buffer, wchar_t *text,
               markup_t * markup, vec2 * pen )
{
    size_t i;
    texture_font_t * font = markup->font;
    float r = markup->foreground_color.red;
    float g = markup->foreground_color.green;
    float b = markup->foreground_color.blue;
    float a = markup->foreground_color.alpha;

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
    glutInit( &argc, argv );
    glutInitWindowSize( 700, 400 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );


    vec4 white = {{{1,1,1,1}}};
    vec4 black = {{{0,0,0,1}}};
    vec4 red  = {{{1,0,0,1}}};
    vec4 green= {{{0,1,0,1}}};
    vec4 blue = {{{0,0,1,1}}};
    markup_t normal = { "Liberation Sans", 18, 0, 0, 0.0, 0.0,
                      {{{0,0,0,1}}}, {{{0,0,0,0}}},
                      0, {{{0,0,0,1}}}, 0, {{{0,0,0,1}}},
                      0, {{{0,0,0,1}}}, 0, {{{0,0,0,1}}}, 0 };
    markup_t title  = normal; title.size = 32;
    markup_t bold   = normal; bold.bold = 1;
    markup_t italic = normal; italic.italic = 1;
    markup_t inverse = normal;
    inverse.foreground_color = white;
    inverse.background_color = black;
    markup_t condensed = normal; condensed.spacing = -2.0;
    markup_t expanded = normal; expanded.spacing   = +2.0;
    markup_t subscript   = normal; subscript.rise   =-2; subscript.size = 12;
    markup_t superscript = normal; superscript.rise = 2; superscript.size = 12;
    markup_t m_red   = normal; m_red.foreground_color   = red;
    markup_t m_green = normal; m_green.foreground_color = green;
    markup_t m_blue  = normal; m_blue.foreground_color  = blue;
    markup_t zapfino = normal; zapfino.family = "Zapfino";


    vec2 pen = {{{0,0}}};
    font_manager_t * manager = font_manager_new( 512, 512, 1 );
    buffer= vertex_buffer_new( "v3f:t2f:c4f" ); 

    title.font       = font_manager_get_from_markup( manager, &title );
    normal.font      = font_manager_get_from_markup( manager, &normal );
    superscript.font = font_manager_get_from_markup( manager, &superscript );
    subscript.font   = font_manager_get_from_markup( manager, &subscript );
    inverse.font     = font_manager_get_from_markup( manager, &inverse );
    m_blue.font      = font_manager_get_from_markup( manager, &m_blue );
    m_green.font     = font_manager_get_from_markup( manager, &m_green );
    m_red.font       = font_manager_get_from_markup( manager, &m_red );
    zapfino.font     = font_manager_get_from_markup( manager, &zapfino );
    condensed.font   = font_manager_get_from_markup( manager, &condensed );
    expanded.font    = font_manager_get_from_markup( manager, &expanded);

    pen.y -= title.font->ascender; pen.x = 0;
    add_text( buffer, L"Freetype OpenGL™", &title, &pen );
    pen.y += title.font->descender;

    pen.y -= normal.font->ascender; pen.x = 0;
    add_text( buffer,
              L"Freetype text handling using a single"
              L"vertex buffer and a single texture featuring:",
              &normal, &pen);
    pen.y += normal.font->descender;

    pen.y -= normal.font->ascender; pen.x = 0;
    add_text( buffer, L" • Unicode characters: ²³€¥∑∫∞√©Ω¿«»",  &normal, &pen );
    pen.y += normal.font->descender;

    pen.y -= normal.font->ascender; pen.x = 0;
    add_text( buffer,  L" • ", &normal, &pen );
    add_text( buffer,  L"superscript ", &superscript, &pen );
    add_text( buffer,  L" and ", &normal, &pen );
    add_text( buffer,  L"subscript ", &subscript, &pen );
    pen.y += normal.font->descender;

    pen.y -= inverse.font->ascender; pen.x = 0;
    add_text( buffer, L" • Inverse video mode ", &inverse, &pen );
    pen.y += inverse.font->descender;

    pen.y -= normal.font->ascender; pen.x = 0;
    add_text( buffer, L" • Any ", &m_green, &pen );
    add_text( buffer, L"color ", &m_blue, &pen );
    add_text( buffer, L"you like", &m_red, &pen );
    pen.y += normal.font->descender;

    pen.y -= zapfino.font->ascender; pen.x = 0;
    add_text( buffer, L" • ", &normal, &pen );
    add_text( buffer, L"Any (installed) font", &zapfino, &pen );
    pen.y += zapfino.font->descender;

    pen.y -= normal.font->ascender; pen.x = 0;
    add_text( buffer, L" • ", &normal, &pen );
    add_text( buffer, L"condensed", &condensed, &pen );
    add_text( buffer, L" or ", &normal, &pen );
    add_text( buffer, L"expanded", &expanded, &pen );
    pen.y += normal.font->descender; pen.x = 0;

    glutMainLoop( );
    return 0;
}
