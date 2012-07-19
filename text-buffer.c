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
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "opengl.h"
#include "text-buffer.h"


#define SET_GLYPH_VERTEX(value,x0,y0,z0,s0,t0,r,g,b,a,sh,gm) { \
	glyph_vertex_t *gv=&value;                                 \
	gv->x=x0; gv->y=y0; gv->z=z0;                              \
	gv->u=s0; gv->v=t0;                                        \
	gv->r=r; gv->g=g; gv->b=b; gv->a=a;                        \
	gv->shift=sh; gv->gamma=gm;}


// ----------------------------------------------------------------------------
text_buffer_t *
text_buffer_new( size_t depth )
{
    
    text_buffer_t *self = (text_buffer_t *) malloc (sizeof(text_buffer_t));
    self->buffer = vertex_buffer_new( "v3f:t2f:c4f:1g1f:2g1f" ); 
    self->manager = font_manager_new( 512, 512, depth );
        self->shader = shader_load("shaders/text.vert",
                                   "shaders/text.frag");
    self->shader_texture = glGetUniformLocation(self->shader, "texture");
    self->shader_pixel = glGetUniformLocation(self->shader, "pixel");
    self->line_start = 0;
    self->line_ascender = 0;
    self->line_descender = 0;
    return self;
}

// ----------------------------------------------------------------------------
void
text_buffer_render( text_buffer_t * self )
{
    glEnable( GL_BLEND );
    glEnable( GL_TEXTURE_2D );
    glColor4f( 1.0, 1.0, 1.0, 1.0);

    if( self->manager->atlas->depth == 1 )
    {
        glDisable( GL_COLOR_MATERIAL );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        vertex_buffer_render( self->buffer, GL_TRIANGLES, "vtc" );
    }
    else
    {
        glEnable( GL_COLOR_MATERIAL );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
        glBlendColor( 1.0, 1.0, 1.0, 1.0 );
        glUseProgram( self->shader );
        glUniform1i( self->shader_texture, 0 );
        glUniform3f( self->shader_pixel,
                     1.0/self->manager->atlas->width,
                     1.0/self->manager->atlas->height,
                 self->manager->atlas->depth );
        vertex_buffer_render( self->buffer, GL_TRIANGLES, "vtc" );
        glUseProgram( 0 );
    }
}

// ----------------------------------------------------------------------------
void
text_buffer_printf( text_buffer_t * self, vec2 *pen, ... )
{
    markup_t *markup;
    wchar_t *text;

    if( vertex_buffer_size( self->buffer ) == 0 )
    {
        self->origin = *pen;
    }

    va_list args;
    va_start ( args, pen ); 
    do {
        markup = va_arg( args, markup_t * );
        if( markup == NULL )
        {
            return;
        }
        text = va_arg( args, wchar_t * );
        text_buffer_add_text( self, pen, markup, text, wcslen(text) );
    } while( markup != 0 );
    va_end ( args );
}

// ----------------------------------------------------------------------------
void
text_buffer_add_text( text_buffer_t * self,
                      vec2 * pen, markup_t * markup,
                      wchar_t * text, size_t length )
{
    vertex_buffer_t * buffer = self->buffer;
    font_manager_t * manager = self->manager;

    if( markup == NULL )
    {
        return;
    }

    if( !markup->font )
    {
        markup->font = font_manager_get_from_markup( manager, markup );
        if( ! markup->font )
        {
            fprintf( stderr, "Houston, we've got a problem !\n" );
            exit( EXIT_FAILURE );
        }
    }

    if( length == 0 )
    {
        length = wcslen(text);
    }

    if( vertex_buffer_size( self->buffer ) == 0 )
    {
        self->origin = *pen;
    }

    if( markup->font->ascender > self->line_ascender )
    {
        size_t i, j;
        float dy = (int)(markup->font->ascender - self->line_ascender);
        for( i=self->line_start; i < vector_size( buffer->items ); ++i )
        {
            ivec4 *item = (ivec4 *) vector_get( buffer->items, i);
            for( j=item->vstart; j<item->vstart+item->vcount; ++j)
            {
                glyph_vertex_t * vertex =
                    (glyph_vertex_t *)  vector_get( buffer->vertices, j );
                vertex->y -= dy;
            }
        }
        self->line_ascender = markup->font->ascender;
        pen->y -= dy;
    }
    if( markup->font->descender < self->line_descender )
    {
        self->line_descender = markup->font->descender;
    }

    text_buffer_add_wchar( self, pen, markup, text[0], 0 );

    size_t i;
    for( i=1; i<length; ++i )
    {
        text_buffer_add_wchar( self, pen, markup, text[i], text[i-1] );
    }
}

