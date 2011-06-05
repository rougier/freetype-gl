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
#ifndef __FONT_MANAGER_H__
#define __FONT_MANAGER_H__

#include "vector.h"
#include "text-markup.h"
#include "texture-font.h"
#include "texture-atlas.h"


/**
 *
 */
typedef struct {
    TextureAtlas * atlas;
    Vector *       fonts;
    wchar_t *      cache;
} FontManager;



/**
 *
 */
  FontManager *
  font_manager_new( void );


/**
 *
 */
  void
  font_manager_delete( FontManager *self );


/**
 *
 */
  TextureFont *
  font_manager_get_from_filename( FontManager * self,
                                  const char * filename,
                                  const float size );


/**
 *
 */
  TextureFont *
  font_manager_get_from_description( FontManager * self,
                                     const char * family,
                                     const float size,
                                     const int bold,
                                     const int italic );


/**
 *
 */
  TextureFont *
  font_manager_get_from_markup( FontManager *self,
                                const TextMarkup *markup );


/**
 *
 */
  char *
  font_manager_match_description( FontManager * self,
                                  const char * family,
                                  const float size,
                                  const int bold,
                                  const int italic );


/**
 *
 */
  const wchar_t *
  font_manager_get_cache( FontManager * self );


/**
 *
 */
  void
  font_manager_set_cache( FontManager * self,
                          const wchar_t * cache );

#endif /* __FONT_MANAGER_H__ */

