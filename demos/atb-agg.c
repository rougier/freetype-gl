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
#include <math.h>
#include <stdio.h>
#include <AntTweakBar.h>
#include "freetype-gl.h"
#include "vertex-buffer.h"
#include "text-buffer.h"
#include "markup.h"
#include "shader.h"
#include "mat4.h"

#if defined(__APPLE__)
    #include <Glut/glut.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif


// ------------------------------------------------------- typedef & struct ---
typedef enum {
    VERA = 1,
    VERA_MONO,
    LUCKIEST_GUY,
    SOURCE_SANS,
    SOURCE_CODE,
    OLD_STANDARD,
    LOBSTER,
} font_family_e;

#define NUM_FONTS 7


// ------------------------------------------------------- global variables ---
TwBar *bar;

text_buffer_t * buffer;
text_buffer_t * buffer_a;
text_buffer_t * buffer_rgb;

mat4 model, view, projection;

font_family_e p_family;
float p_size;
int p_invert;
int p_kerning;
int p_hinting;
int p_lcd_filtering;
float p_gamma;
float p_interval;
float p_weight;
float p_width;
float p_faux_weight;
float p_faux_italic;
float p_primary;
float p_secondary;
float p_tertiary;

static wchar_t text[] =
    L"A single pixel on a color LCD is made of three colored elements \n"
    L"ordered (on various displays) either as blue, green, and red (BGR), \n"
    L"or as red, green, and blue (RGB). These pixel components, sometimes \n"
    L"called sub-pixels, appear as a single color to the human eye because \n"
    L"of blurring by the optics and spatial integration by nerve cells in "
    L"the eye.\n"
    L"\n"
    L"The resolution at which colored sub-pixels go unnoticed differs, \n"
    L"however, with each user some users are distracted by the colored \n"
    L"\"fringes\" resulting from sub-pixel rendering. Subpixel rendering \n"
    L"is better suited to some display technologies than others. The \n"
    L"technology is well-suited to LCDs, but less so for CRTs. In a CRT \n"
    L"the light from the pixel components often spread across pixels, \n"
    L"and the outputs of adjacent pixels are not perfectly independent."
    L"\n";


