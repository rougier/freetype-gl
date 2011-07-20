// ============================================================================
// Freetype GL - An ansi C OpenGL Freetype engine
// Platform:    Any
// API version: 1.0
// WWW:         http://www.loria.fr/~rougier/freetype-gl
// ----------------------------------------------------------------------------
// Copyright (c) 2011 Nicolas P. Rougier <Nicolas.Rougier@inria.fr>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program. If not, see <http://www.gnu.org/licenses/>.
// ============================================================================
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
typedef struct
{
    Vector *       glyphs;
    TextureAtlas * atlas;
    char *         filename;
    int            bold;
    int            italic;
    int            border;
    float          size;
    float          gamma;
} TextureFont;



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

