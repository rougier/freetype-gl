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
#ifndef __TEXTURE_GLYPH_H__
#define __TEXTURE_GLYPH_H__

#include <wchar.h>
#include "vec234.h"
#include "vertex-buffer.h"


typedef struct {
    wchar_t   charcode;
    float     kerning;       
} KerningPair;

typedef struct {
    wchar_t       charcode;
    ivec2         size;
    ivec2         offset;
    vec2          advance;
    vec4          texcoords;
    KerningPair * kerning;
    size_t        kerning_count;
} TextureGlyph;

typedef struct {
    int x, y, z;
    float u, v;
    float r, g, b, a;
} TextureGlyphVertex;


TextureGlyph * texture_glyph_new( void );
TextureGlyph * texture_glyph_copy( TextureGlyph *other );
void           texture_glyph_delete( TextureGlyph *self );

void           texture_glyph_render( TextureGlyph *self,
                                     vec2 *pen );
void           texture_glyph_add_to_vertex_buffer( TextureGlyph *self,
                                                   VertexBuffer *buffer,
                                                   vec2 *pen );
float          texture_glyph_get_kerning( TextureGlyph *self,
                                          wchar_t charcode );

#endif /* __TEXTURE_GLYPH_H__ */
