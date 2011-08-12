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
#pragma once
#ifndef __TEXTURE_GLYPH_H__
#define __TEXTURE_GLYPH_H__

#include <wchar.h>
#include "text-markup.h"
#include "vertex-buffer.h"

/**
 *
 */
typedef struct {
    wchar_t   charcode;
    float     kerning;       
} KerningPair;


/**
 *
 */
typedef struct {
    float x,y;
} Pen;


/**
 *
 */
typedef struct {
    wchar_t       charcode;
    int           width, height;
    int           offset_x, offset_y;
    int           advance_x, advance_y;
    float         u0, v0, u1, v1;
    KerningPair * kerning;
    size_t        kerning_count;
} TextureGlyph;


/**
 *
 */
typedef struct {
    int x, y, z;
    float u, v;
    float r, g, b, a;
} TextureGlyphVertex;


/**
 *
 */
  TextureGlyph *
  texture_glyph_new( void );


/**
 *
 */
  void
  texture_glyph_delete( TextureGlyph * self );


/**
 *
 */
  void
  texture_glyph_render( TextureGlyph * self,
                        TextMarkup * markup,
                        Pen * pen );


/**
 *
 */
  void
  texture_glyph_add_to_vertex_buffer( const TextureGlyph * self,
                                      VertexBuffer * buffer,
                                      const TextMarkup * markup,
                                      Pen * pen );


/**
 *
 */
  float
  texture_glyph_get_kerning( TextureGlyph * self,
                             wchar_t charcode );

#endif /* __TEXTURE_GLYPH_H__ */
