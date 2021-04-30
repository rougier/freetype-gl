/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
// #include FT_ADVANCES_H
#include FT_LCD_FILTER_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "distance-field.h"
#include "texture-font.h"
#include "platform.h"
#include "utf8-utils.h"
#include "ftgl-utils.h"

#define HRES  64
#define HRESf 64.f
#define DPI   72

static float convert_F26Dot6_to_float(FT_F26Dot6 value)
{
  return ((float)value) / 64.0;
}
static FT_F26Dot6 convert_float_to_F26Dot6(float value)
{
  return (FT_F26Dot6) (value * 64.0);
}

#undef FTERRORS_H_
#define FT_ERROR_START_LIST     switch ( error_code ) {
#define FT_ERRORDEF( e, v, s )    case v: return s;
#define FT_ERROR_END_LIST       }
// Same signature as the function defined in fterrors.h:
// https://www.freetype.org/freetype2/docs/reference/ft2-error_enumerations.html#FTGL_Error_String
const char* FTGL_Error_String( FT_Error error_code )
{
#include FT_ERRORS_H
    return "INVALID ERROR CODE";
}

// ------------------------------------------------- texture_font_load_face ---
static int
texture_font_load_face(texture_font_t *self, float size,
        FT_Library *library, FT_Face *face)
{
    FT_Error error;
    FT_Matrix matrix = {
        (int)((1.0/HRES) * 0x10000L),
        (int)((0.0)      * 0x10000L),
        (int)((0.0)      * 0x10000L),
        (int)((1.0)      * 0x10000L)};

    assert(library);
    assert(size);

    /* Initialize library */
    error = FT_Init_FreeType(library);
    if( error )
    {
        log_error( "FT_Error (line %d, 0x%02x) : %s\n",
                 __LINE__, error, FTGL_Error_String(error) );
        goto cleanup;
    }

    /* Load face */
    switch (self->location) {
    case TEXTURE_FONT_FILE:
        error = FT_New_Face(*library, self->filename, 0, face);
        break;

    case TEXTURE_FONT_MEMORY:
        error = FT_New_Memory_Face(*library,
            self->memory.base, self->memory.size, 0, face);
        break;
    }

    if( error )
    {
        log_error( "FT_Error (line %d, code 0x%02x) : %s\n",
                 __LINE__, error, FTGL_Error_String(error) );
        goto cleanup_library;
    }

    /* Select charmap */
    error = FT_Select_Charmap(*face, FT_ENCODING_UNICODE);
    if( error )
    {
        log_error( "FT_Error (line %d, code 0x%02x) : %s\n",
                 __LINE__, error, FTGL_Error_String(error) );
        goto cleanup_face;
    }

    /* Set char size */
    /* See page 24 of “Higher Quality 2D Text Rendering”:
     * http://jcgt.org/published/0002/01/04/
     * “To render high-quality text, Shemarev [2007] recommends using only
     *  vertical hinting and completely discarding the horizontal hints.
     *  Hinting is the responsibility of the rasterization engine (FreeType in
     *  our case) which provides no option to specifically discard horizontal
     *  hinting. In the case of the FreeType library, we can nonetheless trick
     *  the engine by specifying an oversized horizontal DPI (100 times the
     *  vertical) while specifying a transformation matrix that scale down the
     *  glyph as shown in Listing 1.”
     * That horizontal DPI factor is HRES here. */
    error = FT_Set_Char_Size(*face, convert_float_to_F26Dot6(size), 0, DPI * HRES, DPI);

    if( error )
    {
        log_error( "FT_Error (line %d, code 0x%02x) : %s\n",
                 __LINE__, error, FTGL_Error_String(error) );
        goto cleanup_face;
    }

    /* Set transform matrix */
    FT_Set_Transform(*face, &matrix, NULL);

    return 1;

cleanup_face:
    FT_Done_Face( *face );
cleanup_library:
    FT_Done_FreeType( *library );
cleanup:
    return 0;
}

