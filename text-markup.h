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
#ifndef __TEXT_MARKUP_H__
#define __TEXT_MARKUP_H__

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
} TextMarkup;


TextMarkup * text_markup_new( void );
TextMarkup * text_markup_copy( const TextMarkup *other );
void         text_markup_delete( TextMarkup *self );
const char * text_markup_get_family( TextMarkup *self );
void         text_markup_set_family( TextMarkup *self,
                                     const char *family );
int          text_markup_get_italic( TextMarkup *self );
void         text_markup_set_italic( TextMarkup *self,
                                     const int italic );
int          text_markup_get_bold( TextMarkup *self );
void         text_markup_set_bold( TextMarkup *self,
                                   const int bold );
float        text_markup_get_size( TextMarkup *self );
void         text_markup_set_size( TextMarkup *self,
                                   const float size );
float        text_markup_get_rise( TextMarkup *self );
void         text_markup_set_rise( TextMarkup *self,
                                   const float rise );
float        text_markup_get_spacing( TextMarkup *self );
void         text_markup_set_spacing( TextMarkup *self,
                                      const float spacing );
Color        text_markup_get_foreground_color( TextMarkup *self );
void         text_markup_set_foreground_color( TextMarkup *self,
                                               const Color * color );
Color        text_markup_get_background_color( TextMarkup *self );
void         text_markup_set_background_color( TextMarkup *self,
                                               const Color * color );
int          text_markup_get_outline( TextMarkup *self );
void         text_markup_set_outline( TextMarkup *self,
                                      const int outline );
Color        text_markup_get_outline_color( TextMarkup *self );
void         text_markup_set_outline_color( TextMarkup *self,
                                            const Color * color );
int          text_markup_get_underline( TextMarkup *self );
void         text_markup_set_underline( TextMarkup *self,
                                        const int underline );
Color        text_markup_get_underline_color( TextMarkup *self );
void         text_markup_set_underline_color( TextMarkup *self,
                                              const Color * color );
int          text_markup_get_overline( TextMarkup *self );
void         text_markup_set_overline( TextMarkup *self,
                                       const int overline );
Color        text_markup_get_overline_color( TextMarkup *self );
void         text_markup_set_overline_color( TextMarkup *self,
                                             const Color * color );
int          text_markup_get_strikethrough( TextMarkup *self );
void         text_markup_set_strikethrough( TextMarkup *self,
                                            const int strikethrough );
Color        text_markup_get_strikethrough_color( TextMarkup *self );
void         text_markup_set_strikethrough_color( TextMarkup *self,
                                                  const Color * color );


#endif /* __TEXT_MARKUP_H__ */
