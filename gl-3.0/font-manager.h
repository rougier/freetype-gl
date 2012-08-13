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
#ifndef __FONT_MANAGER_H__
#define __FONT_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "vector.h"
#include "markup.h"
#include "texture-font.h"
#include "texture-atlas.h"

/**
 * @file   font-manager.h
 * @author Nicolas Rougier (Nicolas.Rougier@inria.fr)
 *
 * @defgroup font-manager Font manager
 *
 * Structure in charge of caching fonts.
 *
 * <b>Example Usage</b>:
 * @code
 * #include "font-manager.h"
 *
 * int main( int arrgc, char *argv[] )
 * {
 *     font_manager_t * manager = manager_new( 512, 512, 1 );
 *     texture_font_t * font = font_manager_get( manager, "Mono", 12, 0, 0 );
 *
 *     return 0;
 * }
 * @endcode
 *
 * @{
 */


/**
 * Structure in charge of caching fonts.
 */
typedef struct {
    /**
     * Texture atlas to hold font glyphs.
     */
    texture_atlas_t * atlas;

    /**
     * Cached textures.
     */
    vector_t * fonts;

    /**
     * Default glyphs to be loaded when loading a new font.
     */
    wchar_t * cache;

} font_manager_t;



/**
 * Creates a new empty font manager.
 *
 * @param   width   width of the underlying atlas
 * @param   height  height of the underlying atlas
 * @param   depth   bit depth of the underlying atlas
 *
 * @return          a new font manager.
 *
 */
  font_manager_t *
  font_manager_new( size_t width,
                    size_t height,
                    size_t depth );


/**
 *  Deletes a font manager.
 *
 *  @param self a font manager.
 */
  void
  font_manager_delete( font_manager_t *self );


/**
 *  Deletes a font from the font manager.
 *
 *  Note that font glyphs are not removed from the atlas.
 *
 *  @param self a font manager.
 *  @param font font to be deleted
 *
 */
  void
  font_manager_delete_font( font_manager_t * self,
                            texture_font_t * font );


/**
 *  Request for a font based on a filename.
 *
 *  @param self     a font manager.
 *  @param filename font filename
 *  @param size     font size
 *
 *  @return Requested font
 */
  texture_font_t *
  font_manager_get_from_filename( font_manager_t * self,
                                  const char * filename,
                                  const float size );


/**
 *  Request for a font based on a description
 *
 *  @param self     a font manager
 *  @param family   font family
 *  @param size     font size
 *  @param bold     whether font is bold
 *  @param italic   whether font is italic
 *
 *  @return Requested font
 */
  texture_font_t *
  font_manager_get_from_description( font_manager_t * self,
                                     const char * family,
                                     const float size,
                                     const int bold,
                                     const int italic );


/**
 *  Request for a font based on a markup
 *
 *  @param self    a font manager
 *  @param markup  Markup describing a font
 *
 *  @return Requested font
 */
  texture_font_t *
  font_manager_get_from_markup( font_manager_t *self,
                                const markup_t *markup );


/**
 *  Search for a font filename that match description.
 *
 *  @param self    a font manager
 *  @param family   font family
 *  @param size     font size
 *  @param bold     whether font is bold
 *  @param italic   whether font is italic
 *
 *  @return Requested font filename
 */
  char *
  font_manager_match_description( font_manager_t * self,
                                  const char * family,
                                  const float size,
                                  const int bold,
                                  const int italic );

/** @} */

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif /* __FONT_MANAGER_H__ */

