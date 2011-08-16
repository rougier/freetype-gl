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
#include "markup.h"

Markup *
markup_new( void )
{
    Color black = {0,0,0,1};
    Color white = {1,1,1,1};

    Markup *self = (Markup *) malloc( sizeof(Markup) );
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
markup_delete( Markup *self )
{
    assert( self );
   
    free( self->family );
    free( self );
}


int
markup_cmp( const Markup *self,
            const Markup *other )
{
    size_t n = sizeof( Markup ) - sizeof( TextureFont * );
    return memcmp( self, other, n );
}


const char *
markup_get_family( Markup *self )
{
    assert( self );
    return self->family;
}

void
markup_set_family( Markup *self,
                   const char *family )
{
}

int
markup_get_italic( Markup *self )
{
    assert( self );
    return self->italic;
}

void
markup_set_italic( Markup *self,
                   const int italic )
{
}

int
markup_get_bold( Markup *self )
{
    assert( self );
    return self->bold;
}

void
markup_set_bold( Markup *self,
                 const int bold )
{
}

float
markup_get_size( Markup *self )
{
    assert( self );

    return self->size;
}

void
markup_set_size( Markup *self,
                 const float size )
{
}

float
markup_get_rise( Markup *self )
{
    assert( self );
    return self->rise;
}

void
markup_set_rise( Markup *self, const float rise )
{
}

float
markup_get_spacing( Markup *self )
{
    assert( self );
    return self->spacing;
}

void
markup_set_spacing( Markup *self, const float spacing )
{
}

Color
markup_get_foreground_color( Markup *self )
{
    assert( self );
    return self->foreground_color;
}

void
markup_set_foreground_color( Markup *self, const Color * color )
{
}

Color
markup_get_background_color( Markup *self )
{
    assert( self );
    return self->background_color;
}

void
markup_set_background_color( Markup *self, const Color * color )
{
}

int
markup_get_outline( Markup *self )
{
    assert( self );
    return self->outline;
}

void
markup_set_outline( Markup *self,
                    const int outline )
{
}

Color
markup_get_outline_color( Markup *self )
{
    assert( self );
    return self->outline_color;
}

void
markup_set_outline_color( Markup *self,
                          const Color * color )
{
}

int
markup_get_underline( Markup *self )
{
    assert( self );
    return self->underline;
}

void
markup_set_underline( Markup *self,
                      const int underline )
{
}

Color
markup_get_underline_color( Markup *self )
{
    assert( self );
    return self->underline_color;
}

void
markup_set_underline_color( Markup *self,
                            const Color * color )
{
}

int
markup_get_overline( Markup *self )
{
    assert( self );
    return self->overline;
}

void
markup_set_overline( Markup *self,
                     const int overline )
{
}

Color
markup_get_overline_color( Markup *self )
{
    assert( self );
    return self->overline_color;
}

void
markup_set_overline_color( Markup *self,
                           const Color * color )
{
}

int
markup_get_strikethrough( Markup *self )
{
    assert( self );
    return self->strikethrough;
}

void
markup_set_strikethrough( Markup *self,
                          const int strikethrough )
{
}

Color
markup_get_strikethrough_color( Markup *self )
{
    assert( self );
    return self->strikethrough_color;
}

void
markup_set_strikethrough_color( Markup *self,
                                const Color * color )
{
}
