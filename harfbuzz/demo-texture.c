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
 * ============================================================================ */
/* ------------------------------------ */
#include "freetype-gl.h"

#include <assert.h>

#if defined(__APPLE__)
    #include <Glut/glut.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif
#include "mat4.h"
#include "shader.h"
#include "vertex-buffer.h"
#include "texture-font.h"


/* ------------------------------------ */

#define NUM_EXAMPLES 3

/* Tranlations courtesy of google */
const char *texts[NUM_EXAMPLES] = {
    "Ленивый рыжий кот",
    "كسول الزنجبيل القط",
    "懶惰的姜貓",
};

const hb_direction_t text_directions[NUM_EXAMPLES] = {
    HB_DIRECTION_LTR,
    HB_DIRECTION_RTL,
    HB_DIRECTION_TTB,
};

/* XXX: These are not correct, though it doesn't seem to break anything
 *      regardless of their value. */
const char *languages[NUM_EXAMPLES] = {
    "en",
    "ar",
    "ch",
};

const hb_script_t scripts[NUM_EXAMPLES] = {
    HB_SCRIPT_LATIN,
    HB_SCRIPT_ARABIC,
    HB_SCRIPT_HAN,
};

const char *fonts[NUM_EXAMPLES] = {
    "fonts/Liberastika-Regular.ttf",
    "fonts/amiri-regular.ttf",
    "fonts/fireflysung.ttf",
};

enum {
    ENGLISH=0,
    ARABIC,
    CHINESE
};
/* google this */
#ifndef unlikely
#define unlikely
#endif


void ftfdump( FT_Face face )
{
    int i;
    for(i=0; i<face->num_charmaps; i++) {
        printf("%d: %s %s %c%c%c%c plat=%hu id=%hu\n", i,
            face->family_name,
            face->style_name,
            face->charmaps[i]->encoding >>24,
           (face->charmaps[i]->encoding >>16 ) & 0xff,
           (face->charmaps[i]->encoding >>8) & 0xff,
           (face->charmaps[i]->encoding) & 0xff,
            face->charmaps[i]->platform_id,
            face->charmaps[i]->encoding_id
        );
    }
}

/*  See http://www.microsoft.com/typography/otspec/name.htm for a list of some
    possible platform-encoding pairs.  We're interested in 0-3 aka 3-1 - UCS-2.
    Otherwise, fail. If a font has some unicode map, but lacks UCS-2 - it is a
    broken or irrelevant font. What exactly Freetype will select on face load
    (it promises most wide unicode, and if that will be slower that UCS-2 -
    left as an excercise to check. */
int force_ucs2_charmap( FT_Face face )
{
    int i;
    for(i = 0; i < face->num_charmaps; i++)
        if( ((face->charmaps[i]->platform_id == 0) && (face->charmaps[i]->encoding_id == 3))
         || ((face->charmaps[i]->platform_id == 3) && (face->charmaps[i]->encoding_id == 1)) )
            return FT_Set_Charmap( face, face->charmaps[i] );
    return -1;
}


// ------------------------------------------------------- global variables ---
GLuint shader;
vertex_buffer_t * buffer;
mat4 model, view, projection;


// ---------------------------------------------------------------- display ---
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glUseProgram( shader );
    {
        glUniform1i( glGetUniformLocation( shader, "texture" ), 0 );
        glUniform3f( glGetUniformLocation( shader, "pixel" ), 1/512., 1/512., 1.0f );

        glUniformMatrix4fv( glGetUniformLocation( shader, "model" ),
                            1, 0, model.data);
        glUniformMatrix4fv( glGetUniformLocation( shader, "view" ),
                            1, 0, view.data);
        glUniformMatrix4fv( glGetUniformLocation( shader, "projection" ),
                            1, 0, projection.data);
        vertex_buffer_render( buffer, GL_TRIANGLES );
    }

    glutSwapBuffers( );
}