// ------------------------------------------------------ texture_glyph_new ---
texture_glyph_t *
texture_glyph_new(void)
{
    texture_glyph_t *self = (texture_glyph_t *) malloc( sizeof(texture_glyph_t) );
    if(self == NULL) {
        log_error(
                "line %d: No more memory for allocating data\n", __LINE__);
        return NULL;
    }

    self->codepoint  = -1;
    self->width     = 0;
    self->height    = 0;
    self->rendermode = RENDER_NORMAL;
    self->outline_thickness = 0.0;
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
                           const char * codepoint )
{
    size_t i;
    uint32_t ucodepoint = utf8_to_utf32( codepoint );

    assert( self );
    for( i=0; i<vector_size(self->kerning); ++i )
    {
        kerning_t * kerning = (kerning_t *) vector_get( self->kerning, i );
        if( kerning->codepoint == ucodepoint )
        {
            return kerning->kerning;
        }
    }
    return 0;
}


// ------------------------------------------ texture_font_generate_kerning ---
void
texture_font_generate_kerning( texture_font_t *self,
                               FT_Library *library, FT_Face *face )
{
    size_t i, j;
    FT_UInt glyph_index, prev_index;
    texture_glyph_t *glyph, *prev_glyph;
    FT_Vector kerning;

    assert( self );

    /* For each glyph couple combination, check if kerning is necessary */
    /* Starts at index 1 since 0 is for the special backgroudn glyph */
    for( i=1; i<self->glyphs->size; ++i )
    {
        glyph = *(texture_glyph_t **) vector_get( self->glyphs, i );
        glyph_index = FT_Get_Char_Index( *face, glyph->codepoint );
        vector_clear( glyph->kerning );

        for( j=1; j<self->glyphs->size; ++j )
        {
            prev_glyph = *(texture_glyph_t **) vector_get( self->glyphs, j );
            prev_index = FT_Get_Char_Index( *face, prev_glyph->codepoint );
            // FT_KERNING_UNFITTED returns FT_F26Dot6 values.
            FT_Get_Kerning( *face, prev_index, glyph_index, FT_KERNING_UNFITTED, &kerning );
            // printf("%c(%d)-%c(%d): %ld\n",
            //       prev_glyph->codepoint, prev_glyph->codepoint,
            //       glyph_index, glyph_index, kerning.x);
            if( kerning.x )
            {
                kerning_t k = {
                  prev_glyph->codepoint,
                  convert_F26Dot6_to_float(kerning.x) / HRESf
                };
                vector_push_back( glyph->kerning, &k );
            }
        }
    }
}

// ------------------------------------------------------ texture_font_init ---
static int
texture_font_init(texture_font_t *self)
{
    FT_Library library;
    FT_Face face;
    FT_Size_Metrics metrics;

    assert(self->atlas);
    assert(self->size > 0);
    assert((self->location == TEXTURE_FONT_FILE && self->filename)
        || (self->location == TEXTURE_FONT_MEMORY
            && self->memory.base && self->memory.size));

    self->glyphs = vector_new(sizeof(texture_glyph_t *));
    self->height = 0;
    self->ascender = 0;
    self->descender = 0;
    self->rendermode = RENDER_NORMAL;
    self->outline_thickness = 0.0;
    self->hinting = 1;
    self->kerning = 1;
    self->filtering = 1;

    // FT_LCD_FILTER_LIGHT   is (0x00, 0x55, 0x56, 0x55, 0x00)
    // FT_LCD_FILTER_DEFAULT is (0x10, 0x40, 0x70, 0x40, 0x10)
    self->lcd_weights[0] = 0x10;
    self->lcd_weights[1] = 0x40;
    self->lcd_weights[2] = 0x70;
    self->lcd_weights[3] = 0x40;
    self->lcd_weights[4] = 0x10;

    if (!texture_font_load_face(self, self->size, &library, &face))
        return -1;

    self->underline_position = face->underline_position / (float)(HRESf*HRESf) * self->size;
    self->underline_position = roundf( self->underline_position );
    if( self->underline_position > -2 )
    {
        self->underline_position = -2.0;
    }

    self->underline_thickness = face->underline_thickness / (float)(HRESf*HRESf) * self->size;
    self->underline_thickness = roundf( self->underline_thickness );
    if( self->underline_thickness < 1 )
    {
        self->underline_thickness = 1.0;
    }

    metrics = face->size->metrics;
    self->ascender  = metrics.ascender  >> 6;
    self->descender = metrics.descender >> 6;
    self->height    = metrics.height    >> 6;
    self->linegap = self->height - self->ascender + self->descender;
    FT_Done_Face( face );
    FT_Done_FreeType( library );

    /* NULL is a special glyph */
    texture_font_get_glyph( self, NULL );

    return 0;
}

