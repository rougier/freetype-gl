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

    self->font                = 0;

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
