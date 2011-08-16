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
 * ========================================================================= */
#pragma once
#ifndef __MARKUP_H__
#define __MARKUP_H__

typedef struct {
    float r,g,b,a;
} Color;

typedef struct
{
    char *        family;
    float         size;
    int           bold;
    int           italic;
    float         rise;
    float         spacing;
    Color         foreground_color;
    Color         background_color;
    int           outline;
    Color         outline_color;
    int           underline;
    Color         underline_color;
    int           overline;
    Color         overline_color;
    int           strikethrough;
    Color         strikethrough_color;
    struct TextureFont_  * font;
} Markup;


Markup *     markup_new( void );
Markup *     markup_copy( const Markup *other );


int          markup_cmp( const Markup *self,
                         const Markup *other );

void         markup_delete( Markup *self );
const char * markup_get_family( Markup *self );
void         markup_set_family( Markup *self,
                                const char *family );
int          markup_get_italic( Markup *self );
void         markup_set_italic( Markup *self,
                                const int italic );
int          markup_get_bold( Markup *self );
void         markup_set_bold( Markup *self,
                              const int bold );
float        markup_get_size( Markup *self );
void         markup_set_size( Markup *self,
                              const float size );
float        markup_get_rise( Markup *self );
void         markup_set_rise( Markup *self,
                              const float rise );
float        markup_get_spacing( Markup *self );
void         markup_set_spacing( Markup *self,
                                 const float spacing );
Color        markup_get_foreground_color( Markup *self );
void         markup_set_foreground_color( Markup *self,
                                          const Color * color );
Color        markup_get_background_color( Markup *self );
void         markup_set_background_color( Markup *self,
                                          const Color * color );
int          markup_get_outline( Markup *self );
void         markup_set_outline( Markup *self,
                                 const int outline );
Color        markup_get_outline_color( Markup *self );
void         markup_set_outline_color( Markup *self,
                                       const Color * color );
int          markup_get_underline( Markup *self );
void         markup_set_underline( Markup *self,
                                   const int underline );
Color        markup_get_underline_color( Markup *self );
void         markup_set_underline_color( Markup *self,
                                         const Color * color );
int          markup_get_overline( Markup *self );
void         markup_set_overline( Markup *self,
                                  const int overline );
Color        markup_get_overline_color( Markup *self );
void         markup_set_overline_color( Markup *self,
                                        const Color * color );
int          markup_get_strikethrough( Markup *self );
void         markup_set_strikethrough( Markup *self,
                                       const int strikethrough );
Color        markup_get_strikethrough_color( Markup *self );
void         markup_set_strikethrough_color( Markup *self,
                                             const Color * color );


#endif /* __MARKUP_H__ */