// --------------------------------------------- texture_font_new_from_file ---
texture_font_t *
texture_font_new_from_file(texture_atlas_t *atlas, const float pt_size,
        const char *filename)
{
    texture_font_t *self;

    assert(filename);

    self = calloc(1, sizeof(*self));
    if (!self) {
        log_error(
                "line %d: No more memory for allocating data\n", __LINE__);
        return NULL;
    }

    self->atlas = atlas;
    self->size  = pt_size;

    self->location = TEXTURE_FONT_FILE;
    self->filename = strdup(filename);

    if (texture_font_init(self)) {
        texture_font_delete(self);
        return NULL;
    }

    return self;
}

// ------------------------------------------- texture_font_new_from_memory ---
texture_font_t *
texture_font_new_from_memory(texture_atlas_t *atlas, float pt_size,
        const void *memory_base, size_t memory_size)
{
    texture_font_t *self;

    assert(memory_base);
    assert(memory_size);

    self = calloc(1, sizeof(*self));
    if (!self) {
        log_error(
                "line %d: No more memory for allocating data\n", __LINE__);
        return NULL;
    }

    self->atlas = atlas;
    self->size  = pt_size;

    self->location = TEXTURE_FONT_MEMORY;
    self->memory.base = memory_base;
    self->memory.size = memory_size;

    if (texture_font_init(self)) {
        texture_font_delete(self);
        return NULL;
    }

    return self;
}

// ---------------------------------------------------- texture_font_delete ---
void
texture_font_delete( texture_font_t *self )
{
    size_t i;
    texture_glyph_t *glyph;

    assert( self );

    if(self->location == TEXTURE_FONT_FILE && self->filename)
        free( self->filename );

    for( i=0; i<vector_size( self->glyphs ); ++i)
    {
        glyph = *(texture_glyph_t **) vector_get( self->glyphs, i );
        texture_glyph_delete( glyph);
    }

    vector_delete( self->glyphs );
    free( self );
}

texture_glyph_t *
texture_font_find_glyph( texture_font_t * self,
                         const char * codepoint )
{
    size_t i;
    texture_glyph_t *glyph;
    uint32_t ucodepoint = utf8_to_utf32( codepoint );

    for( i = 0; i < self->glyphs->size; ++i )
    {
        glyph = *(texture_glyph_t **) vector_get( self->glyphs, i );
        // If codepoint is -1, we don't care about outline type or thickness
        if( (glyph->codepoint == ucodepoint) &&
            ((ucodepoint == -1) ||
             ((glyph->rendermode == self->rendermode) &&
              (glyph->outline_thickness == self->outline_thickness)) ))
        {
            return glyph;
        }
    }

    return NULL;
}

