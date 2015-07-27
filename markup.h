/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
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
#ifndef __MARKUP_H__
#define __MARKUP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "texture-font.h"
#include "vec234.h"

#ifdef __cplusplus
namespace ftgl {
#endif

/**
 * @file   markup.h
 * @author Nicolas Rougier (Nicolas.Rougier@inria.fr)
 *
 * @defgroup markup Markup
 *
 * Simple structure that describes text properties.
 *
 * <b>Example Usage</b>:
 * @code
 * #include "markup.h"
 *
 * ...
 *
 * vec4 black  = {{0.0, 0.0, 0.0, 1.0}};
 * vec4 white  = {{1.0, 1.0, 1.0, 1.0}};
 * vec4 none   = {{1.0, 1.0, 1.0, 0.0}};
 *
 * markup_t normal = {
 *     .family  = "Droid Serif",
 *     .size = 24.0,
 *     .bold = 0,
 *     .italic = 0,
 *     .rise = 0.0,
 *     .spacing = 1.0,
 *     .gamma = 1.0,
 *     .foreground_color = black, .background_color    = none,
 *     .underline        = 0,     .underline_color     = black,
 *     .overline         = 0,     .overline_color      = black,
 *     .strikethrough    = 0,     .strikethrough_color = black,
 *     .font = 0,
 * };
 *
 * ...
 *
 * @endcode
 *
 * @{
 */


/**
 * Simple structure that describes text properties.
 */
typedef struct markup_t
{
    /**
     * A font family name such as "normal", "sans", "serif" or "monospace".
     */
    char * family;

    /**
     * Font size.
     */
    float size;

    /**
     * Whether text is bold.
     */
    int bold;

    /**
     * Whether text is italic.
     */
    int italic;

    /**
     * Vertical displacement from the baseline.
     */
    float rise;

    /**
     * Spacing between letters.
     */
    float spacing;

    /**
     * Gamma correction.
     */
    float gamma;

    /**
     * Text color.
     */
    vec4 foreground_color;

    /**
     * Background color.
     */
    vec4 background_color;

    /**
     * Whether outline is active.
     */
    int outline;

    /**
     * Outline color.
     */
    vec4 outline_color;

    /**
     * Whether underline is active.
     */
    int underline;

    /**
     * Underline color.
     */
    vec4 underline_color;

    /**
     * Whether overline is active.
     */
    int overline;

    /**
     * Overline color.
     */
    vec4 overline_color;

    /**
     * Whether strikethrough is active.
     */
    int strikethrough;

    /**
     * Strikethrough color.
     */
    vec4 strikethrough_color;

    /**
     * Pointer on the corresponding font (family/size/bold/italic)
     */
    texture_font_t * font;

} markup_t;


extern markup_t default_markup;


/** @} */

#ifdef __cplusplus
}
}
#endif

#endif /* __MARKUP_H__ */
