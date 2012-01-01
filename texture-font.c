/* ===========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * ----------------------------------------------------------------------------
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
 * ============================================================================
 */
#include <ft2build.h>
#include FT_FREETYPE_H
// #include FT_ADVANCES_H
#include FT_LCD_FILTER_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <wchar.h>
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




// ------------------------------------------------- texture_font_load_face ---
int
texture_font_load_face( FT_Library * library,
                        const char * filename,
                        const float size,
                        FT_Face * face )
{
    assert( library );
    assert( filename );
    assert( size );

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
        fprintf( stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                 __LINE__, FT_Errors[error].code, FT_Errors[error].message);
        FT_Done_FreeType( *library );
        return 0;
    }

    /* Select charmap */
    error = FT_Select_Charmap( *face, FT_ENCODING_UNICODE );
    if( error )
    {
        fprintf( stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                 __LINE__, FT_Errors[error].code, FT_Errors[error].message );
        FT_Done_Face( *face );
        FT_Done_FreeType( *library );
        return 0;
    }

    /* Set char size */
    error = FT_Set_Char_Size( *face, (int)(size*64), 0, 72*hres, 72 );
    if( error )
    {
        fprintf( stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                 __LINE__, FT_Errors[error].code, FT_Errors[error].message );
        FT_Done_Face( *face );
        FT_Done_FreeType( *library );
        return 0;
    }

    /* Set transform matrix */
    FT_Set_Transform( *face, &matrix, NULL );

    return 1;
}


// ------------------------------------------------------ texture_glyph_new ---
texture_glyph_t *
texture_glyph_new( void )
{
    texture_glyph_t *self = (texture_glyph_t *) malloc( sizeof(texture_glyph_t) );
    if( self == NULL)
    {
        fprintf( stderr,
                 "line %d: No more memory for allocating data\n", __LINE__ );
        exit( EXIT_FAILURE );
    }
    self->id        = 0;
    self->width     = 0;
    self->height    = 0;
    self->offset_x  = 0;
    self->offset_y  = 0;
    self->advance_x = 0.0;
    self->advance_y = 0.0;
    self->s0        = 0.0;
    self->t0        = 0.0;
    self->s1        = 0.0;
    self->t1        = 0.0;
    self->kerning   = vector_new( sizeof(kerning_t) );
    return self;
}


// --------------------------------------------------- texture_glyph_delete ---
void
texture_glyph_delete( texture_glyph_t *self )
{
    assert( self );

    vector_delete( self->kerning );
    free( self );
}

// ---------------------------------------------- texture_glyph_get_kerning ---
float 
texture_glyph_get_kerning( const texture_glyph_t * self,
                           const wchar_t charcode )
{
    size_t i;

    assert( self );
    for( i=0; i<vector_size(self->kerning); ++i )
    {
        kerning_t * kerning = (kerning_t *) vector_get( self->kerning, i );
        if( kerning->charcode == charcode )
        {
            return kerning->kerning;
        }
    }
    return 0;
}


// ------------------------------------------ texture_font_generate_kerning ---
void
texture_font_generate_kerning( texture_font_t *self )
{
    assert( self );

    size_t i, j;
    FT_Library library;
    FT_Face face;
    FT_UInt glyph_index, prev_index;
    texture_glyph_t *glyph, *prev_glyph;
    FT_Vector kerning;

    /* Load font */
    if( !texture_font_load_face( &library, self->filename, self->size, &face ) )
    {
        return;
    }

    /* For each glyph couple combination, check if kerning is necessary */
    for( i=0; i<self->glyphs->size; ++i )
    {
        glyph = (texture_glyph_t *) vector_get( self->glyphs, i );
        vector_clear( glyph->kerning );

        for( j=0; j<self->glyphs->size; ++j )
        {
            prev_glyph = (texture_glyph_t *) vector_get( self->glyphs, j );
            prev_index = FT_Get_Char_Index( face, prev_glyph->charcode );
            FT_Get_Kerning( face, prev_index, glyph_index, FT_KERNING_UNFITTED, &kerning );
            if( kerning.x != 0.0 )
            {
                // 64 * 64 because of 26.6 encoding AND the transform matrix used
                // in texture_font_load_face (hres = 64)
                kerning_t k = {prev_glyph->charcode, kerning.x/ (float)(64.0f*64.0f)};
                vector_push_back( glyph->kerning, &k );
            }
        }
    }
    FT_Done_Face( face );
    FT_Done_FreeType( library );
}


