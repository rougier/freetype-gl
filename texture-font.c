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
#include <ft2build.h>
#include FT_FREETYPE_H
// #include FT_ADVANCES_H
#include FT_LCD_FILTER_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "texture-font.h"
#include "texture-glyph.h"

#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };
const struct {
    int          code;
    const char*  message;
} FT_Errors[] =
#include FT_ERRORS_H

#define max(a,b) (a)>(b)?(a):(b)
#define min(a,b) (a)<(b)?(a):(b)


/* ------------------------------------------------------------------------- */
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
    self->gamma = 1.;
    self->atlas = atlas;
    self->height = 0;
    self->ascender = 0;
    self->descender = 0;
    self->hinting = 1;
    self->lcd_filter = 0;
    self->lcd_weights[0] = 0;
    self->lcd_weights[1] = 0;
    self->lcd_weights[2] = 255;
    self->lcd_weights[3] = 0;
    self->lcd_weights[4] = 0;

    /* Get font metrics at high resolution */
    FT_Library library;
    FT_Face face;
    if( !texture_font_load_face( &library, self->filename, self->size*100, &face ) )
    {
        return self;
    }

    FT_Size_Metrics metrics = face->size->metrics; 
    self->ascender  = (metrics.ascender >> 6) / 100.0;
    self->descender = (metrics.descender >> 6) / 100.0;
    self->height    = (metrics.height >> 6) / 100.0;
    self->linegap   = self->height - self->ascender + self->descender;

    return self;
}



/* ------------------------------------------------------------------------- */
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



/* ------------------------------------------------------------------------- */
void
texture_font_generate_kerning( TextureFont *self )
{
    size_t i, j, k, count;
    FT_Library   library;
    FT_Face      face;
    FT_UInt      glyph_index, prev_index;
    TextureGlyph *glyph, *prev_glyph;
    FT_Vector    kerning;


    /* Load font */
    if( !texture_font_load_face( &library, self->filename, self->size, &face ) )
    {
        return;
    }

    /* For each glyph couple combination, check if kerning is necessary */
    for( i=0; i<self->glyphs->size; ++i )
    {

        glyph = (TextureGlyph *) vector_get( self->glyphs, i );

        /* Remove any old kerning information */
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
            prev_glyph = (TextureGlyph *) vector_get( self->glyphs, j );
            prev_index = FT_Get_Char_Index( face, prev_glyph->charcode );
            FT_Get_Kerning( face, prev_index, glyph_index, FT_KERNING_UNFITTED, &kerning );
            if( kerning.x != 0.0 )
            {
                count++;
            }
        }
      
        /* No kerning at all */
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
            prev_glyph = (TextureGlyph *) vector_get( self->glyphs, j );
            prev_index = FT_Get_Char_Index( face, prev_glyph->charcode );
            FT_Get_Kerning( face, prev_index, glyph_index, FT_KERNING_UNFITTED, &kerning );
            if( kerning.x != 0.0 )
            {
                glyph->kerning[k].charcode = prev_glyph->charcode;
                // 64 * 64 because of 26.6 encoding AND the transform matrix used
                // in texture_font_load_face (hres = 64)
                glyph->kerning[k].kerning = kerning.x/ (float)(64.0f*64.0f);
                ++k;
            }
        }
    }

    FT_Done_Face( face );
    FT_Done_FreeType( library );
}



