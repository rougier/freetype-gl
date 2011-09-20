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
#ifndef __TEXTURE_FONT_H__
#define __TEXTURE_FONT_H__

#include <ft2build.h>
#include FT_FREETYPE_H
#include "vector.h"
#include "texture-atlas.h"
#include "texture-glyph.h"

/**
 *
 */
struct TextureFont_
{
    Vector *       glyphs;
    TextureAtlas * atlas;
    char *         filename;
    int            bold;
    int            italic;
    float          size;
    float          gamma;
    Region         black;
    int            antialias;
    int            subpixel;
    int            hinting;
    float          height;
    float          linegap;
    float          ascender;
    float          descender;

    int            lcd_filter;
    unsigned char  lcd_weights[5];
};
typedef struct TextureFont_ TextureFont;



/**
 *
 */
  TextureFont *
  texture_font_new( TextureAtlas * atlas,
                    const char * filename,
                    const float size );


/**
 *
 */
  void
  texture_font_delete( TextureFont * self );


/**
 *
 */
  TextureGlyph *
  texture_font_get_glyph( TextureFont * self,
                          wchar_t charcode );


/**
 *
 */
  size_t
  texture_font_cache_glyphs( TextureFont * self,
                             wchar_t * charcodes );


/**
 *
 */
  int
  texture_font_load_face( FT_Library * library,
                          const char * filename,
                          const float size,
                          FT_Face * face );


#endif /* __TEXTURE_FONT_H__ */