// ------------------------------------------------------- texture_font_new ---
texture_font_t *
texture_font_new( texture_atlas_t * atlas,
                  const char * filename,
                  const float size )
{
    assert( filename );
    assert( size );

    texture_font_t *self = (texture_font_t *) malloc( sizeof(texture_font_t) );
    if( self == NULL)
    {
        fprintf( stderr,
                 "line %d: No more memory for allocating data\n", __LINE__ );
        exit( EXIT_FAILURE );
    }
    self->glyphs = vector_new( sizeof(texture_glyph_t) );
    self->atlas = atlas;
    self->height = 0;
    self->ascender = 0;
    self->descender = 0;
    self->filename = strdup( filename );
    self->size = size;
    self->hinting = 1;
    self->filtering = 1;
    // FT_LCD_FILTER_LIGHT   is (0x00, 0x55, 0x56, 0x55, 0x00)
    // FT_LCD_FILTER_DEFAULT is (0x10, 0x40, 0x70, 0x40, 0x10)
    self->lcd_weights[0] = 0x10;
    self->lcd_weights[1] = 0x40;
    self->lcd_weights[2] = 0x70;
    self->lcd_weights[3] = 0x40;
    self->lcd_weights[4] = 0x10;

    /* Get font metrics at high resolution */
    FT_Library library;
    FT_Face face;
    if( !texture_font_load_face( &library, self->filename, self->size*100, &face ) )
    {
        return self;
    }

    FT_Size_Metrics metrics = face->size->metrics; 
    self->ascender = (metrics.ascender >> 6) / 100.0;
    self->descender = (metrics.descender >> 6) / 100.0;
    self->height = (metrics.height >> 6) / 100.0;
    self->linegap = self->height - self->ascender + self->descender;

    FT_Done_Face( face );
    FT_Done_FreeType( library );

    /* -1 is a special glyph */
    texture_font_get_glyph( self, -1 );

    return self;
}


// ---------------------------------------------------- texture_font_delete ---
void
texture_font_delete( texture_font_t *self )
{
    assert( self );

    if( self->filename )
    {
        free( self->filename );
    }
    vector_delete( self->glyphs );
    free( self );
}


