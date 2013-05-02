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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "platform.h"
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
    self->width     = 0;
    self->height    = 0;
    self->offset_x  = 0;
    self->offset_y  = 0;
    self->s0        = 0.0;
    self->t0        = 0.0;
    self->s1        = 0.0;
    self->t1        = 0.0;
    return self;
}


// --------------------------------------------------- texture_glyph_delete ---
void
texture_glyph_delete( texture_glyph_t *self )
{ }


// ------------------------------------------------------- texture_font_new ---
texture_font_t *
texture_font_new( texture_atlas_t *    atlas,
                  const char *         filename,
                  const float          size)
{

    int i;
    texture_font_t *self = (texture_font_t *) malloc( sizeof(texture_font_t) );
    FT_Library library;
    
    assert( filename );
    assert( size );

    if( self == NULL)
    {
        fprintf( stderr,
                 "line %d: No more memory for allocating data\n", __LINE__ );
        exit( EXIT_FAILURE );
    }

    self->glyphs = vector_new( sizeof(texture_glyph_t *) );
    self->atlas = atlas;
    self->height = 0;
    self->ascender = 0;
    self->descender = 0;
    self->filename = strdup( filename );
    self->size = size;
    self->hres = 100;
    self->ft_face = 0;
    self->hb_ft_font = 0;

    /* Initialize freetype library */
    FT_Error error = FT_Init_FreeType( &library );
    if( error )
    {
        fprintf(stderr, "FT_Error (0x%02x) : %s\n",
                FT_Errors[error].code, FT_Errors[error].message);
        return 0;
    }

    /* Load face */
    error = FT_New_Face( library, filename, 0, &self->ft_face );
    if( error )
    {
        fprintf( stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                 __LINE__, FT_Errors[error].code, FT_Errors[error].message);
        return 0;
    }

    size_t hdpi = 72;
    size_t vdpi = 72;
    FT_Library_SetLcdFilter( library, FT_LCD_FILTER_LIGHT );
    FT_Matrix matrix = { (int)((1.0/self->hres) * 0x10000L),
                         (int)((0.0)      * 0x10000L),
                         (int)((0.0)      * 0x10000L),
                         (int)((1.0)      * 0x10000L) };

    /* Set char size */
    FT_Set_Char_Size( self->ft_face, 0, (int)(size*64),(int)(hdpi*self->hres), vdpi );
    if( error )
    {
        //fprintf( stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
        //         __LINE__, FT_Errors[error].code, FT_Errors[error].message );
        FT_Done_Face( self->ft_face );
        return 0;
    }

    /* Set charmap
     *
     * See http://www.microsoft.com/typography/otspec/name.htm for a list of
     * some possible platform-encoding pairs.  We're interested in 0-3 aka 3-1
     * - UCS-2.  Otherwise, fail. If a font has some unicode map, but lacks
     * UCS-2 - it is a broken or irrelevant font. What exactly Freetype will
     * select on face load (it promises most wide unicode, and if that will be
     * slower that UCS-2 - left as an excercise to check.
     */
    for( i=0; i < self->ft_face->num_charmaps; i++ )
    {
        FT_UShort pid = self->ft_face->charmaps[i]->platform_id;
        FT_UShort eid = self->ft_face->charmaps[i]->encoding_id;
        if( ((pid == 0) && (eid == 3)) || ((pid == 3) && (eid == 1)) )
        {
            error = FT_Set_Charmap( self->ft_face, self->ft_face->charmaps[i] );
            if( error )
            {
                fprintf( stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                         __LINE__, FT_Errors[error].code, FT_Errors[error].message );
            }
        }
    }
    /* Set transform matrix */
    FT_Set_Transform( self->ft_face, &matrix, NULL );
    
    /* Set harfbuzz font */
    self->hb_ft_font = hb_ft_font_create( self->ft_face, NULL );

    return self;
 }


// ---------------------------------------------------- texture_font_delete ---
void
texture_font_delete( texture_font_t *self )
{
    size_t i;
    texture_glyph_t *glyph;

    assert( self );

    if( self->filename )
    {
        free( self->filename );
    }


    for( i=0; i<vector_size( self->glyphs ); ++i)
    {
        glyph = *(texture_glyph_t **) vector_get( self->glyphs, i );
        texture_glyph_delete( glyph);
    }

    vector_delete( self->glyphs );

    FT_Done_Face( self->ft_face );
    hb_font_destroy( self->hb_ft_font );

    free( self );
 }


