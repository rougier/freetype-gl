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
#include <stdarg.h>
#include "freetype-gl.h"
#include "font-manager.h"
#include "vertex-buffer.h"
#include "markup.h"


// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, z;
    float u, v;
    float r, g, b, a;
    float shift, gamma;
} vertex_t;


// ------------------------------------------------------- global variables ---
font_manager_t * manager;
vertex_buffer_t * buffer;
GLuint program = 0;
GLuint texture_location;
GLuint pixel_location;

// ------------------------------------------------------------ read_shader ---
char *
read_shader( const char *filename )
{
    FILE * file;
    char * buffer;
	size_t size;

    file = fopen( filename, "rb" );
    if( !file )
    {
        fprintf( stderr, "Unable to open file \"%s\".\n", filename );
		return 0;
    }
	fseek( file, 0, SEEK_END );
	size = ftell( file );
	fseek(file, 0, SEEK_SET );
    buffer = (char *) malloc( (size+1) * sizeof( char *) );
	fread( buffer, sizeof(char), size, file );
    buffer[size] = 0;
    fclose( file );
    return buffer;
}


// ----------------------------------------------------------- build_shader ---
GLuint
build_shader( const char* source, GLenum type )
{
    GLuint handle = glCreateShader( type );
    glShaderSource( handle, 1, &source, 0 );
    glCompileShader( handle );

    GLint compile_status;
    glGetShaderiv( handle, GL_COMPILE_STATUS, &compile_status );
    if( compile_status == GL_FALSE )
    {
        GLchar messages[256];
        glGetShaderInfoLog( handle, sizeof(messages), 0, &messages[0] );
        fprintf( stderr, "%s\n", messages );
        exit( EXIT_FAILURE );
    }
    return handle;
}


// ---------------------------------------------------------- build_program ---
GLuint build_program( const char * vertex_source,
                      const char * fragment_source )
{
    GLuint vertex_shader   = build_shader( vertex_source, GL_VERTEX_SHADER);
    GLuint fragment_shader = build_shader( fragment_source, GL_FRAGMENT_SHADER);
    
    GLuint handle = glCreateProgram( );
    glAttachShader( handle, vertex_shader);
    glAttachShader( handle, fragment_shader);
    glLinkProgram( handle );
    
    GLint link_status;
    glGetProgramiv( handle, GL_LINK_STATUS, &link_status );
    if (link_status == GL_FALSE)
    {
        GLchar messages[256];
        glGetProgramInfoLog( handle, sizeof(messages), 0, &messages[0] );
        fprintf( stderr, "%s\n", messages );
        exit(1);
    }
    
    return handle;
}


// ---------------------------------------------------------------- display ---
void display( void )
{
    glClearColor(1.00,1.00,1.00,1.00);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_BLEND );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_COLOR_MATERIAL );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
    glColor4f( 1.0, 1.0, 1.0, 1.0); 
    glBlendColor( 1.0, 1.0, 1.0, 1.0 );
    glUseProgram( program );
    glUniform1i( texture_location, 0 );
    glUniform2f( pixel_location,
                 1.0/manager->atlas->width,
                 1.0/manager->atlas->height );
    vertex_buffer_render( buffer, GL_TRIANGLES, "vtc" );
    glUseProgram( 0 );
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


