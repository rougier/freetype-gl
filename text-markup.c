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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "font-manager.h"
#include "text-markup.h"

TextMarkup *
text_markup_new( void )
{
    Color black = {0,0,0,1};
    Color white = {1,1,1,1};

    TextMarkup *self = (TextMarkup *) malloc( sizeof(TextMarkup) );
    if( !self )
    {
        return NULL;
    }

    self->family = strdup("monotype");
    self->italic = 0;
    self->bold   = 0;
    self->size   = 16;
    self->rise   = 0;
    self->spacing= 0;

    self->foreground_color    = black;
    self->background_color    = white;
    self->underline           = 0;
    self->underline_color     = black;
    self->overline            = 0;
    self->overline_color      = black;
    self->strikethrough       = 0;
    self->strikethrough_color = black;

    return self;
}

void
text_markup_delete( TextMarkup *self )
{
    assert( self );
   
    free( self->family );
    free( self );
}


const char *
text_markup_get_family( TextMarkup *self )
{
    assert( self );
    return self->family;
}

void
text_markup_set_family( TextMarkup *self, const char *family )
{
}

int
text_markup_get_italic( TextMarkup *self )
{
    assert( self );
    return self->italic;
}

void
text_markup_set_italic( TextMarkup *self, const int italic )
{
}

int
text_markup_get_bold( TextMarkup *self )
{
    assert( self );
    return self->bold;
}

void
text_markup_set_bold( TextMarkup *self, const int bold )
{
}

float
text_markup_get_size( TextMarkup *self )
{
    assert( self );
    return self->size;
}

void
text_markup_set_size( TextMarkup *self, const float size )
{
}

float
text_markup_get_rise( TextMarkup *self )
{
    assert( self );
    return self->rise;
}

void
text_markup_set_rise( TextMarkup *self, const float rise )
{
}

float
text_markup_get_spacing( TextMarkup *self )
{
    assert( self );
    return self->spacing;
}

void
text_markup_set_spacing( TextMarkup *self, const float spacing )
{
}

Color
text_markup_get_foreground_color( TextMarkup *self )
{
    assert( self );
    return self->foreground_color;
}

void
text_markup_set_foreground_color( TextMarkup *self, const Color * color )
{
}

Color
text_markup_get_background_color( TextMarkup *self )
{
    assert( self );
    return self->background_color;
}

void
text_markup_set_background_color( TextMarkup *self, const Color * color )
{
}

int
text_markup_get_outline( TextMarkup *self )
{
    assert( self );
    return self->outline;
}

void
text_markup_set_outline( TextMarkup *self,
                         const int outline )
{
}

Color
text_markup_get_outline_color( TextMarkup *self )
{
    assert( self );
    return self->outline_color;
}

void
text_markup_set_outline_color( TextMarkup *self,
                               const Color * color )
{
}

int
text_markup_get_underline( TextMarkup *self )
{
    assert( self );
    return self->underline;
}

void
text_markup_set_underline( TextMarkup *self, const int underline )
{
}

Color
text_markup_get_underline_color( TextMarkup *self )
{
    assert( self );
    return self->underline_color;
}

void
text_markup_set_underline_color( TextMarkup *self, const Color * color )
{
}

int
text_markup_get_overline( TextMarkup *self )
{
    assert( self );
    return self->overline;
}

void
text_markup_set_overline( TextMarkup *self, const int overline )
{
}

Color
text_markup_get_overline_color( TextMarkup *self )
{
    assert( self );
    return self->overline_color;
}

void
text_markup_set_overline_color( TextMarkup *self, const Color * color )
{
}

int
text_markup_get_strikethrough( TextMarkup *self )
{
    assert( self );
    return self->strikethrough;
}

void
text_markup_set_strikethrough( TextMarkup *self, const int strikethrough )
{
}

Color
text_markup_get_strikethrough_color( TextMarkup *self )
{
    assert( self );
    return self->strikethrough_color;
}

void
text_markup_set_strikethrough_color( TextMarkup *self, const Color * color )
{
}