// ---------------------------------------------------------------- reshape ---
void reshape( int width, int height )
{
    glViewport(0, 0, width, height);
    mat4_set_orthographic( &projection, 0, width, 0, height, -1, 1);
}


// --------------------------------------------------------------- keyboard ---
void keyboard( unsigned char key, int x, int y )
{
    if ( key == 27 )
    {
        exit( 1 );
    }
}


// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    size_t i, j;
    int ptSize = 50*64;
    int device_hdpi = 72;
    int device_vdpi = 72;


    glutInit( &argc, argv );
    glutInitWindowSize( 512, 512 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( argv[0] );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf( stderr, "Error: %s\n", glewGetErrorString(err) );
        exit( EXIT_FAILURE );
    }
    fprintf( stderr, "Using GLEW %s\n", glewGetString(GLEW_VERSION) );

    texture_atlas_t * atlas = texture_atlas_new( 512, 512, 3 );


    /* Init freetype */
    FT_Library ft_library;
    assert(!FT_Init_FreeType(&ft_library));

    /* Load our fonts */
    FT_Face ft_face[NUM_EXAMPLES];
    assert(!FT_New_Face( ft_library, fonts[ENGLISH], 0, &ft_face[ENGLISH]) );
    assert(!FT_Set_Char_Size( ft_face[ENGLISH], 0, ptSize, device_hdpi, device_vdpi ) );
    // ftfdump( ft_face[ENGLISH] );            // wonderful world of encodings ...
    force_ucs2_charmap( ft_face[ENGLISH] ); // which we ignore.

    assert( !FT_New_Face(ft_library, fonts[ARABIC], 0, &ft_face[ARABIC]) );
    assert( !FT_Set_Char_Size(ft_face[ARABIC], 0, ptSize, device_hdpi, device_vdpi ) );
    // ftfdump( ft_face[ARABIC] );
    force_ucs2_charmap( ft_face[ARABIC] );

    assert(!FT_New_Face( ft_library, fonts[CHINESE], 0, &ft_face[CHINESE]) );
    assert(!FT_Set_Char_Size( ft_face[CHINESE], 0, ptSize, device_hdpi, device_vdpi ) );
    // ftfdump( ft_face[CHINESE] );
    force_ucs2_charmap( ft_face[CHINESE] );

    /* Get our harfbuzz font structs */
    hb_font_t *hb_ft_font[NUM_EXAMPLES];
    hb_ft_font[ENGLISH] = hb_ft_font_create( ft_face[ENGLISH], NULL );
    hb_ft_font[ARABIC]  = hb_ft_font_create( ft_face[ARABIC] , NULL );
    hb_ft_font[CHINESE] = hb_ft_font_create( ft_face[CHINESE], NULL );

    /* Create a buffer for harfbuzz to use */
    hb_buffer_t *buf = hb_buffer_create();

    for (i=0; i < NUM_EXAMPLES; ++i)
    {
        hb_buffer_set_direction( buf, text_directions[i] ); /* or LTR */
        hb_buffer_set_script( buf, scripts[i] ); /* see hb-unicode.h */
        hb_buffer_set_language( buf,
                                hb_language_from_string(languages[i], strlen(languages[i])) );

        /* Layout the text */
        hb_buffer_add_utf8( buf, texts[i], strlen(texts[i]), 0, strlen(texts[i]) );
        hb_shape( hb_ft_font[i], buf, NULL, 0 );

        unsigned int         glyph_count;
        hb_glyph_info_t     *glyph_info   = hb_buffer_get_glyph_infos(buf, &glyph_count);
        hb_glyph_position_t *glyph_pos    = hb_buffer_get_glyph_positions(buf, &glyph_count);


        FT_GlyphSlot slot;
        FT_Bitmap ft_bitmap;
        float size = 24;
        size_t hres = 64;
        FT_Error error;
        FT_Int32 flags = 0;
        flags |= FT_LOAD_RENDER;
        flags |= FT_LOAD_TARGET_LCD;
        FT_Library_SetLcdFilter( ft_library, FT_LCD_FILTER_LIGHT );
        FT_Matrix matrix = { (int)((1.0/hres) * 0x10000L),
                             (int)((0.0)      * 0x10000L),
                             (int)((0.0)      * 0x10000L),
                             (int)((1.0)      * 0x10000L) };
        /* Set char size */
        error = FT_Set_Char_Size( ft_face[i], (int)(ptSize), 0, 72*hres, 72 );
        if( error )
        {
            //fprintf( stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
            //         __LINE__, FT_Errors[error].code, FT_Errors[error].message );
            FT_Done_Face( ft_face[i] );
            break;
        }

        /* Set transform matrix */
        FT_Set_Transform( ft_face[i], &matrix, NULL );

        for (j = 0; j < glyph_count; ++j)
        {
            /* Load glyph */
            error = FT_Load_Glyph( ft_face[i], glyph_info[j].codepoint, flags );
            if( error )
            {
                //fprintf( stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
                //         __LINE__, FT_Errors[error].code, FT_Errors[error].message );
                FT_Done_Face( ft_face[i] );
                break;
            }

            slot = ft_face[i]->glyph;
            ft_bitmap = slot->bitmap;
            int ft_bitmap_width = slot->bitmap.width;
            int ft_bitmap_rows  = slot->bitmap.rows;
            int ft_bitmap_pitch = slot->bitmap.pitch;
            int ft_glyph_top    = slot->bitmap_top;
            int ft_glyph_left   = slot->bitmap_left;

            int w = ft_bitmap_width/3; // 3 because of LCD/RGB encoding
            int h = ft_bitmap_rows;

            ivec4 region = texture_atlas_get_region( atlas, w+1, h+1 );
            if ( region.x < 0 )
            {
                fprintf( stderr, "Texture atlas is full (line %d)\n",  __LINE__ );
                continue;
            }
            int x = region.x, y = region.y;
            texture_atlas_set_region( atlas, region.x, region.y,
                                      w, h, ft_bitmap.buffer, ft_bitmap.pitch );
            printf("%d: %dx%d %f %f\n",
                   glyph_info[j].codepoint,
                   ft_bitmap_width,
                   ft_bitmap_rows,
                   glyph_pos[j].x_advance/64.,
                   glyph_pos[j].y_advance/64.);
        }

        /* clean up the buffer, but don't kill it just yet */
        hb_buffer_reset(buf);
    }


    /* Cleanup */
    hb_buffer_destroy( buf );
    for( i=0; i < NUM_EXAMPLES; ++i )
        hb_font_destroy( hb_ft_font[i] );
    FT_Done_FreeType( ft_library );

    glClearColor(1,1,1,1);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_TEXTURE_2D );

    glBindTexture( GL_TEXTURE_2D, atlas->id );
    texture_atlas_upload( atlas );

    typedef struct { float x,y,z, u,v, r,g,b,a, shift, gamma; } vertex_t;
    vertex_t vertices[4] =  {
        {  0,  0,0, 0,1, 0,0,0,1, 0, 1},
        {  0,512,0, 0,0, 0,0,0,1, 0, 1},
        {512,512,0, 1,0, 0,0,0,1, 0, 1},
        {512,  0,0, 1,1, 0,0,0,1, 0, 1} };
    GLuint indices[6] = { 0, 1, 2, 0,2,3 };
    buffer = vertex_buffer_new( "vertex:3f,"
                                "tex_coord:2f,"
                                "color:4f,"
                                "ashift:1f,"
                                "agamma:1f" );






    vertex_buffer_push_back( buffer, vertices, 4, indices, 6 );
    shader = shader_load("shaders/text.vert", "shaders/text.frag");
    mat4_set_identity( &projection );
    mat4_set_identity( &model );
    mat4_set_identity( &view );
    glutMainLoop( );
    return 0;
}