// --------------------------------------------------------------- add_text ---
void add_text( vertex_buffer_t * buffer, vec2 * pen,
               wchar_t * text, size_t length, markup_t *markup )
{
    if( !markup->font )
    {
        markup->font = font_manager_get_from_markup( manager, markup );
        if( ! markup->font )
        {
            fprintf( stderr, "Houston, we've got a problem !\n" );
            exit( EXIT_FAILURE );
        }
    }

    size_t i;
    texture_font_t * font = markup->font;
    float r = markup->foreground_color.red;
    float g = markup->foreground_color.green;
    float b = markup->foreground_color.blue;
    float a = markup->foreground_color.alpha;
    float gamma = markup->gamma;

    for( i=0; i<length; ++i )
    {
        if( text[i] == L'\n' )
        {
            continue;
        }

        texture_glyph_t *glyph = texture_font_get_glyph( font, text[i] );
        texture_glyph_t *black = texture_font_get_glyph( font, -1 );

        if( glyph != NULL )
        {
            float kerning = 0;
            if( i > 0)
            {
                kerning = texture_glyph_get_kerning( glyph, text[i-1] );
            }
            pen->x += kerning;

            /* Background */
            if( markup->background_color.alpha > 0 )
            {
                float r = markup->background_color.r;
                float g = markup->background_color.g;
                float b = markup->background_color.b;
                float a = markup->background_color.a;
                float x0  = ( pen->x -kerning );
                float y0  = (int)( pen->y + font->descender );
                float x1  = ( x0 + glyph->advance_x );
                float y1  = (int)( y0 + font->height + font->linegap );
                float s0 = black->s0;
                float t0 = black->t0;
                float s1 = black->s1;
                float t1 = black->t1;
                GLuint index = buffer->vertices->size;
                GLuint indices[] = {index, index+1, index+2,
                                    index, index+2, index+3};
                vertex_t vertices[] = {
                    { (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma },
                    { (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma },
                    { (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma },
                    { (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma } };
                vertex_buffer_push_back_indices( buffer, indices, 6 );
                vertex_buffer_push_back_vertices( buffer, vertices, 4 );
            }

            /* Underline */
            if( markup->underline )
            {
                float r = markup->underline_color.r;
                float g = markup->underline_color.g;
                float b = markup->underline_color.b;
                float a = markup->underline_color.a;
                float x0  = ( pen->x - kerning );
                float y0  = (int)( pen->y + font->underline_position );
                float x1  = ( x0 + glyph->advance_x );
                float y1  = (int)( y0 + font->underline_thickness ); 
                float s0 = black->s0;
                float t0 = black->t0;
                float s1 = black->s1;
                float t1 = black->t1;
                GLuint index = buffer->vertices->size;
                GLuint indices[] = {index, index+1, index+2,
                                    index, index+2, index+3};
                vertex_t vertices[] = {
                    { (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma },
                    { (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma },
                    { (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma },
                    { (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma } };
                vertex_buffer_push_back_indices( buffer, indices, 6 );
                vertex_buffer_push_back_vertices( buffer, vertices, 4 );
            }

            /* Overline */
            if( markup->overline )
            {
                float r = markup->overline_color.r;
                float g = markup->overline_color.g;
                float b = markup->overline_color.b;
                float a = markup->overline_color.a;
                float x0  = ( pen->x -kerning );
                float y0  = (int)( pen->y + (int)font->ascender );
                float x1  = ( x0 + glyph->advance_x );
                float y1  = (int)( y0 + (int)font->underline_thickness ); 
                float s0 = black->s0;
                float t0 = black->t0;
                float s1 = black->s1;
                float t1 = black->t1;
                GLuint index = buffer->vertices->size;
                GLuint indices[] = {index, index+1, index+2,
                                    index, index+2, index+3};
                vertex_t vertices[] = {
                    { (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma },
                    { (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma },
                    { (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma },
                    { (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma } };
                vertex_buffer_push_back_indices( buffer, indices, 6 );
                vertex_buffer_push_back_vertices( buffer, vertices, 4 );
            }

            /* Strikethrough */
            if( markup->strikethrough )
            {
                float r = markup->overline_color.r;
                float g = markup->overline_color.g;
                float b = markup->overline_color.b;
                float a = markup->overline_color.a;
                float x0  = ( pen->x -kerning );
                float y0  = (int)( pen->y + (int)font->ascender*.25);
                float x1  = ( x0 + glyph->advance_x );
                float y1  = (int)( y0 + (int)font->underline_thickness ); 
                float s0 = black->s0;
                float t0 = black->t0;
                float s1 = black->s1;
                float t1 = black->t1;
                GLuint index = buffer->vertices->size;
                GLuint indices[] = {index, index+1, index+2,
                                    index, index+2, index+3};
                vertex_t vertices[] = {
                    { (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma },
                    { (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma },
                    { (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma },
                    { (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma } };
                vertex_buffer_push_back_indices( buffer, indices, 6 );
                vertex_buffer_push_back_vertices( buffer, vertices, 4 );
            }

            /* Actual glyph */
            float x0  = ( pen->x + glyph->offset_x );
            float y0  = (int)( pen->y + glyph->offset_y );
            float x1  = ( x0 + glyph->width );
            float y1  = (int)( y0 - glyph->height );
            float s0 = glyph->s0;
            float t0 = glyph->t0;
            float s1 = glyph->s1;
            float t1 = glyph->t1;
            GLuint index = buffer->vertices->size;
            GLuint indices[] = {index, index+1, index+2,
                                index, index+2, index+3};
            vertex_t vertices[] = {
                { (int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma },
                { (int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma },
                { (int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma },
                { (int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma } };

            vertex_buffer_push_back_indices( buffer, indices, 6 );
            vertex_buffer_push_back_vertices( buffer, vertices, 4 );
            pen->x += glyph->advance_x;
        }
    }
}


// ------------------------------------------------------------ init_colors ---
void
init_colors( vec4 *colors )
{
    vec4 defaults[16] = 
        {
            {{ 46/256.0f,  52/256.0f,  54/256.0f, 1.0f}},
            {{204/256.0f,   0/256.0f,   0/256.0f, 1.0f}},
            {{ 78/256.0f, 154/256.0f,   6/256.0f, 1.0f}},
            {{196/256.0f, 160/256.0f,   0/256.0f, 1.0f}},
            {{ 52/256.0f, 101/256.0f, 164/256.0f, 1.0f}},
            {{117/256.0f,  80/256.0f, 123/256.0f, 1.0f}},
            {{  6/256.0f, 152/256.0f, 154/256.0f, 1.0f}},
            {{211/256.0f, 215/256.0f, 207/256.0f, 1.0f}},
            {{ 85/256.0f,  87/256.0f,  83/256.0f, 1.0f}},
            {{239/256.0f,  41/256.0f,  41/256.0f, 1.0f}},
            {{138/256.0f, 226/256.0f,  52/256.0f, 1.0f}},
            {{252/256.0f, 233/256.0f,  79/256.0f, 1.0f}},
            {{114/256.0f, 159/256.0f, 207/256.0f, 1.0f}},
            {{173/256.0f, 127/256.0f, 168/256.0f, 1.0f}},
            {{ 52/256.0f, 226/256.0f, 226/256.0f, 1.0f}},
            {{238/256.0f, 238/256.0f, 236/256.0f, 1.0f}}
        };
    size_t i = 0;
    // Default 16 colors
    for( i=0; i< 16; ++i )
    {
        colors[i] = defaults[i];
    }
    // Color cube
    for( i=0; i<6*6*6; i++ )
    {
        vec4 color = {{ (i/6/6)/5.0f, ((i/6)%6)/5.0f, (i%6)/5.0f, 1.0f}};
        colors[i+16] = color;
    }
    // Grascale ramp (24 tones)
    for( i=0; i<24; i++ )
    {
        vec4 color ={{i/24.0f, i/24.0f, i/24.0f, 1.0f}};
        colors[232+i] = color;
    }
}


// --------------------------------------------------------- ansi_to_markup ---
void
ansi_to_markup( wchar_t *sequence, size_t length, markup_t *markup )
{
    size_t i;
    int code = 0;
    int set_bg = -1;
    int set_fg = -1;
    vec4 none = {{0,0,0,0}};
    static vec4 * colors = 0;

    if( colors == 0 )
    {
        colors = (vec4 *) malloc( sizeof(vec4) * 256 );
        init_colors( colors );
    }


    if( length <= 1 )
    {
        markup->foreground_color = colors[0];
        markup->underline_color = markup->foreground_color;
        markup->overline_color = markup->foreground_color;
        markup->strikethrough_color = markup->foreground_color;
        markup->outline_color = markup->foreground_color;
        markup->background_color = none;
        markup->underline = 0;
        markup->overline = 0;
        markup->bold = 0;
        markup->italic = 0;
        markup->strikethrough = 0;
        return;
    }

    for( i=0; i<length; ++i)
    {
        wchar_t c = *(sequence+i);
        if( c >= '0' && c <= '9' )
        {
            code = code * 10 + (c-'0');
        }
        else if( (c == ';') || (i == (length-1)) )
        {
            if( set_fg == 1 )
            {
                markup->foreground_color = colors[code];
                set_fg = -1;
            }
            else if( set_bg == 1 )
            {
                markup->background_color = colors[code];
                set_bg = -1;
            }
            else if( (set_fg == 0) && (code == 5) )
            {
                set_fg = 1;
                code = 0;
            }
            else if( (set_bg == 0) && (code == 5) )
            {
                set_bg = 1;
                code = 0;
            }
            // Set fg color (30 + x, where x is the index of the color)
            else if( (code >= 30) && (code < 38 ) )
            {
                markup->foreground_color = colors[code-30];
            }
            // Set bg color (40 + x, where x is the index of the color)
            else if( (code >= 40) && (code < 48 ) )
            {
                markup->background_color = colors[code-40];
            }
            else 
            {
                switch (code)
                {
                case 0:
                    markup->foreground_color = colors[0];
                    markup->background_color = none;
                    markup->underline = 0;
                    markup->overline = 0;
                    markup->bold = 0;
                    markup->italic = 0;
                    markup->strikethrough = 0;
                    break;
                case 1: markup->bold = 1; break;
                case 21: markup->bold = 0; break;
                case 2: markup->foreground_color.alpha = 0.5; break;
                case 22: markup->foreground_color.alpha = 1.0; break;
                case 3:  markup->italic = 1; break;
                case 23: markup->italic = 0; break;
                case 4:  markup->underline = 1; break;
                case 24: markup->underline = 0; break;
                case 8: markup->foreground_color.alpha = 0.0; break;
                case 28: markup->foreground_color.alpha = 1.0; break;
                case 9:  markup->strikethrough = 1; break;
                case 29: markup->strikethrough = 0; break;
                case 53: markup->overline = 1; break;
                case 55: markup->overline = 0; break;
                case 39: markup->foreground_color = colors[0]; break;
                case 49: markup->background_color = none; break;
                case 38: set_fg = 0; break;
                case 48: set_bg = 0; break;
                default: break;
                }
            }
            code = 0;
        }
    }
    markup->underline_color = markup->foreground_color;
    markup->overline_color = markup->foreground_color;
    markup->strikethrough_color = markup->foreground_color;
    markup->outline_color = markup->foreground_color;
}

// ------------------------------------------------------------------ print ---
void
print( vertex_buffer_t * buffer, vec2 * pen,
       wchar_t *text, markup_t *markup )
{
    wchar_t *seq_start = text, *seq_end = text;
    wchar_t *p;
    for( p=text; p<(text+wcslen(text)); ++p )
    {
        wchar_t *start = wcsstr( p, L"\033[" );
        wchar_t *end = NULL;
        if( start) 
        {
            end = wcsstr( start+1, L"m");
        }
        if( (start == p) && (end > start) )
        {
            seq_start = start+2;
            seq_end = end;
            p = end;
        }
        else
        {
            int seq_size = (seq_end-seq_start)+1;
            wchar_t * text_start = p;
            int text_size = 0;
            if( start )
            {
                text_size = start-p;
                p = start-1;
            }
            else
            {
                text_size = text+wcslen(text)-p;
                p = text+wcslen(text);
            }                
            //wprintf( L"\033[%.*lsm", seq_size, seq_start);
            //wprintf( L"%.*ls", text_size, text_start);
            ansi_to_markup(seq_start, seq_size, markup );
            markup->font = font_manager_get_from_markup( manager, markup );
            add_text( buffer, pen, text_start, text_size, markup );
        }
    }
}


// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitWindowSize( 800, 500 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    manager = font_manager_new( 512, 512, 3 );
    buffer = vertex_buffer_new( "v3f:t2f:c4f:1g1f:2g1f" ); 
    vec4 black  = {{0.0, 0.0, 0.0, 1.0}};
    vec4 none   = {{1.0, 1.0, 1.0, 0.0}};
    markup_t markup = {
        .family  = "Bitstream Vera Sans Mono",
        .size    = 14.0,
        .bold    = 0,
        .italic  = 0,
        .rise    = 0.0,
        .spacing = 0.0,
        .gamma   = 0.85,
        .foreground_color    = black,
        .background_color    = none,
        .underline           = 0,
        .underline_color     = black,
        .overline            = 0,
        .overline_color      = black,
        .strikethrough       = 0,
        .strikethrough_color = black,
        .font = 0,
    };
    markup.font = font_manager_get_from_markup( manager, &markup );
    vec2 pen = {{10.0, 480.0}};

    FILE *file = fopen ( "256colors.txt", "r" );
    if ( file != NULL )
    {
        wchar_t line[1024];
        while( fgetws ( line, sizeof(line), file ) != NULL )
        {
            print(buffer, &pen, line, &markup);
            pen.x = 10;
            pen.y -= markup.font->height-1;
        }
        fclose ( file );
    }


    // Create the GLSL program
    char * vertex_shader_source   = read_shader("./markup.vert");
    char * fragment_shader_source = read_shader("./markup.frag");
    program = build_program( vertex_shader_source, fragment_shader_source );
    texture_location = glGetUniformLocation(program, "texture");
    pixel_location   = glGetUniformLocation(program, "pixel");

    glutMainLoop( );
    return 0;
}
