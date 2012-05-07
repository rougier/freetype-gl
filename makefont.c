/* ============================================================================
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
#include "freetype-gl.h"

// ---------------------------------------------------------------- display ---
void display( void )
{
    int viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    GLuint width  = viewport[2];
    GLuint height = viewport[3];

    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_TEXTURE_2D );
    glColor4f(0,0,0,1);
    glBegin(GL_QUADS);
    glTexCoord2f( 0, 1 ); glVertex2i( 0, 0 );
    glTexCoord2f( 0, 0 ); glVertex2i( 0, height );
    glTexCoord2f( 1, 0 ); glVertex2i( width, height );
    glTexCoord2f( 1, 1 ); glVertex2i( width, 0 );
    glEnd();
    glutSwapBuffers( );
}


// ---------------------------------------------------------------- reshape ---
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
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

    wchar_t * font_cache = 
        L" !\"#$%&'()*+,-./0123456789:;<=>?"
        L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
        L"`abcdefghijklmnopqrstuvwxyz{|}~";

    float  font_size   = 16.0;
    char * font_filename   = "fonts/Arial.ttf";
    char * header_filename = "arial-16.h";

    texture_atlas_t * atlas = texture_atlas_new( 128, 128, 1 );
    texture_font_t  * font  = texture_font_new( atlas, font_filename, font_size );
    

    glutInit( &argc, argv );
    glutInitWindowSize( atlas->width, atlas->height );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glBindTexture( GL_TEXTURE_2D, atlas->id );

    size_t missed = texture_font_load_glyphs( font, font_cache );

    wprintf( L"Font filename              : %s\n", font_filename );
    wprintf( L"Font size                  : %.1f\n", font_size );
    wprintf( L"Number of glyphs           : %ld\n", wcslen(font_cache) );
    wprintf( L"Number of missed glyphs    : %ld\n", missed );
    wprintf( L"Texture size               : %ldx%ldx%ld\n",
             atlas->width, atlas->height, atlas->depth );
    wprintf( L"Texture occupancy          : %.2f%%\n", 
            100.0*atlas->used/(float)(atlas->width*atlas->height) );
    wprintf( L"\n" );
    wprintf( L"Header filename            : %s\n", header_filename );


    size_t texture_size = atlas->width * atlas->height *atlas->depth;
    size_t glyph_count = font->glyphs->size;
    size_t max_kerning_count = 1;
    for( i=0; i < glyph_count; ++i )
    {
        texture_glyph_t *glyph = *(texture_glyph_t **) vector_get( font->glyphs, i );

        if( vector_size(glyph->kerning) > max_kerning_count )
        {
            max_kerning_count = vector_size(glyph->kerning);
        }
    }


    FILE *file = fopen( header_filename, "w" );


    // -------------
    // Header
    // -------------
    fwprintf( file, 
        L"/* ============================================================================\n"
        L" * Freetype GL - A C OpenGL Freetype engine\n"
        L" * Platform:    Any\n"
        L" * WWW:         http://code.google.com/p/freetype-gl/\n"
        L" * ----------------------------------------------------------------------------\n"
        L" * Copyright 2011,2012 Nicolas P. Rougier. All rights reserved.\n"
        L" *\n"
        L" * Redistribution and use in source and binary forms, with or without\n"
        L" * modification, are permitted provided that the following conditions are met:\n"
        L" *\n"
        L" *  1. Redistributions of source code must retain the above copyright notice,\n"
        L" *     this list of conditions and the following disclaimer.\n"
        L" *\n"
        L" *  2. Redistributions in binary form must reproduce the above copyright\n"
        L" *     notice, this list of conditions and the following disclaimer in the\n"
        L" *     documentation and/or other materials provided with the distribution.\n"
        L" *\n"
        L" * THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR\n"
        L" * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n"
        L" * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO\n"
        L" * EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,\n"
        L" * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n"
        L" * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n"
        L" * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n"
        L" * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n"
        L" * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n"
        L" * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"
        L" *\n"
        L" * The views and conclusions contained in the software and documentation are\n"
        L" * those of the authors and should not be interpreted as representing official\n"
        L" * policies, either expressed or implied, of Nicolas P. Rougier.\n"
        L" * ===============================================================================\n"
        L" */\n");


    // ----------------------
    // Structure declarations
    // ----------------------
    fwprintf( file,
        L"#include <stddef.h>\n"
        L"\n"
        L"typedef struct\n"
        L"{\n"
        L"    wchar_t charcode;\n"
        L"    float kerning;\n"
        L"} kerning_t;\n\n" );

    fwprintf( file,
        L"typedef struct\n"
        L"{\n"
        L"    wchar_t charcode;\n"
        L"    int width, height;\n"
        L"    int offset_x, offset_y;\n"
        L"    float advance_x, advance_y;\n"
        L"    float s0, t0, s1, t1;\n"
        L"    size_t kerning_count;\n"
        L"    kerning_t kerning[%d];\n"
        L"} texture_glyph_t;\n\n", max_kerning_count );

    fwprintf( file,
        L"typedef struct\n"
        L"{\n"
        L"    size_t tex_width;\n"
        L"    size_t tex_height;\n"
        L"    size_t tex_depth;\n"
        L"    char tex_data[%d];\n"
        L"    float size;\n"
        L"    float height;\n"
        L"    float linegap;\n"
        L"    float ascender;\n"
        L"    float descender;\n"
        L"    size_t glyphs_count;\n"
        L"    texture_glyph_t glyphs[%d];\n"
        L"} texture_font_t;\n\n", texture_size, glyph_count );


    
    fwprintf( file, L"texture_font_t font = {\n" );


    // ------------
    // Texture data
    // ------------
    fwprintf( file, L" %d, %d, %d, \n", atlas->width, atlas->height, atlas->depth );
    fwprintf( file, L" {" );
    for( i=0; i < texture_size; i+= 32 )
    {
        for( j=0; j < 32 && (j+i) < texture_size ; ++ j)
        {
            if( (j+i) < (texture_size-1) )
            {
                fwprintf( file, L"%d,", atlas->data[i+j] );
            }
            else
            {
                fwprintf( file, L"%d", atlas->data[i+j] );
            }
        }
        if( (j+i) < texture_size )
        {
            fwprintf( file, L"\n  " );
        }
    }
    fwprintf( file, L"}, \n" );


    // -------------------
    // Texture information
    // -------------------
    fwprintf( file, L" %f, %f, %f, %f, %f, %d, \n", 
             font->size, font->height,
             font->linegap,font->ascender, font->descender,
             glyph_count );

    // --------------
    // Texture glyphs
    // --------------
    fwprintf( file, L" {\n" );
    for( i=0; i < glyph_count; ++i )
    {
        texture_glyph_t * glyph = *(texture_glyph_t **) vector_get( font->glyphs, i );

/*
        // Debugging information
        wprintf( L"glyph : '%lc'\n",
                 glyph->charcode );
        wprintf( L"  size       : %dx%d\n",
                 glyph->width, glyph->height );
        wprintf( L"  offset     : %+d%+d\n",
                 glyph->offset_x, glyph->offset_y );
        wprintf( L"  advance    : %f, %f\n",
                 glyph->advance_x, glyph->advance_y );
        wprintf( L"  tex coords.: %f, %f, %f, %f\n", 
                 glyph->u0, glyph->v0, glyph->u1, glyph->v1 );

        wprintf( L"  kerning    : " );
        if( glyph->kerning_count )
        {
            for( j=0; j < glyph->kerning_count; ++j )
            {
                wprintf( L"('%lc', %f)", 
                         glyph->kerning[j].charcode, glyph->kerning[j].kerning );
                if( j < (glyph->kerning_count-1) )
                {
                    wprintf( L", " );
                }
            }
        }
        else
        {
            wprintf( L"None" );
        }
        wprintf( L"\n\n" );
*/


        // TextureFont
        if( (glyph->charcode == L'\'' ) || (glyph->charcode == L'\\' ) )
        {
            fwprintf( file, L"  {L'\\%lc', ", glyph->charcode );
            //wprintf( L"  {L'\\%lc', ", glyph->charcode );
        }
        else if( (glyph->charcode == (wchar_t)(-1) ) )
        {
            fwprintf( file, L"  {L'\\0', " );
            //wprintf( L"  {L'\\0', " );
        }
        else
        {
            fwprintf( file, L"  {L'%lc', ", glyph->charcode );
            //wprintf( L"  {L'%lc', ", glyph->charcode );
        }
        fwprintf( file, L"%d, %d, ", glyph->width, glyph->height );
        fwprintf( file, L"%d, %d, ", glyph->offset_x, glyph->offset_y );
        fwprintf( file, L"%f, %f, ", glyph->advance_x, glyph->advance_y );
        fwprintf( file, L"%f, %f, %f, %f, ", glyph->s0, glyph->t0, glyph->s1, glyph->t1 );
        fwprintf( file, L"%d, ", vector_size(glyph->kerning) );
        fwprintf( file, L"{ " );
        for( j=0; j < vector_size(glyph->kerning); ++j )
        {
            kerning_t *kerning = (kerning_t *) vector_get( glyph->kerning, j);
            wchar_t charcode = kerning->charcode;

            if( (charcode == L'\'' ) || (charcode == L'\\') )
            {
                fwprintf( file, L"{L'\\%lc', %f}", charcode, kerning->kerning );
            }
            else if( (charcode != (wchar_t)(-1) ) )
            {
                fwprintf( file, L"{L'%lc', %f}", charcode, kerning->kerning );
            }
            if( j < (vector_size(glyph->kerning)-1))
            {
                fwprintf( file, L", " );
            }
        }
        fwprintf( file, L"} },\n" );
    }
    fwprintf( file, L" }\n};\n" );

    return 0;
}
