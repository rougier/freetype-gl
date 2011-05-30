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
#ifndef __TEXTURE_FONT_H__
#define __TEXTURE_FONT_H__

#include "vec234.h"
#include "vector.h"
#include "texture-atlas.h"
#include "texture-glyph.h"

typedef struct
{
    vector *       glyphs;
    TextureAtlas * atlas;
    char *         filename;
    size_t         size;
    float          gamma;
} TextureFont;



TextureFont *  texture_font_new( TextureAtlas *atlas,
                                 char *filename, size_t size, float gamma );
void           texture_font_delete( TextureFont *self );
TextureGlyph * texture_font_get_glyph( TextureFont *self,
                                       wchar_t charcode );
size_t         texture_font_cache_glyphs( TextureFont *self,
                                          wchar_t * charcodes );

#endif /* __TEXTURE_FONT_H__ */

