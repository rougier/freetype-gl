/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         https://github.com/rougier/freetype-gl
 * -------------------------------------------------------------------------
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
 * ========================================================================= */
#ifndef __UTF8_UTILS_H__
#define __UTF8_UTILS_H__

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {

namespace ftgl {
#endif

/**
 * @file    utf8-utils.h
 * @author  Marcel Metz <mmetz@adrian-broher.net>
 *
 * defgroup utf8-utils UTF-8 Utilities
 *
 * @{
 */

  /**
   * Returns the size in bytes of a given UTF-8 encoded character surrogate
   *
   * @param character  An UTF-8 encoded character
   *
   * @return  The length of the surrogate in bytes.
   */
  size_t
  utf8_surrogate_len( const char* character );

  /**
   * Return the length of the given UTF-8 encoded and
   * NULL terminated string.
   *
   * @param string  An UTF-8 encoded string
   *
   * @return  The length of the string in characters.
   */
  size_t
  utf8_strlen( const char* string );

  /**
   * Converts a given UTF-8 encoded character to its UTF-32 LE equivalent
   *
   * @param character  An UTF-8 encoded character
   *
   * @return  The equivalent of the given character in UTF-32 LE
   *          encoding.
   */
  uint32_t
  utf8_to_utf32( const char * character );

/**
 * @}
 */

#ifdef __cplusplus
}
}
#endif

#endif /* #define __UTF8_UTILS_H__ */
