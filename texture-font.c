// ============================================================================
// Freetype GL - An ansi C OpenGL Freetype engine
// Platform:    Any
// API version: 1.0
// WWW:         http://www.loria.fr/~rougier/freetype-gl
// ----------------------------------------------------------------------------
// Copyright (c) 2011 Nicolas P. Rougier <Nicolas.Rougier@inria.fr>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program. If not, see <http://www.gnu.org/licenses/>.
// ============================================================================
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "texture-font.h"

#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };
const struct {
    int          code;
    const char*  message;
} FT_Errors[] =
#include FT_ERRORS_H



TextureFont *
texture_font_new( TextureAtlas *atlas,
                  const char *filename,
                  const float size )
{
    TextureFont *self = (TextureFont *) malloc( sizeof(TextureFont) );
    if( !self )
    {
        return NULL;
    }
    self->glyphs = vector_new( sizeof(TextureGlyph) );
    self->filename = strdup( filename );
    self->size = size;
    self->gamma = 1.25;
    self->atlas = atlas;
    return self;
}



void
texture_font_delete( TextureFont *self )
{
    assert( self );
    if( self->filename )
    {
        free( self->filename );
    }
    vector_delete( self->glyphs );
    free( self );
}


void
texture_font_generate_kerning( TextureFont *self )
{
    size_t i, j, k, count;
    FT_Library   library;
    FT_Face      face;
    FT_UInt      glyph_index, prev_index;
    TextureGlyph *glyph, *prev_glyph;
    FT_Vector    kerning;

    if( !texture_font_load_face( &library, self->filename, self->size, &face ) )
    {
        return;
    }

    for( i=0; i<self->glyphs->size; ++i )
    {
        glyph = (TextureGlyph *) vector_get( self->glyphs, i );
        if( glyph->kerning )
        {
            free( glyph->kerning );
            glyph->kerning = 0;
            glyph->kerning_count = 0;
        }

        /* Count how many kerning pairs we need */
        count = 0;
        glyph_index = FT_Get_Char_Index( face, glyph->charcode );
        for( j=0; j<self->glyphs->size; ++j )
        {
            prev_glyph = (TextureGlyph *) vector_get( self->glyphs, i );
            prev_index = FT_Get_Char_Index( face, prev_glyph->charcode );
            FT_Get_Kerning( face, prev_index, glyph_index, FT_KERNING_UNSCALED, &kerning );
            if( kerning.x != 0.0 )
            {
                count++;
            }
        }
      
        /* No kerning necessary */
        if( !count )
        {
            continue;
        }
        /* Store kerning pairs */
        glyph->kerning = (KerningPair *) malloc( count * sizeof(KerningPair) );
        glyph->kerning_count = count;
        k = 0;
        for( j=0; j<self->glyphs->size; ++j )
        {
            prev_glyph = (TextureGlyph *) vector_get( self->glyphs, i );
            prev_index = FT_Get_Char_Index( face, prev_glyph->charcode );
            FT_Get_Kerning( face, prev_index, glyph_index, FT_KERNING_UNSCALED, &kerning );
            if( kerning.x != 0.0 )
            {
                glyph->kerning[k].charcode = prev_glyph->charcode;
                glyph->kerning[k].kerning = kerning.x / 64.0f;
                ++k;
            }
        }
    }

    FT_Done_FreeType( library );
}



