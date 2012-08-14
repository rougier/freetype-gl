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
#include "shader.h"

// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, z;
    float u, v;
    float r, g, b, a;
    float shift, gamma;
} vertex_t;


// ------------------------------------------------------- global variables ---
texture_atlas_t *atlas;
vertex_buffer_t *buffer;
GLuint shader;


// ---------------------------------------------------------------- display ---
void display( void )
{
    static GLuint texture = 0;
    static GLuint pixel = 0;
    if( !texture  || !pixel )
    {
        texture = glGetUniformLocation( shader, "texture" );
        pixel = glGetUniformLocation( shader, "pixel" );
    }

    int viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_TEXTURE_2D );
    glEnable( GL_COLOR_MATERIAL );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
    glBlendColor( 1.0, 1.0, 1.0, 1.0 );
    glEnable( GL_BLEND );

    glPushMatrix();
    glTranslatef(5, viewport[3], 0);
    glUseProgram( shader );
    glUniform1i( texture, 0 );
    glUniform3f( pixel, 1.0/atlas->width, 1.0/atlas->height, atlas->depth );
    vertex_buffer_render( buffer, GL_TRIANGLES );
    glUseProgram( 0 );

    glPopMatrix();
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
        exit( 1 );
    }
}


// --------------------------------------------------------------- add_text ---
void add_text( vertex_buffer_t * buffer, texture_font_t * font,
               wchar_t * text, vec4 * color, vec2 * pen )
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
            vertex_t vertices[] = { { x0,y0,0,  s0,t0,  r,g,b,a, 0,1 },
                                    { x0,y1,0,  s0,t1,  r,g,b,a, 0,1 },
                                    { x1,y1,0,  s1,t1,  r,g,b,a, 0,1 },
                                    { x1,y0,0,  s1,t0,  r,g,b,a, 0,1 } };
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
    glutInitWindowSize( 800, 400 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL / LCD filtering" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    size_t i;
    texture_font_t *font = 0;
    atlas = texture_atlas_new( 512, 512, 3 );
    const char * filename = "fonts/Vera.ttf";
    wchar_t *text = L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789";
    buffer = vertex_buffer_new( "vertex:3f,tex_coord:2f,color:4f,ashift:1f,agamma:1f" ); 
    vec2 pen = {{0,0}};
    vec4 color = {{0,0,0,1}};

    for( i=7; i < 27; ++i)
    {
        font = texture_font_new( atlas, filename, i );
        pen.x = 0;
        pen.y -= font->height;
        texture_font_load_glyphs( font, text );
        add_text( buffer, font, text, &color, &pen );
        texture_font_delete( font );
    }
    glBindTexture( GL_TEXTURE_2D, atlas->id );
    shader = shader_load("shaders/text.vert", "shaders/text.frag");
    glutMainLoop( );

    return 0;
}