// ----------------------------------------------------------------------------
void
text_buffer_add_wchar( text_buffer_t * self,
                       vec2 * pen, markup_t * markup,
                       wchar_t current, wchar_t previous )
{
    size_t vcount = 0;
    size_t icount = 0;
    vertex_buffer_t * buffer = self->buffer;
    size_t i = 0;
    texture_font_t * font = markup->font;
    float gamma = markup->gamma;

    // Maximum number of vertices is 20 (= 5x2 triangles) per glyph:
    //  - 2 triangles for background
    //  - 2 triangles for overline
    //  - 2 triangles for underline
    //  - 2 triangles for strikethrough
    //  - 2 triangles for glyph
    glyph_vertex_t vertices[4*5];
    GLuint indices[6*5];
    
    if( current == L'\n' )
    {
        pen->x = self->origin.x;
        pen->y += (int)(self->line_descender);
        self->line_descender = 0;
        self->line_ascender = 0;
        self->line_start = vector_size( self->buffer->items );
        return;
    }
        
    texture_glyph_t *glyph = texture_font_get_glyph( font, current );
    texture_glyph_t *black = texture_font_get_glyph( font, -1 );
        
    if( glyph == NULL )
    {
        return;
    }
    
    float kerning = 0;
    if( i > 0)
    {
        kerning = texture_glyph_get_kerning( glyph, previous );
    }
    pen->x += kerning;
        
    // Background
    if( markup->background_color.alpha > 0 )
    {
        float r = markup->background_color.r;
        float g = markup->background_color.g;
        float b = markup->background_color.b;
        float a = markup->background_color.a;
        float x0 = ( pen->x -kerning );
        float y0 = (int)( pen->y + font->descender );
        float x1 = ( x0 + glyph->advance_x );
        float y1 = (int)( y0 + font->height + font->linegap );
        float s0 = black->s0;
        float t0 = black->t0;
        float s1 = black->s1;
        float t1 = black->t1;

        SET_GLYPH_VERTEX(vertices[vcount+0],
                         (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+1],
                         (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+2],
                         (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+3],
                         (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma );
/*
        vertices[vcount+0] =
            (glyph_vertex_t) { (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma };
        vertices[vcount+1] =
            (glyph_vertex_t) { (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma };
        vertices[vcount+2] =
            (glyph_vertex_t) { (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma };
        vertices[vcount+3] =
            (glyph_vertex_t) { (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma };
*/
        indices[icount + 0] = vcount+0;
        indices[icount + 1] = vcount+1;
        indices[icount + 2] = vcount+2;
        indices[icount + 3] = vcount+0;
        indices[icount + 4] = vcount+2;
        indices[icount + 5] = vcount+3;
        vcount += 4;
        icount += 6;
    }
        
    // Underline
    if( markup->underline )
    {
        float r = markup->underline_color.r;
        float g = markup->underline_color.g;
        float b = markup->underline_color.b;
        float a = markup->underline_color.a;
        float x0 = ( pen->x - kerning );
        float y0 = (int)( pen->y + font->underline_position );
        float x1 = ( x0 + glyph->advance_x );
        float y1 = (int)( y0 + font->underline_thickness ); 
        float s0 = black->s0;
        float t0 = black->t0;
        float s1 = black->s1;
        float t1 = black->t1;

        SET_GLYPH_VERTEX(vertices[vcount+0],
                         (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+1],
                         (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+2],
                         (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+3],
                         (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma );
/*
        vertices[vcount+0] =
            (glyph_vertex_t) { (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma };
        vertices[vcount+1] =
            (glyph_vertex_t) { (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma };
        vertices[vcount+2] =
            (glyph_vertex_t) { (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma };
        vertices[vcount+3] =
            (glyph_vertex_t) { (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma };
*/
        indices[icount + 0] = vcount+0;
        indices[icount + 1] = vcount+1;
        indices[icount + 2] = vcount+2;
        indices[icount + 3] = vcount+0;
        indices[icount + 4] = vcount+2;
        indices[icount + 5] = vcount+3;
        vcount += 4;
        icount += 6;
    }
    
    // Overline
    if( markup->overline )
    {
        float r = markup->overline_color.r;
        float g = markup->overline_color.g;
        float b = markup->overline_color.b;
        float a = markup->overline_color.a;
        float x0 = ( pen->x -kerning );
        float y0 = (int)( pen->y + (int)font->ascender );
        float x1 = ( x0 + glyph->advance_x );
        float y1 = (int)( y0 + (int)font->underline_thickness ); 
        float s0 = black->s0;
        float t0 = black->t0;
        float s1 = black->s1;
        float t1 = black->t1;
        SET_GLYPH_VERTEX(vertices[vcount+0],
                         (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+1],
                         (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+2],
                         (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+3],
                         (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma );
/*
        vertices[vcount+0] =
            (glyph_vertex_t) { (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma };
        vertices[vcount+1] =
            (glyph_vertex_t) { (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma };
        vertices[vcount+2] =
            (glyph_vertex_t) { (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma };
        vertices[vcount+3] =
            (glyph_vertex_t) { (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma };
*/
        indices[icount + 0] = vcount+0;
        indices[icount + 1] = vcount+1;
        indices[icount + 2] = vcount+2;
        indices[icount + 3] = vcount+0;
        indices[icount + 4] = vcount+2;
        indices[icount + 5] = vcount+3;
        vcount += 4;
        icount += 6;
    }
        
    /* Strikethrough */
    if( markup->strikethrough )
    {
        float r = markup->strikethrough_color.r;
        float g = markup->strikethrough_color.g;
        float b = markup->strikethrough_color.b;
        float a = markup->strikethrough_color.a;
        float x0  = ( pen->x -kerning );
        float y0  = (int)( pen->y + (int)font->ascender*.33);
        float x1  = ( x0 + glyph->advance_x );
        float y1  = (int)( y0 + (int)font->underline_thickness ); 
        float s0 = black->s0;
        float t0 = black->t0;
        float s1 = black->s1;
        float t1 = black->t1;
        SET_GLYPH_VERTEX(vertices[vcount+0],
                         (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+1],
                         (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+2],
                         (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma );
        SET_GLYPH_VERTEX(vertices[vcount+3],
                         (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma );
/*
        vertices[vcount+0] =
            (glyph_vertex_t) { (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma };
        vertices[vcount+1] =
            (glyph_vertex_t) { (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma };
        vertices[vcount+2] =
            (glyph_vertex_t) { (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma };
        vertices[vcount+3] =
            (glyph_vertex_t) { (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma };
*/
        indices[icount + 0] = vcount+0;
        indices[icount + 1] = vcount+1;
        indices[icount + 2] = vcount+2;
        indices[icount + 3] = vcount+0;
        indices[icount + 4] = vcount+2;
        indices[icount + 5] = vcount+3;
        vcount += 4;
        icount += 6;
    }

    // Actual glyph
    float r = markup->foreground_color.red;
    float g = markup->foreground_color.green;
    float b = markup->foreground_color.blue;
    float a = markup->foreground_color.alpha;
    float x0 = ( pen->x + glyph->offset_x );
    float y0 = (int)( pen->y + glyph->offset_y );
    float x1 = ( x0 + glyph->width );
    float y1 = (int)( y0 - glyph->height );
    float s0 = glyph->s0;
    float t0 = glyph->t0;
    float s1 = glyph->s1;
    float t1 = glyph->t1;

    SET_GLYPH_VERTEX(vertices[vcount+0],
                     (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma );
    SET_GLYPH_VERTEX(vertices[vcount+1],
                     (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma );
    SET_GLYPH_VERTEX(vertices[vcount+2],
                     (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma );
    SET_GLYPH_VERTEX(vertices[vcount+3],
                     (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma );
/*
    vertices[vcount+0] =
        (glyph_vertex_t) { (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma };
    vertices[vcount+1] =
        (glyph_vertex_t) { (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma };
    vertices[vcount+2] =
        (glyph_vertex_t) { (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma };
    vertices[vcount+3] =
        (glyph_vertex_t) { (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma };
*/
    indices[icount + 0] = vcount+0;
    indices[icount + 1] = vcount+1;
    indices[icount + 2] = vcount+2;
    indices[icount + 3] = vcount+0;
    indices[icount + 4] = vcount+2;
    indices[icount + 5] = vcount+3;
    vcount += 4;
    icount += 6;
    
    vertex_buffer_push_back( buffer, vertices, vcount, indices, icount );
    pen->x += glyph->advance_x;
}