size_t
texture_font_cache_glyphs( TextureFont *self,
                           wchar_t * charcodes )
{
    size_t i, x, y, width, height;
    FT_Library    library;
    FT_Error      error;
    FT_Face       face;
    FT_GlyphSlot  slot;
    FT_UInt       glyph_index;
    TextureGlyph *glyph;
    ivec2         pos;
    unsigned char c;
    size_t        missed = 0;
    width  = self->atlas->width;
    height = self->atlas->height;

    if( !texture_font_load_face( &library, self->filename, self->size, &face ) )
    {
        return wcslen(charcodes);
    }

    /* Load each glyph */
    for( i=0; i<wcslen(charcodes); ++i )
    {
        glyph_index = FT_Get_Char_Index( face, charcodes[i] );
        error = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT );
        if( error )
        {
            fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                    __LINE__, FT_Errors[error].code, FT_Errors[error].message);
            FT_Done_FreeType( library );
            return wcslen(charcodes)-i;
        }
        slot = face->glyph;
        for( x=0; x<slot->bitmap.width; ++x )
        {
            for( y=0; y<slot->bitmap.rows; ++y )
            {
                c = *(unsigned char *)(slot->bitmap.buffer + y*slot->bitmap.pitch + x );
                c = (unsigned char) ( pow(c/255.0, self->gamma) * 255);
                *(unsigned char *)(slot->bitmap.buffer + y*slot->bitmap.pitch + x ) = c;
            }
        }
        pos = texture_atlas_get_region( self->atlas,
                                        slot->bitmap.width, slot->bitmap.rows );
        if ( pos.x < 0 )
        {
            missed++;
            continue;
        }
        texture_atlas_set_region( self->atlas, pos.x, pos.y,
                                  slot->bitmap.width, slot->bitmap.rows,
                                  slot->bitmap.buffer, slot->bitmap.pitch );

        glyph = texture_glyph_new( );
        glyph->charcode     = charcodes[i];
        glyph->kerning      = 0;
        glyph->size.width   = slot->bitmap.width;
        glyph->size.height  = slot->bitmap.rows;
        glyph->offset.x     = slot->bitmap_left;
        glyph->offset.y     = slot->bitmap_top;
        glyph->texcoords.u0 = pos.x/(float)width;
        glyph->texcoords.v0 = pos.y/(float)height;
        glyph->texcoords.u1 = (pos.x + glyph->size.width)/(float)width;
        glyph->texcoords.v1 = (pos.y + glyph->size.height)/(float)height;

        /* Discard hinting to get advance */
        FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
        slot = face->glyph;
        glyph->advance.x    = slot->advance.x/64.0;
        glyph->advance.y    = slot->advance.y/64.0;

        vector_push_back( self->glyphs, glyph );
        texture_glyph_delete( glyph );
    }
    FT_Done_FreeType( library );
    texture_atlas_upload( self->atlas );
    texture_font_generate_kerning( self );
    return missed;
}



TextureGlyph *
texture_font_get_glyph( TextureFont *self,
                        wchar_t charcode )
{
    size_t i;
    static wchar_t *buffer = 0;
    TextureGlyph *glyph;

    assert( self );
    assert( self->filename );
    assert( self->atlas );

    /* Check if charcode has been already loaded */
    for( i=0; i<self->glyphs->size; ++i )
    {
        glyph = (TextureGlyph *) vector_get( self->glyphs, i );
        if( glyph->charcode == charcode )
        {
            return glyph;
        }
    }

    /* If not, load it */
    if( !buffer)
    {
        buffer = (wchar_t *) calloc( 2, sizeof(wchar_t) );
    }
    buffer[0] = charcode;

    if( texture_font_cache_glyphs( self, buffer ) == 0 )
    {
        glyph = vector_back( self->glyphs );
        return (TextureGlyph *) vector_back( self->glyphs );
    }
    return NULL;
}


int
texture_font_load_face( FT_Library * library,
                        const char * filename,
                        const float size,
                        FT_Face * face )
{
    size_t hres = 1;
    FT_Error error;
    FT_Matrix matrix = { (int)((1.0/hres) * 0x10000L),
                         (int)((0.0)      * 0x10000L),
                         (int)((0.0)      * 0x10000L),
                         (int)((1.0)      * 0x10000L) };

    /* Initialize library */
    error = FT_Init_FreeType( library );
    if( error )
    {
        fprintf(stderr, "FT_Error (0x%02x) : %s\n",
                FT_Errors[error].code, FT_Errors[error].message);
        return 0;
    }

    /* Load face */
    error = FT_New_Face( *library, filename, 0, face );
    if( error )
    {
        fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                __LINE__, FT_Errors[error].code, FT_Errors[error].message);
        FT_Done_FreeType( *library );
        return 0;
    }

    /* Select charmap */
    error = FT_Select_Charmap( *face, FT_ENCODING_UNICODE );
    if( error )
    {
        fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                __LINE__, FT_Errors[error].code, FT_Errors[error].message);
        FT_Done_FreeType( *library );
        return 0;
    }

    /* Set char size */
    error = FT_Set_Char_Size( *face, size*64, 0, 72*hres, 72 );
    /* error = FT_Set_Char_Size( *face, size*64, 0, 72, 72 ); */
    if( error )
    {
        fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                __LINE__, FT_Errors[error].code, FT_Errors[error].message);
        FT_Done_FreeType( *library );
        return 0;
    }

    /* Set transform matrix */
    FT_Set_Transform( *face, &matrix, NULL );

    return 1;
}
                       