// ----------------------------------------------- texture_font_load_glyphs ---
void
texture_font_load_glyphs( texture_font_t * self,
                          const char *text,
                          const hb_direction_t directions,
                          const char *language,
                          const hb_script_t script )
{
    int i, j;

    /* Create a buffer for harfbuzz to use */
    hb_buffer_t *buffer = hb_buffer_create();

    hb_buffer_set_direction( buffer, directions );
    hb_buffer_set_script( buffer, script );
    hb_buffer_set_language( buffer,
                            hb_language_from_string(language, strlen(language)) );

    /* Layout the text */
    hb_buffer_add_utf8( buffer, text, strlen(text), 0, strlen(text) );
    hb_shape( self->hb_ft_font, buffer, NULL, 0 );

    unsigned int         glyph_count;
    hb_glyph_info_t     *glyph_info = hb_buffer_get_glyph_infos(buffer, &glyph_count);
    //hb_glyph_position_t *glyph_pos  = hb_buffer_get_glyph_positions(buffer, &glyph_count);

    FT_Error error;
    FT_GlyphSlot slot;
    FT_Bitmap ft_bitmap;
    FT_Int32 flags =  FT_LOAD_RENDER | FT_LOAD_TARGET_LCD;
    for (i = 0; i < glyph_count; ++i)
    {

        int skip = 0;
        for( j=0; j<self->glyphs->size; ++j )
        {
            texture_glyph_t * glyph = *(texture_glyph_t **) vector_get( self->glyphs, j );
            if( glyph->codepoint == glyph_info[i].codepoint ) 
            {
                skip = 1;
                break;
            }
        }
        if( skip ) continue;


        /* Load glyph */
        error = FT_Load_Glyph( self->ft_face, glyph_info[i].codepoint, flags );
        if( error )
        {
            //fprintf( stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
            //         __LINE__, FT_Errors[error].code, FT_Errors[error].message );
            break;
        }

        slot = self->ft_face->glyph;
        ft_bitmap = slot->bitmap;
        int ft_bitmap_width = slot->bitmap.width;
        int ft_bitmap_rows  = slot->bitmap.rows;
        // int ft_bitmap_pitch = slot->bitmap.pitch;
        int ft_glyph_top    = slot->bitmap_top;
        int ft_glyph_left   = slot->bitmap_left;

        int w = ft_bitmap_width/3; // 3 because of LCD/RGB encoding
        int h = ft_bitmap_rows;
        ivec4 region = texture_atlas_get_region( self->atlas, w+1, h+1 );
        if ( region.x < 0 )
        {
            fprintf( stderr, "Texture atlas is full (line %d)\n",  __LINE__ );
            continue;
        }
        int x = region.x, y = region.y;
        texture_atlas_set_region( self->atlas, region.x, region.y,
                                  w, h, ft_bitmap.buffer, ft_bitmap.pitch );


        texture_glyph_t *glyph = texture_glyph_new( );
        glyph->codepoint = glyph_info[i].codepoint;
        glyph->width    = w;
        glyph->height   = h;
        glyph->offset_x = ft_glyph_left;
        glyph->offset_y = ft_glyph_top;
        glyph->s0       = x/(float)self->atlas->width;
        glyph->t0       = y/(float)self->atlas->height;
        glyph->s1       = (x + w)/(float)self->atlas->width;
        glyph->t1       = (y + h)/(float)self->atlas->height;
        vector_push_back( self->glyphs, &glyph );
    }

    /* clean up the buffer, but don't kill it just yet */
    hb_buffer_reset(buffer);

    /* Cleanup */
    hb_buffer_destroy( buffer );
}


// ------------------------------------------------- texture_font_get_glyph ---
texture_glyph_t *
texture_font_get_glyph( texture_font_t * self,
                        size_t codepoint )
{
    size_t i;
    texture_glyph_t *glyph;

    assert( self );
    assert( self->filename );
    assert( self->atlas );

    /* Check if charcode has been already loaded */
    for( i=0; i<self->glyphs->size; ++i )
    {
        glyph = *(texture_glyph_t **) vector_get( self->glyphs, i );
        if( glyph->codepoint == codepoint ) 
        {
            return glyph;
        }
    }
    return NULL;
}


