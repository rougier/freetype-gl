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

#include "texture-font.h"
#include "vec234.h"


/**
 *
 */
typedef struct
{
    /**
     *
     */
    char * family;

    /**
     *
     */
    float size;

    /**
     *
     */
    int bold;

    /**
     *
     */
    int italic;

    /**
     *
     */
    float rise;

    /**
     *
     */
    float spacing;

    /**
     *
     */
    vec4 foreground_color;

    /**
     *
     */
    vec4 background_color;

    /**
     *
     */
    int outline;

    /**
     *
     */
    vec4 outline_color;

    /**
     *
     */
    int underline;

    /**
     *
     */
    vec4 underline_color;

    /**
     *
     */
    int overline;

    /**
     *
     */
    vec4 overline_color;

    /**
     *
     */
    int strikethrough;

    /**
     *
     */
    vec4 strikethrough_color;

    /**
     *
     */
    texture_font_t * font;
} markup_t;


markup_t *     markup_new( void );
markup_t *     markup_copy( const markup_t *other );

int          markup_cmp( const markup_t *self,
                         const markup_t *other );
void         markup_delete( markup_t *self );

const char * markup_get_family( const markup_t *self );
void         markup_set_family( markup_t *self,
                                const char *family );

int  markup_get_italic( const markup_t *self );
void markup_set_italic( markup_t *self,
                        const int italic );

int  markup_get_bold( const markup_t *self );
void markup_set_bold( markup_t *self,
                      const int bold );

float markup_get_size( const markup_t *self );
void  markup_set_size( markup_t *self,
                       const float size );

float markup_get_rise( const markup_t *self );
void  markup_set_rise( markup_t *self,
                       const float rise );

float markup_get_spacing( const markup_t *self );
void  markup_set_spacing( markup_t *self,
                          const float spacing );

vec4  markup_get_foreground_color( const markup_t *self );
void  markup_set_foreground_color( markup_t *self,
                                   const vec4 * color );

vec4 markup_get_background_color( const markup_t *self );
void markup_set_background_color( markup_t *self,
                                         const vec4 * color );

int  markup_get_outline( const markup_t *self );
void markup_set_outline( markup_t *self,
                         const int outline );

vec4 markup_get_outline_color( const markup_t *self );
void markup_set_outline_color( markup_t *self,
                               const vec4 * color );

int  markup_get_underline( const markup_t *self );
void markup_set_underline( markup_t *self,
                           const int underline );

vec4 markup_get_underline_color( const markup_t *self );
void markup_set_underline_color( markup_t *self,
                                 const vec4 * color );

int  markup_get_overline( const markup_t *self );
void markup_set_overline( markup_t *self,
                          const int overline );

vec4 markup_get_overline_color( const markup_t *self );
void markup_set_overline_color( markup_t *self,
                                const vec4 * color );

int  markup_get_strikethrough( const markup_t *self );
void markup_set_strikethrough( markup_t *self,
                               const int strikethrough );

vec4 markup_get_strikethrough_color( const markup_t *self );
void markup_set_strikethrough_color( markup_t *self,
                                     const vec4 * color );

#endif /* __MARKUP_H__ */
