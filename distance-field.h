/* ============================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         https://github.com/rougier/freetype-gl
 * ----------------------------------------------------------------------------
 * Copyright 2011,2012,2013 Nicolas P. Rougier. All rights reserved.
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
 * ----------------------------------------------------------------------------
 */
#ifndef __DISTANCE_FIELD_H__
#define __DISTANCE_FIELD_H__

#ifdef __cplusplus
extern "C" {
namespace ftgl {
#endif

/**
 * @file   shader.h
 * @author Nicolas Rougier (Nicolas.Rougier@inria.fr)
 *
 * @defgroup distance-field Distance Field
 *
 * Functions to calculate signed distance fields for bitmaps.
 *
 * <b>Example Usage</b>:
 * @code
 * #include "distance-field.h"
 *
 * int main( int arrgc, char *argv[] )
 * {
 *     int width = 512;
 *     int height = 512;
 *     unsigned char *image = create_greyscale_image(width, height);
 *
 *     image = make_distance_map( image, width, height );
 *
 *     return 0;
 * }
 * @endcode
 *
 * @{
 */

/**
 * Create a distance file from the given image.
 *
 * @param img     A greyscale image.
 * @param width   The width of the given image.
 * @param height  The height of the given image.
 *
 * @return        A newly allocated distance field.  This image must
 *                be freed after usage.
 *
 */
double *
make_distance_mapd( double *img,
                    unsigned int width, unsigned int height );

unsigned char *
make_distance_mapb( unsigned char *img,
                    unsigned int width, unsigned int height );

/** @} */

#ifdef __cplusplus
}
}
#endif

#endif /* __DISTANCE_FIELD_H__ */
