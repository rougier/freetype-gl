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
 * =========================================================================

   This source is based on the article by Jukka Jylänki :
   "A Thousand Ways to Pack the Bin - A Practical Approach to
    Two-Dimensional Rectangle Bin Packing", February 27, 2010.

   More precisely, this is an implementation of the Skyline Bottom-Left
   algorithm based on C++ sources provided by Jukka Jylänki at:
   http://clb.demon.fi/files/RectangleBinPack/

   ========================================================================= */
#pragma once
#ifndef __TEXTURE_ATLAS_H__
#define __TEXTURE_ATLAS_H__
#include "vector.h"

/**
 * A Region describes
 *
 *
 */
typedef struct
{
    int x;
    int y;
    int width;
    int height;
} Region;


typedef struct { float x,y,z,w; }  vec4;
typedef struct { float x,y,z;   }  vec3;
typedef struct { float x,y;     }  vec2;
typedef struct { int x,y,z,w;   } ivec4;
typedef struct { int x,y,z;     } ivec3;
typedef struct { int x,y;       } ivec2;


typedef struct
{
    /** Current allocated nodes */
    Vector *nodes;

    /** Width (in pixels) of the underlying texture */
    size_t width;

    /** Height (in pixels) of the underlying texture */
    size_t height;

    /** Texture format (1, 3 or 4) */
    size_t depth;

    /** Allocated surface  */
    size_t used;

    /** Texture identity (OpenGL) */
    unsigned int texid;

    unsigned char *data;

    /** A special region */
    Region black;

} TextureAtlas;


/**
 *
 */
  TextureAtlas *
  texture_atlas_new( size_t width,
                     size_t height,
                     size_t depth );


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

/**
 *
 */
  void
  texture_atlas_clear( TextureAtlas *self );

#endif /* __TEXTURE_ATLAS_H__ */
