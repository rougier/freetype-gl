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
#ifndef __TEXT_BUFFER_H__
#define __TEXT_BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "vertex-buffer.h"
#include "font-manager.h"
#include "markup.h"
#include "shader.h"

#ifdef __cplusplus
namespace ftgl {
#endif

/**
 * Use LCD filtering
 */
#define LCD_FILTERING_ON    3

/**
 * Do not use LCD filtering
 */
#define LCD_FILTERING_OFF 1

/**
 * @file   text-buffer.h
 * @author Nicolas Rougier (Nicolas.Rougier@inria.fr)
 *
 * @defgroup text-buffer Text buffer
 *
 *
 * <b>Example Usage</b>:
 * @code
 * #include "shader.h"
 *
 * int main( int arrgc, char *argv[] )
 * {
 *
 *     return 0;
 * }
 * @endcode
 *
 * @{
 */

/**
 * Text buffer structure
 */
typedef struct  text_buffer_t {
    /**
     * Vertex buffer
     */
    vertex_buffer_t *buffer;

    /**
     * Font manager
     */
    font_manager_t *manager;

    /**
     * Base color for text
     */
    vec4 base_color;


    /**
     * Pen origin
     */
    vec2 origin;

    /**
     * Index (in the vertex buffer) of the line start
     */
    size_t line_start;

    /**
     * Current line ascender
     */
    float line_ascender;

    /**
     * Current line decender
     */
    float line_descender;

    /**
     * Shader handler
     */
    GLuint shader;

    /**
     * Shader "texture" location
     */
    GLuint shader_texture;

    /**
     * Shader "pixel" location
     */
    GLuint shader_pixel;

} text_buffer_t;



/**
 * Glyph vertex structure
 */
typedef struct glyph_vertex_t {
    /**
     * Vertex x coordinates
     */
    float x;

    /**
     * Vertex y coordinates
     */
    float y;

    /**
     * Vertex z coordinates
     */
    float z;

    /**
     * Texture first coordinate
     */
    float u;

    /**
     * Texture second coordinate
     */
    float v;

    /**
     * Color red component
     */
    float r;

    /**
     * Color green component
     */
    float g;

    /**
     * Color blue component
     */
    float b;

    /**
     * Color alpha component
     */
    float a;

    /**
     * Shift along x
     */
    float shift;

    /**
     * Color gamma correction
     */
    float gamma;

} glyph_vertex_t;



/**
 * Creates a new empty text buffer.
 *
 * @param depth  Underlying atlas bit depth (1 or 3)
 *
 * @return  a new empty text buffer.
 *
 */
  text_buffer_t *
  text_buffer_new( size_t depth );



/**
 * Creates a new empty text buffer using custom shaders.
 *
 * @param depth          Underlying atlas bit depth (1 or 3)
 * @param vert_filename  Path to vertex shader
 * @param frag_filename  Path to fragment shader
 *
 * @return  a new empty text buffer.
 *
 */
  text_buffer_t *
  text_buffer_new_with_shaders( size_t depth,
                                const char * vert_filename,
                                const char * frag_filename );

/**
 * Creates a new empty text buffer using custom shaders.
 *
 * @param depth          Underlying atlas bit depth (1 or 3)
 * @param program        Shader program
 *
 * @return  a new empty text buffer.
 *
 */
  text_buffer_t *
  text_buffer_new_with_program( size_t depth,
                                GLuint program );

/**
 * Deletes texture buffer and its associated shader and vertex buffer.
 *
 * @param  self  texture buffer to delete
 *
 */
  void
  text_buffer_delete( text_buffer_t * self );

/**
 * Render a text buffer.
 *
 * @param self a text buffer
 *
 */
  void
  text_buffer_render( text_buffer_t * self );


 /**
  * Print some text to the text buffer
  *
  * @param self a text buffer
  * @param pen  position of text start
  * @param ...  a series of markup_t *, wchar_t * ended by NULL
  *
  */
  void
  text_buffer_printf( text_buffer_t * self, vec2 * pen, ... );


 /**
  * Add some text to the text buffer
  *
  * @param self   a text buffer
  * @param pen    position of text start
  * @param markup Markup to be used to add text
  * @param text   Text to be added
  * @param length Length of text to be added
  */
  void
  text_buffer_add_text( text_buffer_t * self,
                        vec2 * pen, markup_t * markup,
                        const wchar_t * text, size_t length );

 /**
  * Add a char to the text buffer
  *
  * @param self     a text buffer
  * @param pen      position of text start
  * @param markup   markup to be used to add text
  * @param current  charactr to be added
  * @param previous previous character (if any)
  */
  void
  text_buffer_add_wchar( text_buffer_t * self,
                         vec2 * pen, markup_t * markup,
                         wchar_t current, wchar_t previous );

/**
  * Clear text buffer
  *
  * @param self a text buffer
 */
  void
  text_buffer_clear( text_buffer_t * self );


/** @} */

#ifdef __cplusplus
}
}
#endif

#endif /* #define __TEXT_BUFFER_H__ */