// ----------------------------------------------------------- build_buffer ---
void
build_buffer( void )
{
    vec2 pen;
    texture_font_t *font;
    vec4 black  = {{0.0, 0.0, 0.0, 1.0}};
    vec4 white  = {{1.0, 1.0, 1.0, 1.0}};
    vec4 none   = {{1.0, 1.0, 1.0, 0.0}};
    vec4 color = white;
    if( p_invert )
    {
        color = black;
    }

    markup_t markup = {
        .family  = "Source Sans Pro",
        .size    = 10.0,
        .bold    = 0,
        .italic  = 0,
        .rise    = 0.0,
        .spacing = p_interval,
        .gamma   = p_gamma,
        .foreground_color    = color,
        .background_color    = none,
        .underline           = 0,
        .underline_color     = color,
        .overline            = 0,
        .overline_color      = color,
        .strikethrough       = 0,
        .strikethrough_color = color,
        .font = 0,
    };

    text_buffer_clear( buffer );
    texture_atlas_t * atlas = buffer->manager->atlas;
    texture_atlas_clear( atlas );

    if( p_family == VERA)
    {
        font = texture_font_new_from_file( atlas, p_size, "fonts/Vera.ttf" );
    }
    else if( p_family == VERA_MONO)
    {
        font = texture_font_new_from_file( atlas, p_size, "fonts/VeraMono.ttf" );
    }
    else if( p_family == LUCKIEST_GUY)
    {
        font = texture_font_new_from_file( atlas, p_size, "fonts/LuckiestGuy.ttf" );
    }
    else if( p_family == SOURCE_SANS )
    {
        font = texture_font_new_from_file( atlas, p_size, "fonts/SourceSansPro-Regular.ttf" );
    }
    else if( p_family == SOURCE_CODE )
    {
        font = texture_font_new_from_file( atlas, p_size, "fonts/SourceCodePro-Regular.ttf" );
    }
    else if( p_family == OLD_STANDARD )
    {
        font = texture_font_new_from_file( atlas, p_size, "fonts/OldStandard-Regular.ttf" );
    }
    else if( p_family == LOBSTER )
    {
        font = texture_font_new_from_file( atlas, p_size, "fonts/Lobster-Regular.ttf" );
    }
    else
    {
        fprintf( stderr, "Error : Unknown family type\n" );
        return;
    }

	if (!font)
		return;

    markup.font = font;
    font->hinting = p_hinting;
    font->kerning = p_kerning;
    font->filtering = 1;
    float norm = 1.0/(p_primary + 2*p_secondary + 2*p_tertiary);
    font->lcd_weights[0] = (unsigned char)(p_tertiary*norm*256);
    font->lcd_weights[1] = (unsigned char)(p_secondary*norm*256);
    font->lcd_weights[2] = (unsigned char)(p_primary*norm*256);
    font->lcd_weights[3] = (unsigned char)(p_secondary*norm*256);
    font->lcd_weights[4] = (unsigned char)(p_tertiary*norm*256);
    pen.x = 10;
    pen.y = 600 - font->height - 10;
    text_buffer_printf( buffer, &pen, &markup, text, NULL );

    // Post-processing for width and orientation
    vertex_buffer_t * vbuffer = buffer->buffer;
    size_t i;
    for( i=0; i < vector_size( vbuffer->items ); ++i )
    {
        ivec4 *item = (ivec4 *) vector_get( vbuffer->items, i);
        glyph_vertex_t * v0 = /* x0,y0 */
            (glyph_vertex_t *) vector_get( vbuffer->vertices, item->vstart+0 );
        //glyph_vertex_t * v1 = /* x0,y1 */
        //    (glyph_vertex_t *) vector_get( vbuffer->vertices, item->vstart+1 );
        glyph_vertex_t * v2 = /* x1,y1 */
            (glyph_vertex_t *) vector_get( vbuffer->vertices, item->vstart+2 );
        glyph_vertex_t * v3 = /* x1,y0 */
            (glyph_vertex_t *) vector_get( vbuffer->vertices, item->vstart+3 );

        float x0 = v0->x, y0 = v0->y;
        float x1 = v2->x, y1 = v2->y;
        v2->x = v3->x = x0 + (x1-x0)*p_width;

        float dy = abs(y1-y0);
        float dx = tan(p_faux_italic/180.0 * M_PI) * dy;
        v0->x += dx;
        v0->shift = fmod(v0->shift + dx-(int)(dx),1.0);
        v3->x += dx;
        v3->shift = fmod(v3->shift + dx-(int)(dx),1.0);
    }


    texture_font_delete( font );
}



// ---------------------------------------------------------------- display ---
void display(void)
{
    vec4 black  = {{0.0, 0.0, 0.0, 1.0}};
    vec4 white  = {{1.0, 1.0, 1.0, 1.0}};

    if( !p_invert )
    {
        glClearColor( 0, 0, 0, 1 );
        buffer->base_color = white;

    }
    else
    {
        glClearColor( 1, 1, 1, 1 );
        buffer->base_color = black;
    }
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( buffer->shader );
    {
        glUniformMatrix4fv( glGetUniformLocation( buffer->shader, "model" ),
                            1, 0, model.data);
        glUniformMatrix4fv( glGetUniformLocation( buffer->shader, "view" ),
                            1, 0, view.data);
        glUniformMatrix4fv( glGetUniformLocation( buffer->shader, "projection" ),
                            1, 0, projection.data);
        text_buffer_render( buffer );
    }

    TwDraw( );
    glutSwapBuffers( );
}


