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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "font-manager.h"
#include "markup.h"

// ------------------------------------------------------------- markup_new ---
markup_t *
markup_new( void )
{
    vec4 black = {{0,0,0,1}};
    vec4 white = {{1,1,1,1}};

    markup_t * self = (markup_t *) malloc( sizeof(markup_t) );
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

    self->font = 0;

    return self;
}


// ---------------------------------------------------------- markup_delete ---
void
markup_delete( markup_t * self )
{
    assert( self );
   
    free( self->family );
    free( self );
}

// ------------------------------------------------------------- markup_cmp ---
int
markup_cmp( const markup_t * self,
            const markup_t *other )
{
    size_t n = sizeof( markup_t ) - sizeof( texture_font_t * );
    return memcmp( self, other, n );
}


// ------------------------------------------------------ markup_get_family ---
const char *
markup_get_family( const markup_t * self )
{
    assert( self );

    return self->family;
}


// ------------------------------------------------------ markup_set_family ---
void
markup_set_family( markup_t * self,
                   const char *family )
{
    assert( self );
    assert( family );
}


// ------------------------------------------------------ markup_get_italic ---
int
markup_get_italic( const markup_t * self )
{
    assert( self );

    return self->italic;
}


// ------------------------------------------------------ markup_set_italic ---
void
markup_set_italic( markup_t * self,
                   const int italic )
{
    assert( self );
}


// -------------------------------------------------------- markup_get_bold ---
int
markup_get_bold( const markup_t * self )
{
    assert( self );

    return self->bold;
}

// -------------------------------------------------------- markup_set_bold ---
void
markup_set_bold( markup_t * self,
                 const int bold )
{
    assert( self );

}

// -------------------------------------------------------- markup_get_size ---
float
markup_get_size( const markup_t * self )
{
    assert( self );

    return self->size;
}


// -------------------------------------------------------- markup_set_size ---
void
markup_set_size( markup_t * self,
                 const float size )
{
    assert( self );

}


// -------------------------------------------------------- markup_get_rise ---
float
markup_get_rise( const markup_t * self )
{
    assert( self );

    return self->rise;
}


// -------------------------------------------------------- markup_set_rise ---
void
markup_set_rise( markup_t * self,
                 const float rise )
{
    assert( self );
}


// ----------------------------------------------------- markup_get_spacing ---
float
markup_get_spacing( const markup_t * self )
{
    assert( self );

    return self->spacing;
}


// ----------------------------------------------------- markup_set_spacing ---
void
markup_set_spacing( markup_t * self,
                    const float spacing )
{
    assert( self );

}

// -------------------------------------------- markup_get_foreground_color ---
vec4
markup_get_foreground_color( const markup_t * self )
{
    assert( self );

    return self->foreground_color;
}


// -------------------------------------------- markup_set_foreground_color ---
void
markup_set_foreground_color( markup_t * self,
                             const vec4 * color )
{
    assert( self );
    assert( color );

}


// -------------------------------------------- markup_get_background_color ---
vec4
markup_get_background_color( const markup_t * self )
{
    assert( self );

    return self->background_color;
}


// -------------------------------------------- markup_get_background_color ---
void
markup_set_background_color( markup_t * self, const vec4 * color )
{
    assert( self );
    assert( color );
}

// ----------------------------------------------------- markup_get_outline ---
int
markup_get_outline( const markup_t * self )
{
    assert( self );

    return self->outline;
}


// ----------------------------------------------------- markup_set_outline ---
void
markup_set_outline( markup_t * self,
                    const int outline )
{
    assert( self );

}


// ----------------------------------------------- markup_get_outline_color ---
vec4
markup_get_outline_color( const markup_t * self )
{
    assert( self );

    return self->outline_color;
}


// ----------------------------------------------- markup_set_outline_color ---
void
markup_set_outline_color( markup_t * self,
                          const vec4 * color )
{
    assert( self );
    assert( color );

}

// --------------------------------------------------- markup_get_underline ---
int
markup_get_underline( const markup_t * self )
{
    assert( self );

    return self->underline;
}


// --------------------------------------------------- markup_set_underline ---
void
markup_set_underline( markup_t * self,
                      const int underline )
{
    assert( self );

}


// --------------------------------------------- markup_get_underline_color ---
vec4
markup_get_underline_color( const markup_t * self )
{
    assert( self );
    return self->underline_color;
}


// --------------------------------------------- markup_set_underline_color ---
void
markup_set_underline_color( markup_t * self,
                            const vec4 * color )
{
    assert( self );
    assert( color );
}


// ---------------------------------------------------- markup_get_overline ---
int
markup_get_overline( const markup_t * self )
{
    assert( self );

    return self->overline;
}


// ---------------------------------------------------- markup_set_overline ---
void
markup_set_overline( markup_t * self,
                     const int overline )
{
    assert( self );

}


// ---------------------------------------------- markup_get_overline_color ---
vec4
markup_get_overline_color( const markup_t * self )
{
    assert( self );

    return self->overline_color;
}


// ---------------------------------------------- markup_set_overline_color ---
void
markup_set_overline_color( markup_t * self,
                           const vec4 * color )
{
    assert( self );
    assert( color );
}


// ----------------------------------------------- markup_get_strikethrough ---
int
markup_get_strikethrough( const markup_t * self )
{
    assert( self );
    return self->strikethrough;
}


// ----------------------------------------------- markup_set_strikethrough ---
void
markup_set_strikethrough( markup_t * self,
                          const int strikethrough )
{
    assert( self );

}


// ---------------------------------------- markup_get_strikethrought_color ---
vec4
markup_get_strikethrough_color( const markup_t * self )
{
    assert( self );

    return self->strikethrough_color;
}


// ---------------------------------------- markup_set_strikethrought_color ---
void
markup_set_strikethrough_color( markup_t * self,
                                const vec4 * color )
{
    assert( self );
    assert( color );
}