// ------------------------------------------------ texture_font_load_glyph ---
int
texture_font_load_glyph( texture_font_t * self,
                         const char * codepoint )
{
    size_t i, x, y;

    FT_Library library;
    FT_Error error;
    FT_Face face;
    FT_Glyph ft_glyph;
    FT_GlyphSlot slot;
    FT_Bitmap ft_bitmap;

    FT_UInt glyph_index;
    texture_glyph_t *glyph;
    FT_Int32 flags = 0;
    int ft_glyph_top = 0;
    int ft_glyph_left = 0;

    ivec4 region;
    size_t missed = 0;


    if (!texture_font_load_face(self, self->size, &library, &face))
        return 0;

    /* Check if codepoint has been already loaded */
    if (texture_font_find_glyph(self, codepoint)) {
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return 1;
    }

    /* codepoint NULL is special : it is used for line drawing (overline,
     * underline, strikethrough) and background.
     */
    if( !codepoint )
    {
        ivec4 region = texture_atlas_get_region( self->atlas, 5, 5 );
        texture_glyph_t * glyph = texture_glyph_new( );
        static unsigned char data[4*4*3] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                                            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                                            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                                            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
        if ( region.x < 0 )
        {
            log_error( "Texture atlas is full (line %d)\n",  __LINE__ );
            FT_Done_Face( face );
            FT_Done_FreeType( library );
            return 0;
        }
        texture_atlas_set_region( self->atlas, region.x, region.y, 4, 4, data, 0 );
        glyph->codepoint = -1;
        glyph->s0 = (region.x+2)/(float)self->atlas->width;
        glyph->t0 = (region.y+2)/(float)self->atlas->height;
        glyph->s1 = (region.x+3)/(float)self->atlas->width;
        glyph->t1 = (region.y+3)/(float)self->atlas->height;
        vector_push_back( self->glyphs, &glyph );

        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return 1;
    }

    flags = 0;
    ft_glyph_top = 0;
    ft_glyph_left = 0;
    glyph_index = FT_Get_Char_Index( face, (FT_ULong)utf8_to_utf32( codepoint ) );
    // WARNING: We use texture-atlas depth to guess if user wants
    //          LCD subpixel rendering

    if( self->rendermode != RENDER_NORMAL && self->rendermode != RENDER_SIGNED_DISTANCE_FIELD )
    {
        flags |= FT_LOAD_NO_BITMAP;
    }
    else
    {
        flags |= FT_LOAD_RENDER;
    }

    if( !self->hinting )
    {
        flags |= FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;
    }
    else
    {
        flags |= FT_LOAD_FORCE_AUTOHINT;
    }

    if( self->atlas->depth == 3 )
    {
        FT_Library_SetLcdFilter( library, FT_LCD_FILTER_LIGHT );
        flags |= FT_LOAD_TARGET_LCD;

        if( self->filtering )
        {
            FT_Library_SetLcdFilterWeights( library, self->lcd_weights );
        }
    }
    else if (HRES == 1)
    {
        /* “FT_LOAD_TARGET_LIGHT
         *  A lighter hinting algorithm for gray-level modes. Many generated
         *  glyphs are fuzzier but better resemble their original shape.
         *  This is achieved by snapping glyphs to the pixel grid
         *  only vertically (Y-axis), as is done by FreeType's new CFF engine
         *  or Microsoft's ClearType font renderer.”
         * https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_load_target_xxx
         */
        flags |= FT_LOAD_TARGET_LIGHT;
    }

    error = FT_Load_Glyph( face, glyph_index, flags );
    if( error )
    {
        log_error( "FT_Error (line %d, code 0x%02x) : %s\n",
                 __LINE__, error, FTGL_Error_String(error) );
        FT_Done_Face( face );
        FT_Done_FreeType( library );
        return 0;
    }

    if( self->rendermode == RENDER_NORMAL || self->rendermode == RENDER_SIGNED_DISTANCE_FIELD )
    {
        slot            = face->glyph;
        ft_bitmap       = slot->bitmap;
        ft_glyph_top    = slot->bitmap_top;
        ft_glyph_left   = slot->bitmap_left;
    }
    else
    {
        FT_Stroker stroker;
        FT_BitmapGlyph ft_bitmap_glyph;

        error = FT_Stroker_New( library, &stroker );

        if( error )
        {
            log_error( "FT_Error (line %d, 0x%02x) : %s\n",
                     __LINE__, error, FTGL_Error_String(error) );
            goto cleanup_stroker;
        }

        FT_Stroker_Set(stroker,
                        (int)(self->outline_thickness * HRES),
                        FT_STROKER_LINECAP_ROUND,
                        FT_STROKER_LINEJOIN_ROUND,
                        0);

        error = FT_Get_Glyph( face->glyph, &ft_glyph);

        if( error )
        {
            log_error( "FT_Error (line %d, 0x%02x) : %s\n",
                     __LINE__, error, FTGL_Error_String(error) );
            goto cleanup_stroker;
        }

        if( self->rendermode == RENDER_OUTLINE_EDGE )
            error = FT_Glyph_Stroke( &ft_glyph, stroker, 1 );
        else if ( self->rendermode == RENDER_OUTLINE_POSITIVE )
            error = FT_Glyph_StrokeBorder( &ft_glyph, stroker, 0, 1 );
        else if ( self->rendermode == RENDER_OUTLINE_NEGATIVE )
            error = FT_Glyph_StrokeBorder( &ft_glyph, stroker, 1, 1 );

        if( error )
        {
            log_error( "FT_Error (line %d, 0x%02x) : %s\n",
                     __LINE__, error, FTGL_Error_String(error) );
            goto cleanup_stroker;
        }

        if( self->atlas->depth == 1 )
            error = FT_Glyph_To_Bitmap( &ft_glyph, FT_RENDER_MODE_NORMAL, 0, 1);
        else
            error = FT_Glyph_To_Bitmap( &ft_glyph, FT_RENDER_MODE_LCD, 0, 1);

        if( error )
        {
            log_error( "FT_Error (line %d, 0x%02x) : %s\n",
                     __LINE__, error, FTGL_Error_String(error) );
            goto cleanup_stroker;
        }

        ft_bitmap_glyph = (FT_BitmapGlyph) ft_glyph;
        ft_bitmap       = ft_bitmap_glyph->bitmap;
        ft_glyph_top    = ft_bitmap_glyph->top;
        ft_glyph_left   = ft_bitmap_glyph->left;

cleanup_stroker:
        FT_Stroker_Done( stroker );

        if( error )
        {
            FT_Done_Face( face );
            FT_Done_FreeType( library );
            return 0;
        }
    }

    struct {
        int left;
        int top;
        int right;
        int bottom;
    } padding = { 0, 0, 1, 1 };

    if( self->rendermode == RENDER_SIGNED_DISTANCE_FIELD )
    {
        padding.top = 1;
        padding.left = 1;
    }

    if( self->padding != 0 )
    {
        padding.top += self->padding;
        padding.left += self->padding;
        padding.right += self->padding;
        padding.bottom += self->padding;
    }

    size_t src_w = ft_bitmap.width/self->atlas->depth;
    size_t src_h = ft_bitmap.rows;

    size_t tgt_w = src_w + padding.left + padding.right;
    size_t tgt_h = src_h + padding.top + padding.bottom;

    region = texture_atlas_get_region( self->atlas, tgt_w, tgt_h );

    if ( region.x < 0 )
    {
        log_error( "Texture atlas is full (line %d)\n",  __LINE__ );
        FT_Done_Face( face );
        FT_Done_FreeType( library );
        return 0;
    }

    x = region.x;
    y = region.y;

    unsigned char *buffer = calloc( tgt_w * tgt_h * self->atlas->depth, sizeof(unsigned char) );

    unsigned char *dst_ptr = buffer + (padding.top * tgt_w + padding.left) * self->atlas->depth;
    unsigned char *src_ptr = ft_bitmap.buffer;
    for( i = 0; i < src_h; i++ )
    {
        //difference between width and pitch: https://www.freetype.org/freetype2/docs/reference/ft2-basic_types.html#FT_Bitmap
        memcpy( dst_ptr, src_ptr, ft_bitmap.width);
        dst_ptr += tgt_w * self->atlas->depth;
        src_ptr += ft_bitmap.pitch;
    }

    if( self->rendermode == RENDER_SIGNED_DISTANCE_FIELD )
    {
        unsigned char *sdf = make_distance_mapb( buffer, tgt_w, tgt_h );
        free( buffer );
        buffer = sdf;
    }

    texture_atlas_set_region( self->atlas, x, y, tgt_w, tgt_h, buffer, tgt_w * self->atlas->depth);

    free( buffer );

    glyph = texture_glyph_new( );
    glyph->codepoint = utf8_to_utf32( codepoint );
    glyph->is_placeholder = glyph_index == 0;
    glyph->width    = tgt_w;
    glyph->height   = tgt_h;
    glyph->rendermode = self->rendermode;
    glyph->outline_thickness = self->outline_thickness;
    glyph->offset_x = ft_glyph_left;
    glyph->offset_y = ft_glyph_top;
    glyph->s0       = x/(float)self->atlas->width;
    glyph->t0       = y/(float)self->atlas->height;
    glyph->s1       = (x + glyph->width)/(float)self->atlas->width;
    glyph->t1       = (y + glyph->height)/(float)self->atlas->height;

    // Discard hinting to get advance
    FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
    slot = face->glyph;
    glyph->advance_x = convert_F26Dot6_to_float(slot->advance.x);
    glyph->advance_y = convert_F26Dot6_to_float(slot->advance.y);

    vector_push_back( self->glyphs, &glyph );

    if( self->rendermode != RENDER_NORMAL && self->rendermode != RENDER_SIGNED_DISTANCE_FIELD )
        FT_Done_Glyph( ft_glyph );

    texture_font_generate_kerning( self, &library, &face );

    FT_Done_Face( face );
    FT_Done_FreeType( library );

    return 1;
}