// ---------------------------------------------------------------- reshape ---
void reshape( int width, int height )
{
    glViewport(0, 0, width, height);
    mat4_set_orthographic( &projection, 0, width, 0, height, -1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    TwWindowSize( width, height );
}


// ------------------------------------------------------------------- quit ---
void reset( void )
{
    p_family    = VERA;
    p_size      = 12.0;
    p_invert    = 0;
    p_kerning   = 1;
    p_hinting   = 1;
    p_lcd_filtering = 1;
    p_gamma     = 1.75;
    p_interval  = 0.0;
    p_weight    = 0.33;
    p_width     = 1.0;
    p_faux_weight = 0.0;
    p_faux_italic = 0.0;

    // FT_LCD_FILTER_LIGHT
    p_primary   = 1.0/3.0;
    p_secondary = 1.0/3.0;
    p_tertiary  = 0.0/3.0;

    // FT_LCD_FILTER_DEFAULT
    // p_primary   = 3.0/9.0;
    // p_secondary = 2.0/9.0;
    // p_tertiary  = 1.0/9.0;

    build_buffer();
}

// ------------------------------------------------------------------- quit ---
void quit( void )
{
    exit( EXIT_SUCCESS );
}

// -------------------------------------------------------------- terminate ---
void terminate( void )
{
    TwTerminate();
}

void timer( int fps )
{
    glutPostRedisplay();
    glutTimerFunc( 1000.0/fps, timer, fps );
}

// --------------------------------------------------------- get/set invert ---
void TW_CALL set_invert( const void *value, void *data )
{
    p_invert = *(const int *) value;
    build_buffer();
}
void TW_CALL get_invert( void *value, void *data )
{
    *(int *)value = p_invert;
}

// -------------------------------------------------------- get/set kerning ---
void TW_CALL set_kerning( const void *value, void *data )
{
    p_kerning = *(const int *) value;
    build_buffer();
}
void TW_CALL get_kerning( void *value, void *data )
{
    *(int *)value = p_kerning;
}

// -------------------------------------------------------- get/set hinting ---
void TW_CALL set_hinting( const void *value, void *data )
{
    p_hinting = *(const int *) value;
    build_buffer();
}
void TW_CALL get_hinting( void *value, void *data )
{
    *(int *)value = p_hinting;
}

// -------------------------------------------------- get/set lcd_filtering ---
void TW_CALL set_lcd_filtering( const void *value, void *data )
{
    p_lcd_filtering = *(const int *) value;
    if( p_lcd_filtering )
    {
        buffer = buffer_rgb;
    }
    else
    {
        buffer = buffer_a;
    }
    build_buffer();
}
void TW_CALL get_lcd_filtering( void *value, void *data )
{
    *(int *)value = p_lcd_filtering;
}

// --------------------------------------------------------- get/set weight ---
void TW_CALL set_weight( const void *value, void *data )
{
    p_weight = *(const float *) value;
}
void TW_CALL get_weight( void *value, void *data )
{
    *(float *)value = p_weight;
}

// ---------------------------------------------------------- get/set gamma ---
void TW_CALL set_gamma( const void *value, void *data )
{
    p_gamma = *(const float *) value;
    build_buffer();
}
void TW_CALL get_gamma( void *value, void *data )
{
   *(float *)value = p_gamma;
}

// ---------------------------------------------------------- get/set width ---
void TW_CALL set_width( const void *value, void *data )
{
    p_width = *(const float *) value;
    build_buffer();
}
void TW_CALL get_width( void *value, void *data )
{
    *(float *)value = p_width;
}

// ------------------------------------------------------- get/set interval ---
void TW_CALL set_interval( const void *value, void *data )
{
    p_interval = *(const float *) value;
    build_buffer();
}
void TW_CALL get_interval( void *value, void *data )
{
    *(float *)value = p_interval;
}

// ---------------------------------------------------- get/set faux_weight ---
void TW_CALL set_faux_weight( const void *value, void *data )
{
    p_faux_weight = *(const float *) value;
}
void TW_CALL get_faux_weight( void *value, void *data )
{
    *(float *)value = p_faux_weight;
}

// ---------------------------------------------------- get/set faux_italic ---
void TW_CALL set_faux_italic( const void *value, void *data )
{
    p_faux_italic = *(const float *) value;
    build_buffer();
}
void TW_CALL get_faux_italic( void *value, void *data )
{
    *(float *)value = p_faux_italic;
}

// ----------------------------------------------------------- get/set size ---
void TW_CALL set_size( const void *value, void *data )
{
    p_size = *(const float *) value;
    build_buffer();

}
void TW_CALL get_size( void *value, void *data )
{
    *(float *)value = p_size;
}

// --------------------------------------------------------- get/set family ---
void TW_CALL set_family( const void *value, void *data )
{
    p_family = *(const font_family_e *) value;
    build_buffer();
}
void TW_CALL get_family( void *value, void *data )
{
    *(font_family_e *)value = p_family;
}

// ----------------------------------------------------------- get/set primary ---
void TW_CALL set_primary( const void *value, void *data )
{
    p_primary = *(const float *) value;
    build_buffer();

}
void TW_CALL get_primary( void *value, void *data )
{
    *(float *)value = p_primary;
}

// ----------------------------------------------------------- get/set secondary ---
void TW_CALL set_secondary( const void *value, void *data )
{
    p_secondary = *(const float *) value;
    build_buffer();

}
void TW_CALL get_secondary( void *value, void *data )
{
    *(float *)value = p_secondary;
}

// ----------------------------------------------------------- get/set tertiary ---
void TW_CALL set_tertiary( const void *value, void *data )
{
    p_tertiary = *(const float *) value;
    build_buffer();

}
void TW_CALL get_tertiary( void *value, void *data )
{
    *(float *)value = p_tertiary;
}


// Main
int main(int argc, char *argv[])
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 800, 600 );
    glutCreateWindow( "Font rendering advanced tweaking" );
    glutCreateMenu( NULL );

