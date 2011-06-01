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
#include <fontconfig/fontconfig.h>
#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include "font-manager.h"


wchar_t *
wcsdup( const wchar_t *string )
{
    wchar_t * result;
    assert( string );
    result = (wchar_t *) malloc( (wcslen(string) + 1) * sizeof(wchar_t) );
    wcscpy( result, string );
    return result;
}


FontManager *
font_manager_new( void )
{
    static FontManager *self = 0;
    if( !self )
    {
        TextureAtlas *atlas = texture_atlas_new( 512, 512 );
        self = (FontManager *) malloc( sizeof(FontManager) );
        if( !self )
        {
            return 0;
        }
        self->atlas = atlas;
        self->fonts = vector_new( sizeof(TextureFont) );
        self->cache = wcsdup( L" " );
/*
        self->cache = wcsdup( L" !\"#$%&'()*+,-./0123456789:;<=>?"
                              L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                              L"`abcdefghijklmnopqrstuvwxyz{|}~" );
*/
    }
    return self;
}

FontManager *
font_manager_copy( FontManager *other )
{
    return other;
}

void
font_manager_delete( FontManager *self )
{
    assert( self );
    vector_delete( self->fonts );
    texture_atlas_delete( self->atlas );
    if( self->cache )
    {
        free( self->cache );
    }
    free( self );
}

TextureFont *
font_manager_get_from_filename( FontManager *self,
                                const char * filename,
                                const float size )
{
    size_t i;
    TextureFont *font;

    assert( self );

    for( i=0; i<self->fonts->size;++i )
    {
        font = (TextureFont *) vector_get( self->fonts, i );
        if( (strcmp(font->filename, filename) == 0) && ( font->size == size) )
        {
            return font;
        }
    }
    font = texture_font_new( self->atlas, filename, size );
    texture_font_cache_glyphs( font, self->cache );
    if( font )
    {
        vector_push_back( self->fonts, font );
    }
    return font;
}


TextureFont *
font_manager_get_from_description( FontManager *self,
                                   const char * family,
                                   const float size,
                                   const int bold,
                                   const int italic )
{
    TextureFont *font;
    char *filename = font_manager_match_description( self, family, size, bold, italic );

    assert( self );
    if( !filename )
    {
        return 0;
    }
    font = font_manager_get_from_filename( self, filename, size );
    free( filename );
    return font;
}

TextureFont *
font_manager_get_from_markup( FontManager *self,
                              const TextMarkup *markup )
{
    assert( self );
    assert( markup );

    return font_manager_get_from_description( self, markup->family, markup->size,
                                                    markup->bold,   markup->italic );
}


char *
font_manager_match_description( FontManager *self,
                                const char * family,
                                const float size,
                                const int bold,
                                const int italic )
{
    char *filename = 0;
    int weight = FC_WEIGHT_REGULAR;
    int slant = FC_SLANT_ROMAN;
    if ( bold )
    {
        weight = FC_WEIGHT_BOLD;
    }
    if( italic )
    {
        slant = FC_SLANT_ITALIC;
    }
    FcInit();
    FcPattern *pattern = FcPatternCreate();
    FcPatternAddDouble( pattern, FC_SIZE, size );
    FcPatternAddInteger( pattern, FC_WEIGHT, weight );
    FcPatternAddInteger( pattern, FC_SLANT, slant );
    FcPatternAddString( pattern, FC_FAMILY, (FcChar8*) family );
    FcConfigSubstitute( 0, pattern, FcMatchPattern );
    FcDefaultSubstitute( pattern );
    FcResult result;
    FcPattern *match = FcFontMatch( 0, pattern, &result );
    FcPatternDestroy( pattern );

    if ( !match )
    {
        fprintf( stderr, "fontconfig error: could not match family '%s'", family );
        return 0;
    }
    else
    {
        FcValue value;
        FcResult result = FcPatternGet( match, FC_FILE, 0, &value );
        if ( result )
        {
            fprintf( stderr, "fontconfig error: could not match family '%s'", family );
        }
        else
        {
            filename = strdup( (char *)(value.u.s) );
        }
    }
    FcPatternDestroy( match );
    return filename;
}


const wchar_t *
font_manager_get_cache( FontManager *self )
{
    assert( self );
    return self->cache;
}

void
font_manager_set_cache( FontManager *self,
                        const wchar_t * cache )
{
    assert( self );
    assert( cache );

    if( self->cache )
    {
        free( self->cache );
    }
    self->cache = wcsdup( cache );
}