// ----------------------------------------------- texture_font_load_glyphs ---
size_t
texture_font_load_glyphs( texture_font_t * self,
                          const char * codepoints )
{
    size_t i, c;

    /* Load each glyph */
    for( i = 0; i < strlen(codepoints); i += utf8_surrogate_len(codepoints + i) ) {
        if( !texture_font_load_glyph( self, codepoints + i ) )
            return utf8_strlen( codepoints + i );
    }

    return 0;
}


// ------------------------------------------------- texture_font_get_glyph ---
texture_glyph_t *
texture_font_get_glyph( texture_font_t * self,
                        const char * codepoint )
{
    texture_glyph_t *glyph;

    assert( self );
    assert( self->filename );
    assert( self->atlas );

    /* Check if codepoint has been already loaded */
    if( (glyph = texture_font_find_glyph( self, codepoint )) )
        return glyph;

    /* Glyph has not been already loaded */
    if( texture_font_load_glyph( self, codepoint ) )
        return texture_font_find_glyph( self, codepoint );

    return NULL;
}

// ------------------------------------------------- texture_font_enlarge_atlas ---
void
texture_font_enlarge_atlas( texture_font_t * self, size_t width_new,
                            size_t height_new )
{
    assert(self);
    assert(self->atlas);
    //ensure size increased
    assert(width_new >= self->atlas->width);
    assert(height_new >= self->atlas->height);
    assert(width_new + height_new > self->atlas->width + self->atlas->height);
    texture_atlas_t* ta = self->atlas;
    size_t width_old = ta->width;
    size_t height_old = ta->height;
    //allocate new buffer
    unsigned char* data_old = ta->data;
    ta->data = calloc(1,width_new*height_new * sizeof(char)*ta->depth);
    //update atlas size
    ta->width = width_new;
    ta->height = height_new;
    //add node reflecting the gained space on the right
    if( width_new > width_old )
    {
        ivec3 node;
        node.x = width_old - 1;
        node.y = 1;
        node.z = width_new - width_old;
        vector_push_back(ta->nodes, &node);
    }
    //copy over data from the old buffer, skipping first row and column because of the margin
    size_t pixel_size = sizeof(char) * ta->depth;
    size_t old_row_size = width_old * pixel_size;
    texture_atlas_set_region(ta, 1, 1, width_old - 2, height_old - 2, data_old + old_row_size + pixel_size, old_row_size);
    free(data_old);
    //change uv coordinates of existing glyphs to reflect size change
    float mulw = (float)width_old / width_new;
    float mulh = (float)height_old / height_new;
    size_t i;
    for( i = 0; i < vector_size(self->glyphs); i++ )
    {
        texture_glyph_t* g = *(texture_glyph_t**)vector_get(self->glyphs, i);
        g->s0 *= mulw;
        g->s1 *= mulw;
        g->t0 *= mulh;
        g->t1 *= mulh;
    }
}
