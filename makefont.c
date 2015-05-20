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
#include "opengl.h"
#include "vec234.h"
#include "vector.h"
#include "freetype-gl.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#if defined(__APPLE__)
    #include <Glut/glut.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

// ---------------------------------------------------------------- display ---
void display( void )
{}

// ---------------------------------------------------------------- reshape ---
void reshape(int width, int height)
{}

// --------------------------------------------------------------- keyboard ---
void keyboard( unsigned char key, int x, int y )
{}

// ------------------------------------------------------------- print help ---
void print_help()
{
    fprintf( stderr, "Usage: makefont [--help] --font <font file> "
             "--header <header file> --size <font size> --variable <variable name>\n" );
}

// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    FILE* test;
    size_t i, j;
    int arg;

    wchar_t * font_cache = 
        L" !\"#$%&'()*+,-./0123456789:;<=>?"
        L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
        L"`abcdefghijklmnopqrstuvwxyz{|}~";

    float  font_size   = 0.0;
    const char * font_filename   = NULL;
    const char * header_filename = NULL;
    const char * variable_name   = "font";
    int show_help = 0;

    for ( arg = 1; arg < argc; ++arg )
    {
        if ( 0 == strcmp( "--font", argv[arg] ) || 0 == strcmp( "-f", argv[arg] ) )
        {
            ++arg;

            if ( font_filename )
            {
                fprintf( stderr, "Multiple --font parameters.\n" );
                print_help();
                exit( 1 );
            }

            if ( arg >= argc )
            {
                fprintf( stderr, "No font file given.\n" );
                print_help();
                exit( 1 );
            }

            font_filename = argv[arg];
            continue;
        }

        if ( 0 == strcmp( "--header", argv[arg] ) || 0 == strcmp( "-o", argv[arg] )  )
        {
            ++arg;

            if ( header_filename )
            {
                fprintf( stderr, "Multiple --header parameters.\n" );
                print_help();
                exit( 1 );
            }

            if ( arg >= argc )
            {
                fprintf( stderr, "No header file given.\n" );
                print_help();
                exit( 1 );
            }

            header_filename = argv[arg];
            continue;
        }

        if ( 0 == strcmp( "--help", argv[arg] ) || 0 == strcmp( "-h", argv[arg] ) )
        {
            show_help = 1;
            break;
        }

        if ( 0 == strcmp( "--size", argv[arg] ) || 0 == strcmp( "-s", argv[arg] ) )
        {
            ++arg;

            if ( 0.0 != font_size )
            {
                fprintf( stderr, "Multiple --size parameters.\n" );
                print_help();
                exit( 1 );
            }

            if ( arg >= argc )
            {
                fprintf( stderr, "No font size given.\n" );
                print_help();
                exit( 1 );
            }

            errno = 0;

            font_size = atof( argv[arg] );

            if ( errno )
            {
                fprintf( stderr, "No valid font size given.\n" );
                print_help();
                exit( 1 );
            }

            continue;
        }

        if ( 0 == strcmp( "--variable", argv[arg] ) || 0 == strcmp( "-arg", argv[arg] )  )
        {
            ++arg;

            if ( 0 != strcmp( "font", variable_name ) )
            {
                fprintf( stderr, "Multiple --variable parameters.\n" );
                print_help();
                exit( 1 );
            }

            if ( arg >= argc )
            {
                fprintf( stderr, "No variable name given.\n" );
                print_help();
                exit( 1 );
            }

            variable_name = argv[arg];
            continue;
        }

        fprintf( stderr, "Unknown parameter %s\n", argv[arg] );
        print_help();
        exit( 1 );
    }

    if ( show_help )
    {
        print_help();
        exit( 1 );
    }

    if ( !font_filename )
    {
        fprintf( stderr, "No font file given.\n" );
        print_help();
        exit( 1 );
    }

    if ( !( test = fopen( font_filename, "r" ) ) )
    {
        fprintf( stderr, "Font file \"%s\" does not exist.\n", font_filename );
    }

    fclose( test );

    if ( 4.0 > font_size )
    {
        fprintf( stderr, "Font size too small, expected at least 4 pt.\n" );
        print_help();
        exit( 1 );
    }

    if ( !header_filename )
    {
        fprintf( stderr, "No header file given.\n" );
        print_help();
        exit( 1 );
    }

    texture_atlas_t * atlas = texture_atlas_new( 128, 128, 1 );
    texture_font_t  * font  = texture_font_new_from_file( atlas, font_size, font_filename );

    glutInit( &argc, argv );
    glutInitWindowSize( atlas->width, atlas->height );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

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
    wprintf( L"Variable name              : %s\n", variable_name );


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
        L"#ifdef __cplusplus\n"
        L"extern \"C\" {\n"
        L"#endif\n"
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


    
    fwprintf( file, L"texture_font_t %s = {\n", variable_name );


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
    fwprintf( file, L" %ff, %ff, %ff, %ff, %ff, %d, \n", 
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
        wprintf( L"  advance    : %ff, %ff\n",
                 glyph->advance_x, glyph->advance_y );
        wprintf( L"  tex coords.: %ff, %ff, %ff, %ff\n", 
                 glyph->u0, glyph->v0, glyph->u1, glyph->v1 );

        wprintf( L"  kerning    : " );
        if( glyph->kerning_count )
        {
            for( j=0; j < glyph->kerning_count; ++j )
            {
                wprintf( L"('%lc', %ff)", 
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
        else if( glyph->charcode == (wchar_t)(-1) )
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
        fwprintf( file, L"%ff, %ff, ", glyph->advance_x, glyph->advance_y );
        fwprintf( file, L"%ff, %ff, %ff, %ff, ", glyph->s0, glyph->t0, glyph->s1, glyph->t1 );
        fwprintf( file, L"%d, ", vector_size(glyph->kerning) );
        fwprintf( file, L"{ " );
        for( j=0; j < vector_size(glyph->kerning); ++j )
        {
            kerning_t *kerning = (kerning_t *) vector_get( glyph->kerning, j);
            wchar_t charcode = kerning->charcode;

            if( (charcode == L'\'' ) || (charcode == L'\\') )
            {
                fwprintf( file, L"{L'\\%lc', %ff}", charcode, kerning->kerning );
            }
            else if( (charcode != (wchar_t)(-1) ) )
            {
                fwprintf( file, L"{L'%lc', %ff}", charcode, kerning->kerning );
            }
            if( j < (vector_size(glyph->kerning)-1))
            {
                fwprintf( file, L", " );
            }
        }
        fwprintf( file, L"} },\n" );
    }
    fwprintf( file, L" }\n};\n" );

    fwprintf( file,
        L"#ifdef __cplusplus\n"
        L"}\n"
        L"#endif\n" );

    return 0;
}
