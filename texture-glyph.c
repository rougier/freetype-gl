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
#include <assert.h>
#include <stdlib.h>
#include "texture-font.h"
#include "texture-glyph.h"


/* ------------------------------------------------------------------------- */
TextureGlyph *
texture_glyph_new( void )
{
    TextureGlyph *self = (TextureGlyph *) malloc( sizeof(TextureGlyph) );
    if(! self )
    {
        return NULL;
    }
    self->width     = 0;
    self->height    = 0;
    self->offset_x  = 0;
    self->offset_y  = 0;
    self->advance_x = 0.0;
    self->advance_y = 0.0;
    self->u0        = 0.0;
    self->v0        = 0.0;
    self->u1        = 0.0;
    self->v1        = 0.0;
    return self;
}



/* ------------------------------------------------------------------------- */
void
texture_glyph_delete( TextureGlyph *self )
{
    assert( self );
    free( self );
}



/* ------------------------------------------------------------------------- */
void
texture_glyph_render( TextureGlyph *self,
                      Markup *markup,
                      Pen *pen )
{
    assert( self );

    int x  = pen->x + self->offset_x;
    int y  = pen->y + self->offset_y + markup->rise;
    int w  = self->width;
    int h  = self->height;

    float u0 = self->u0;
    float v0 = self->v0;
    float u1 = self->u1;
    float v1 = self->v1;

    glBegin( GL_TRIANGLES );
    {
        glTexCoord2f( u0, v0 ); glVertex2i( x,   y   );
        glTexCoord2f( u0, v1 ); glVertex2i( x,   y-h );
        glTexCoord2f( u1, v1 ); glVertex2i( x+w, y-h );
        
        glTexCoord2f( u0, v0 ); glVertex2i( x,   y   );
        glTexCoord2f( u1, v1 ); glVertex2i( x+w, y-h );
        glTexCoord2f( u1, v0 ); glVertex2i( x+w, y   );
    }
    glEnd();

    pen->x += self->advance_x + markup->spacing;
    pen->y += self->advance_y;
}



/* ------------------------------------------------------------------------- */
void
texture_glyph_add_to_vertex_buffer( const TextureGlyph *self,
                                    VertexBuffer *buffer,
                                    const Markup *markup,
                                    Pen *pen, int kerning )
{
    TextureFont *font = self->font;
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 1;
    int rise = 0;
    int spacing = 0;

    if( markup )
    {
        rise = markup->rise;
        spacing = markup->spacing;
    }
    
    pen->x += kerning;

    // Background
    if( markup && markup->background_color.a > 0 )
    {
        float u0 = font->atlas->black.x / (float) font->atlas->width;
        float v0 = font->atlas->black.y / (float) font->atlas->height;
        float u1 = u0 + font->atlas->black.width / (float) font->atlas->width;
        float v1 = v0 + font->atlas->black.height / (float) font->atlas->height;
        int x0  = pen->x - kerning;
        int y0  = pen->y + font->descender;
        int x1  = x0 + self->advance_x + markup->spacing + kerning;
        int y1  = y0 + font->height - font->linegap;
        r = markup->background_color.r;
        g = markup->background_color.g;
        b = markup->background_color.b;
        a = markup->background_color.a;
        GLuint index = buffer->vertices->size;
        GLuint indices[] = {index, index+1, index+2,
                            index, index+2, index+3};
        TextureGlyphVertex vertices[] = { { x0,y0,0,  u0,v0,  r,g,b,a },
                                          { x0,y1,0,  u0,v1,  r,g,b,a },
                                          { x1,y1,0,  u1,v1,  r,g,b,a },
                                          { x1,y0,0,  u1,v0,  r,g,b,a } };
        vertex_buffer_push_back_indices( buffer, indices, 6 );
        vertex_buffer_push_back_vertices( buffer, vertices, 4 );
    }

    // Underline

    // Overline

    // Outline

    // Strikethrough

    // Actual glyph
    if( markup )
    {
        r = markup->foreground_color.r;
        g = markup->foreground_color.g;
        b = markup->foreground_color.b;
        a = markup->foreground_color.a;
    }
    int x0  = pen->x + self->offset_x;
    int y0  = pen->y + self->offset_y + rise;
    int x1  = x0 + self->width;
    int y1  = y0 - self->height;
    float u0 = self->u0;
    float v0 = self->v0;
    float u1 = self->u1;
    float v1 = self->v1;
    GLuint index = buffer->vertices->size;
    GLuint indices[] = {index, index+1, index+2,
                        index, index+2, index+3};
    TextureGlyphVertex vertices[] = { { x0,y0,0,  u0,v0,  r,g,b,a },
                                      { x0,y1,0,  u0,v1,  r,g,b,a },
                                      { x1,y1,0,  u1,v1,  r,g,b,a },
                                      { x1,y0,0,  u1,v0,  r,g,b,a } };
    vertex_buffer_push_back_indices( buffer, indices, 6 );
    vertex_buffer_push_back_vertices( buffer, vertices, 4 );

    pen->x += self->advance_x + spacing;
    pen->y += self->advance_y;
}



/* ------------------------------------------------------------------------- */
float 
texture_glyph_get_kerning( TextureGlyph *self,
                           wchar_t charcode )
{
    size_t i;

    assert( self );
    if( !self->kerning )
    {
        return 0;
    }

    for( i=0; i<self->kerning_count; ++i )
    {
        if( self->kerning[i].charcode == charcode )
        {
            return self->kerning[i].kerning;
        }
    }
    return 0;
}