/* ------------------------------------------------------------------------- */
size_t
texture_font_cache_glyphs( TextureFont *self,
                           wchar_t * charcodes )
{
    size_t i, x, y, width, height, depth, w, h;
    FT_Library    library;
    FT_Error      error;
    FT_Face       face;
    FT_GlyphSlot  slot;
    FT_UInt       glyph_index;
    TextureGlyph *glyph;
    Region        region;
    unsigned char c;
    size_t        missed = 0;
    width  = self->atlas->width;
    height = self->atlas->height;
    depth  = self->atlas->depth;

    if( !texture_font_load_face( &library, self->filename, self->size, &face ) )
    {
        return wcslen(charcodes);
    }

    /* Load each glyph */
    for( i=0; i<wcslen(charcodes); ++i )
    {
        glyph_index = FT_Get_Char_Index( face, charcodes[i] );

        // WARNING: We use texture-atlas depth to guess if user wants
        //          LCD subpixel rendering
        FT_Int32 flags = FT_LOAD_RENDER;

        if( !self->hinting )
        {
            flags |= FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;
        }
        else
        {
            flags |= FT_LOAD_FORCE_AUTOHINT;
        }

        if( depth == 3 )
        {
            FT_Library_SetLcdFilter( library, FT_LCD_FILTER_LIGHT );
            flags |= FT_LOAD_TARGET_LCD;
            if( self->lcd_filter )
            {
                FT_Library_SetLcdFilterWeights( library, self->lcd_weights );
            }
        }
        error = FT_Load_Glyph( face, glyph_index, flags );

        if( error )
        {
            fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                    __LINE__, FT_Errors[error].code, FT_Errors[error].message);
            FT_Done_FreeType( library );
            return wcslen(charcodes)-i;
        }
        slot = face->glyph;

        /* Gamma correction (sort of) */
        for( x=0; x<slot->bitmap.width; ++x )
        {
            for( y=0; y<slot->bitmap.rows; ++y )
            {
                c = *(unsigned char *)(slot->bitmap.buffer
                                       + y*slot->bitmap.pitch + x );
                c = (unsigned char) ( pow(c/255.0, self->gamma) * 255);
                *(unsigned char *)(slot->bitmap.buffer
                                   + y*slot->bitmap.pitch + x ) = c;
            }
        }

        // We want each glyph to be separated by at least one black pixel
        // (for example for shader used in demo-subpixel.c)
        w = slot->bitmap.width/depth + 1;
        h = slot->bitmap.rows + 1;
        region = texture_atlas_get_region( self->atlas, w, h );
        if ( region.x < 0 )
        {
            missed++;
            continue;
        }
        w = w - 1;
        h = h - 1;
        x = region.x;
        y = region.y;
        texture_atlas_set_region( self->atlas, x, y, w, h,
                                  slot->bitmap.buffer, slot->bitmap.pitch );

        glyph = texture_glyph_new( );
        glyph->font = self;
        glyph->charcode = charcodes[i];
        glyph->kerning  = 0;
        glyph->width    = w;
        glyph->height   = h;
        glyph->offset_x = slot->bitmap_left;
        glyph->offset_y = slot->bitmap_top;
        glyph->u0       = x/(float)width;
        glyph->v0       = y/(float)height;
        glyph->u1       = (x + glyph->width)/(float)width;
        glyph->v1       = (y + glyph->height)/(float)height;

        /* Discard hinting to get advance */
        FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
        slot = face->glyph;
        glyph->advance_x    = slot->advance.x/64.0;
        glyph->advance_y    = slot->advance.y/64.0;

        vector_push_back( self->glyphs, glyph );
        texture_glyph_delete( glyph );
    }
    FT_Done_Face( face );
    FT_Done_FreeType( library );
    texture_atlas_upload( self->atlas );
    texture_font_generate_kerning( self );
    return missed;
}



/* ------------------------------------------------------------------------- */
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
        return (TextureGlyph *) vector_back( self->glyphs );
    }
    return NULL;
}



/* ------------------------------------------------------------------------- */
int
texture_font_load_face( FT_Library * library,
                        const char * filename,
                        const float size,
                        FT_Face * face )
{
    size_t hres = 64;
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
        FT_Done_Face( *face );
        FT_Done_FreeType( *library );
        return 0;
    }

    /* Set char size */
    error = FT_Set_Char_Size( *face, (int)(size*64), 0, 72*hres, 72 );

    /* error = FT_Set_Char_Size( *face, size*64, 0, 72, 72 ); */
    if( error )
    {
        fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                __LINE__, FT_Errors[error].code, FT_Errors[error].message);
        FT_Done_Face( *face );
        FT_Done_FreeType( *library );
        return 0;
    }

    /* Set transform matrix */
    FT_Set_Transform( *face, &matrix, NULL );

    return 1;
}
                       