// ----------------------------------------------- texture_font_load_glyphs ---
size_t
texture_font_load_glyphs( texture_font_t * self,
                          const wchar_t * charcodes )
{
    assert( self );
    assert( charcodes );

    size_t i, x, y, width, height, depth, w, h;
    FT_Library library;
    FT_Error error;
    FT_Face face;
    FT_GlyphSlot  slot;
    FT_UInt glyph_index;
    texture_glyph_t *glyph;
    ivec4 region;
    size_t missed = 0;
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
            if( self->filtering )
            {
                FT_Library_SetLcdFilterWeights( library, self->lcd_weights );
            }
        }
        error = FT_Load_Glyph( face, glyph_index, flags );

        if( error )
        {
            fprintf( stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                     __LINE__, FT_Errors[error].code, FT_Errors[error].message );
            FT_Done_FreeType( library );
            return wcslen(charcodes)-i;
        }
        slot = face->glyph;

        // We want each glyph to be separated by at least one black pixel
        // (for example for shader used in demo-subpixel.c)
        w = slot->bitmap.width/depth + 1;
        h = slot->bitmap.rows + 1;
        region = texture_atlas_get_region( self->atlas, w, h );
        if ( region.x < 0 )
        {
            missed++;
            fprintf( stderr, "Texture atlas is full (line %d)\n",  __LINE__ );
            continue;
        }
        w = w - 1;
        h = h - 1;
        x = region.x;
        y = region.y;
        texture_atlas_set_region( self->atlas, x, y, w, h,
                                  slot->bitmap.buffer, slot->bitmap.pitch );

        glyph = texture_glyph_new( );
        glyph->charcode = charcodes[i];
        glyph->width    = w;
        glyph->height   = h;
        glyph->offset_x = slot->bitmap_left;
        glyph->offset_y = slot->bitmap_top;
        glyph->s0       = x/(float)width;
        glyph->t0       = y/(float)height;
        glyph->s1       = (x + glyph->width)/(float)width;
        glyph->t1       = (y + glyph->height)/(float)height;

        // Discard hinting to get advance
        FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
        slot = face->glyph;
        glyph->advance_x    = slot->advance.x/64.0;
        glyph->advance_y    = slot->advance.y/64.0;

        vector_push_back( self->glyphs, glyph );

        // We cannot use texture_glyph_delete since it would delete the kerning
        // vector  (and the glyphs vector holds a copy)
        free( glyph );
    }
    FT_Done_Face( face );
    FT_Done_FreeType( library );
    texture_atlas_upload( self->atlas );
    texture_font_generate_kerning( self );
    return missed;
}


// ------------------------------------------------- texture_font_get_glyph ---
texture_glyph_t *
texture_font_get_glyph( texture_font_t * self,
                        wchar_t charcode )
{
    assert( self );

    size_t i;
    static wchar_t *buffer = 0;
    texture_glyph_t *glyph;

    assert( self );
    assert( self->filename );
    assert( self->atlas );

    /* Check if charcode has been already loaded */
    for( i=0; i<self->glyphs->size; ++i )
    {
        glyph = (texture_glyph_t *) vector_get( self->glyphs, i );
        if( glyph->charcode == charcode )
        {
            return glyph;
        }
    }

    /* charcode -1 is special : it is used for line drawing (overline,
     * underline, strikethrough) and background.
     */
    if( charcode == (wchar_t)(-1) )
    {
        size_t x, y, width, height, depth, w, h;
        texture_glyph_t *glyph;
        ivec4 region;

        width  = self->atlas->width;
        height = self->atlas->height;
        depth  = self->atlas->depth;
        w = 2 + 1;
        h = 2 + 1;
        region = texture_atlas_get_region( self->atlas, w, h );
        if ( region.x < 0 )
        {
            fprintf( stderr, "Texture atlas is full (line %d)\n",  __LINE__ );
            return NULL;
        }
        w = w - 1;
        h = h - 1;
        x = region.x;
        y = region.y;
        unsigned char data[4] = {255,255,255,255};
        texture_atlas_set_region( self->atlas, x, y, w, h, data, 0 );
        glyph = texture_glyph_new( );
        glyph->charcode = (wchar_t)(-1);
        glyph->width    = w;
        glyph->height   = h;
        glyph->offset_x = 0;
        glyph->offset_y = 0;
        glyph->s0       = x/(float)width;
        glyph->t0       = y/(float)height;
        glyph->s1       = (x + glyph->width)/(float)width;
        glyph->t1       = (y + glyph->height)/(float)height;
        glyph->advance_x = 0;
        glyph->advance_y = 0;
        vector_push_back( self->glyphs, glyph );
        free( glyph );
        return (texture_glyph_t *) vector_back( self->glyphs );
    }

    /* Glyph has not been already loaded */
    if( !buffer)
    {
        buffer = (wchar_t *) calloc( 2, sizeof(wchar_t) );
    }
    buffer[0] = charcode;

    if( texture_font_load_glyphs( self, buffer ) == 0 )
    {
        return (texture_glyph_t *) vector_back( self->glyphs );
    }
    return NULL;
}


