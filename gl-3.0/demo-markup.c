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
 *
 * Example demonstrating markup usage
 *
 * ============================================================================
 */
#include <fontconfig/fontconfig.h>
#include "freetype-gl.h"

#include "font-manager.h"
#include "vertex-buffer.h"
#include "text-buffer.h"
#include "markup.h"
#include "shader.h"



// ------------------------------------------------------- global variables ---
text_buffer_t * buffer;


// ------------------------------------------------------ match_description ---
char *
match_description( char * family, float size, int bold, int italic )
{

#if defined _WIN32 || defined _WIN64
    fprintf( stderr, "\"font_manager_match_description\" "
                     "not implemented for windows.\n" );
    return 0;
#endif

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


// ---------------------------------------------------------------- display ---
void display( void )
{
    glClearColor(0.40,0.40,0.45,1.00);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    text_buffer_render( buffer );
    glutSwapBuffers( );
}


// --------------------------------------------------------------- reshape ---
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
        exit( EXIT_SUCCESS );
    }
}



// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitWindowSize( 500, 220 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( argv[0] );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    buffer = text_buffer_new( LCD_FILTERING_ON );

    vec4 black  = {{0.0, 0.0, 0.0, 1.0}};
    vec4 white  = {{1.0, 1.0, 1.0, 1.0}};
    vec4 yellow = {{1.0, 1.0, 0.0, 1.0}};
    vec4 grey   = {{0.5, 0.5, 0.5, 1.0}};
    vec4 none   = {{1.0, 1.0, 1.0, 0.0}};

    char *f_normal   = match_description("Droid Serif", 24, 0, 0);
    char *f_bold     = match_description("Droid Serif", 24, 1, 0);
    char *f_italic   = match_description("Droid Serif", 24, 0, 1);
    char *f_japanese = match_description("Droid Sans Japanese", 18, 0, 0);
    char *f_math     = match_description("DejaVu Sans", 24, 0, 0);

    markup_t normal = {
        .family  = f_normal,
        .size    = 24.0, .bold    = 0,   .italic  = 0,
        .rise    = 0.0,  .spacing = 0.0, .gamma   = 1.5,
        .foreground_color    = white, .background_color    = none,
        .underline           = 0,     .underline_color     = white,
        .overline            = 0,     .overline_color      = white,
        .strikethrough       = 0,     .strikethrough_color = white,
        .font = 0,
    };
    markup_t highlight = normal; highlight.background_color = grey;
    markup_t reverse   = normal; reverse.foreground_color = black;
                                 reverse.background_color = white;
                                 reverse.gamma = 1.0;
    markup_t overline  = normal; overline.overline = 1;
    markup_t underline = normal; underline.underline = 1;
    markup_t small     = normal; small.size = 10.0;
    markup_t big       = normal; big.size = 48.0;
                                 big.italic = 1; 
                                 big.foreground_color = yellow;
    markup_t bold      = normal; bold.bold = 1; bold.family = f_bold;
    markup_t italic    = normal; italic.italic = 1; italic.family = f_italic;
    markup_t japanese  = normal; japanese.family = f_japanese;
                                 japanese.size = 18.0;
    markup_t math      = normal; math.family = f_math;


    vec2 pen = {{20, 200}};
    text_buffer_printf( buffer, &pen,
                        &underline, L"The",
                        &normal,    L" Quick",
                        &big,       L" brown ",
                        &reverse,   L" fox \n",
                        &italic,    L"jumps over ",
                        &bold,      L"the lazy ",
                        &normal,    L"dog.\n",
                        &small,     L"Now is the time for all good men "
                                    L"to come to the aid of the party.\n",
                        &italic,    L"Ég get etið gler án þess að meiða mig.\n",
                        &japanese,  L"私はガラスを食べられます。 それは私を傷つけません\n",
                        &math,      L"ℕ ⊆ ℤ ⊂ ℚ ⊂ ℝ ⊂ ℂ", NULL );

    glutMainLoop( );
    return 0;
}
