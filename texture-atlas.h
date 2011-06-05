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
 * =========================================================================

   This source is based on the article by Jukka Jylänki :
   "A Thousand Ways to Pack the Bin - A Practical Approach to
    Two-Dimensional Rectangle Bin Packing", February 27, 2010.

   More precisely, this is an implementation of the Skyline Bottom-Left
   algorithm based on C++ sources provided by Jukka Jylänki at:
   http://clb.demon.fi/files/RectangleBinPack/

   ========================================================================= */
#ifndef __TEXTURE_ATLAS_H__
#define __TEXTURE_ATLAS_H__
#include "vector.h"

typedef struct
{
    int x, y, width, height;
} Region;

typedef struct
{
    Vector *nodes;
    size_t width, height, used;
    unsigned int texid;
    unsigned char *data;
} TextureAtlas;


/**
 *
 */
  TextureAtlas *
  texture_atlas_new( size_t width,
                     size_t height );


/**
 *
 */
  void
  texture_atlas_delete( TextureAtlas *self );


/**
 *
 */
  void
  texture_atlas_upload( TextureAtlas *self );


/**
 *
 */
  Region
  texture_atlas_get_region( TextureAtlas *self,
                            size_t width,
                            size_t height );


/**
 *
 */
  void
  texture_atlas_set_region( TextureAtlas *self,
                            size_t x,
                            size_t y,
                            size_t width,
                            size_t height,
                            unsigned char *data,
                            size_t stride );

#endif /* __TEXTURE_ATLAS_H__ */