#ifndef __APPLE__
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf( stderr, "Error: %s\n", glewGetErrorString(err) );
        exit( EXIT_FAILURE );
    }
    fprintf( stderr, "Using GLEW %s\n", glewGetString(GLEW_VERSION) );
#endif

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    atexit( terminate );
    TwInit( TW_OPENGL, NULL );
    glutMouseFunc( (GLUTmousebuttonfun)TwEventMouseButtonGLUT );
    glutMotionFunc( (GLUTmousemotionfun)TwEventMouseMotionGLUT );
    glutPassiveMotionFunc( (GLUTmousemotionfun)TwEventMouseMotionGLUT );
    glutKeyboardFunc( (GLUTkeyboardfun)TwEventKeyboardGLUT );
    glutSpecialFunc( (GLUTspecialfun)TwEventSpecialGLUT );
    TwGLUTModifiersFunc( glutGetModifiers );

    // Create a new tweak bar
    bar = TwNewBar("TweakBar");
    TwDefine("GLOBAL "
             "help = 'This example shows how to tune all font parameters.' ");
    TwDefine("TweakBar                      "
             "size          = '280 400'     "
             "position      = '500 20'      "
             "color         = '127 127 127' "
             "alpha         = 240           "
             "label         = 'Parameters'  "
             "resizable     = True          "
             "fontresizable = True          "
             "iconifiable   = True          ");

    {
        TwEnumVal familyEV[NUM_FONTS] = {
            {VERA,         "Vera"},
            {VERA_MONO,    "Vera Mono"},
            {LUCKIEST_GUY, "Luckiest Guy"},
            {SOURCE_SANS,  "Source Sans Pro"},
            {SOURCE_CODE,  "Source Code Pro"},
            {OLD_STANDARD, "Old Standard TT"},
            {LOBSTER,      "Lobster"} };
        TwType family_type = TwDefineEnum("Family", familyEV, NUM_FONTS);
        TwAddVarCB(bar, "Family", family_type, set_family, get_family, NULL,
                   "label = 'Family'      "
                   "group = 'Font'        "
                   "help  = ' '           ");
    }
    TwAddVarCB(bar, "Size", TW_TYPE_FLOAT, set_size, get_size, NULL,
               "label = 'Size' "
               "group = 'Font' "
               "min   = 6.0    "
               "max   = 24.0   "
               "step  = 0.05   "
               "help  = ' '    ");
    TwAddVarCB(bar, "LCD filtering", TW_TYPE_BOOL32, set_lcd_filtering, get_lcd_filtering, NULL,
               "label = 'LCD filtering' "
              "group = 'Font'        "
               "help  = ' '             ");


    // Rendering
    TwAddVarCB(bar, "Kerning", TW_TYPE_BOOL32, set_kerning, get_kerning, NULL,
               "label = 'Kerning'   "
               "group = 'Rendering' "
               "help  = ' '         ");
    TwAddVarCB(bar, "Hinting", TW_TYPE_BOOL32, set_hinting, get_hinting, NULL,
               "label = 'Hinting'   "
               "group = 'Rendering' "
               "help  = ' '         ");

    // Color
    TwAddVarCB(bar, "Invert", TW_TYPE_BOOL32, set_invert, get_invert, NULL,
               "label = 'Invert' "
               "group = 'Color'  "
               "help  = ' '      ");

    // Glyph
    TwAddVarCB(bar, "Width", TW_TYPE_FLOAT, set_width, get_width, NULL,
               "label = 'Width' "
               "group = 'Glyph' "
               "min   = 0.75    "
               "max   = 1.25    "
               "step  = 0.01    "
               "help  = ' '     ");

    TwAddVarCB(bar, "Interval", TW_TYPE_FLOAT, set_interval, get_interval, NULL,
               "label = 'Spacing' "
               "group = 'Glyph'   "
               "min   = -0.2      "
               "max   = 0.2       "
               "step  = 0.01      "
               "help  = ' '       " );

    TwAddVarCB(bar, "Faux italic", TW_TYPE_FLOAT, set_faux_italic, get_faux_italic, NULL,
               "label = 'Faux italic' "
               "group = 'Glyph'       "
               "min   = -30.0         "
               "max   =  30.0         "
               "step  = 0.1           "
               "help  = ' '           ");

    // Energy distribution
    TwAddVarCB(bar, "Primary", TW_TYPE_FLOAT, set_primary, get_primary, NULL,
               "label = 'Primary weight'      "
               "group = 'Energy distribution' "
               "min   = 0                     "
               "max   = 1                     "
               "step  = 0.01                  "
               "help  = ' '                   " );

    TwAddVarCB(bar, "Secondary", TW_TYPE_FLOAT, set_secondary, get_secondary, NULL,
               "label = 'Secondy weight'      "
               "group = 'Energy distribution' "
               "min   = 0                     "
               "max   = 1                     "
               "step  = 0.01                  "
               "help  = ' '                   " );

    TwAddVarCB(bar, "Tertiary", TW_TYPE_FLOAT, set_tertiary, get_tertiary, NULL,
               "label = 'Tertiary weight'      "
               "group = 'Energy distribution' "
               "min   = 0                     "
               "max   = 1                     "
               "step  = 0.01                  "
               "help  = ' '                   " );

    TwAddSeparator(bar, "",
                   "group = 'Energy distribution' " );

    TwAddVarCB(bar, "Gamma", TW_TYPE_FLOAT, set_gamma, get_gamma, NULL,
               "label = 'Gamma correction'    "
               "group = 'Energy distribution' "
               "min   = 0.50                  "
               "max   = 2.5                   "
               "step  = 0.01                  "
               "help  = ' '                   ");

    TwAddSeparator(bar, "", "");
    TwAddButton(bar, "Reset", (TwButtonCallback) reset, NULL,
                "help='Reset all parameters to default values.'");
    TwAddSeparator(bar, "", "");
    TwAddButton(bar, "Quit", (TwButtonCallback) quit, NULL,
                "help='Quit.'");

    buffer_a = text_buffer_new( 1 );
    buffer_rgb = text_buffer_new( 3 );
    buffer = buffer_rgb;
    reset();

    glutTimerFunc( 1000.0/60.0, timer, 60 );

    mat4_set_identity( &projection );
    mat4_set_identity( &model );
    mat4_set_identity( &view );

    glutMainLoop();
    return EXIT_SUCCESS;
}

