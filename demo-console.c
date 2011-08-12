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
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "vector.h"
#include "vertex-buffer.h"
#include "font-manager.h"
#include "texture-font.h"
#include "texture-atlas.h"



// -------------------------------------------------------------- constants ---
const int __SIGNAL_ACTIVATE__     = 0;
const int __SIGNAL_COMPLETE__     = 1;
const int __SIGNAL_HISTORY_NEXT__ = 2;
const int __SIGNAL_HISTORY_PREV__ = 3;
#define MAX_LINE_LENGTH  511


const int MARKUP_NORMAL      = 0;
const int MARKUP_BOLD        = 1;
const int MARKUP_ITALIC      = 2;
const int MARKUP_BOLD_ITALIC = 3;
const int MARKUP_FAINT       = 4;
#define MARKUP_COUNT 5


// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x,y;
    Color color;
} Point;

struct Terminal_ {
    Vector *       lines;
    wchar_t *      prompt;
    wchar_t        killring[MAX_LINE_LENGTH+1];
    wchar_t        input[MAX_LINE_LENGTH+1];
    size_t         cursor;
    TextMarkup     markup[MARKUP_COUNT];
    VertexBuffer * buffer;
    Pen            pen; 
    void (*handlers[4])( struct Terminal_ *, wchar_t * );
};
typedef struct Terminal_ Terminal;


// ------------------------------------------------------- global variables ---
static Terminal * terminal = 0;
static FontManager * manager = 0;
static VertexBuffer *lines_buffer;



// ----------------------------------------------------------------------------
Terminal *
terminal_new( void )
{
    Terminal *self = (Terminal *) malloc( sizeof(Terminal) );
    if( !self )
        return self;

    self->lines = vector_new( sizeof(wchar_t *) );
    self->prompt = (wchar_t *) wcsdup( L">>> " );
    self->cursor = 0;
    self->buffer = vertex_buffer_new( "v3i:t2f:c4f" );
    self->input[0] = L'\0';
    self->killring[0] = L'\0';
    self->handlers[__SIGNAL_ACTIVATE__]     = 0;
    self->handlers[__SIGNAL_COMPLETE__]     = 0;
    self->handlers[__SIGNAL_HISTORY_NEXT__] = 0;
    self->handlers[__SIGNAL_HISTORY_PREV__] = 0;
    self->pen.x = self->pen.y = 0; 
    TextMarkup normal = { "Mono", 12, 0, 0, 0.0, 0.0,
                          {0,0,0,1}, {1,1,1,0},
                          0, {0,0,0,1}, 0, {0,0,0,1},
                          0, {0,0,0,1}, 0, {0,0,0,1} };
    TextMarkup bold        = normal; bold.bold = 1;
    TextMarkup italic      = normal; italic.italic = 1;
    TextMarkup bold_italic = normal; bold.bold = 1; italic.italic = 1;
    TextMarkup faint       = normal;
    faint.foreground_color.r = 0.35;
    faint.foreground_color.g = 0.35;
    faint.foreground_color.b = 0.35;

    self->markup[MARKUP_NORMAL]      = normal;
    self->markup[MARKUP_FAINT]       = faint;
    self->markup[MARKUP_BOLD]        = bold;
    self->markup[MARKUP_ITALIC]      = italic;
    self->markup[MARKUP_BOLD_ITALIC] = bold_italic;

    if( !manager )
    {
        manager = font_manager_new( );
    }
    size_t i;
    for( i=0; i < MARKUP_COUNT; ++i )
    {
        self->markup[i].font = font_manager_get_from_markup( manager, &self->markup[i] );
    }
    return self;
}



// -------------------------------------------------------- terminal_delete ---
void
terminal_delete( Terminal *self )
{ }



// ----------------------------------------------------- terminal_add_glyph ---
void
terminal_add_glyph( Terminal *self,
                    wchar_t current,
                    wchar_t previous,
                    TextMarkup *markup )
{
    TextureGlyph *glyph  = texture_font_get_glyph( markup->font, current );
    if( previous != L'\0' )
    {
        self->pen.x += texture_glyph_get_kerning( glyph, previous );
    }
    int x0  = self->pen.x + glyph->offset_x;
    int y0  = self->pen.y + glyph->offset_y;
    int x1  = x0 + glyph->width;
    int y1  = y0 - glyph->height;
    float u0 = glyph->u0;
    float v0 = glyph->v0;
    float u1 = glyph->u1;
    float v1 = glyph->v1;
    GLuint index = self->buffer->vertices->size;
    GLuint indices[] = {index, index+1, index+2,
                        index, index+2, index+3};

    float r = markup->foreground_color.r;
    float g = markup->foreground_color.g;
    float b = markup->foreground_color.b;
    float a = markup->foreground_color.a;
    TextureGlyphVertex vertices[] = { { x0,y0,0,  u0,v0,  r,g,b,a },
                                      { x0,y1,0,  u0,v1,  r,g,b,a },
                                      { x1,y1,0,  u1,v1,  r,g,b,a },
                                      { x1,y0,0,  u1,v0,  r,g,b,a } };
    vertex_buffer_push_back_indices( self->buffer, indices, 6 );
    vertex_buffer_push_back_vertices( self->buffer, vertices, 4 );
    self->pen.x += glyph->advance_x;
    self->pen.y += glyph->advance_y;
}



// -------------------------------------------------------- terminal_render ---
void
terminal_render( Terminal *self )
{
    int viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    size_t i, index;

    self->pen.x = 0;
    self->pen.y = viewport[3]-12;
    vertex_buffer_clear( terminal->buffer );

    int cursor_x = self->pen.x;
    int cursor_y = self->pen.y;

    TextMarkup markup;

    // Console buffer
    markup = self->markup[MARKUP_FAINT];
    for( i=0; i<self->lines->size; ++i )
    {
        wchar_t *text = * (wchar_t **) vector_get( self->lines, i ) ;
        if( wcslen(text) > 0 )
        {
            terminal_add_glyph( terminal, text[0], L'\0', &markup );
            for( index=1; index < wcslen(text)-1; ++index )
            {
                terminal_add_glyph( terminal, text[index], text[index-1], &markup );
            }
        }
        self->pen.y -= 12;
        self->pen.x = 0;
        cursor_x = self->pen.x;
        cursor_y = self->pen.y;
    }

    // Prompt
    markup = self->markup[MARKUP_BOLD];
    if( wcslen( self->prompt ) > 0 )
    {
        terminal_add_glyph( terminal, self->prompt[0], L'\0', &markup );
        for( index=1; index < wcslen(self->prompt); ++index )
        {
            terminal_add_glyph( terminal, self->prompt[index], self->prompt[index-1], &markup );
        }
    }
    cursor_x = (int) self->pen.x;

    // Input
    markup = self->markup[MARKUP_NORMAL];
    if( wcslen(self->input) > 0 )
    {
        terminal_add_glyph( terminal, self->input[0], L'\0', &markup );
        if( self->cursor > 0)
        {
            cursor_x = (int) self->pen.x;
        }
        for( index=1; index < wcslen(self->input); ++index )
        {
            terminal_add_glyph( terminal, self->input[index], self->input[index-1], &markup );
            if( index < self->cursor )
            {
                cursor_x = (int) self->pen.x;
            }
        }
    }

    // Cursor
    vertex_buffer_clear( lines_buffer );
    Point p = {cursor_x+1, cursor_y, markup.foreground_color};
    vertex_buffer_push_back_vertex( lines_buffer, &p );
    p.y += 12;
    vertex_buffer_push_back_vertex( lines_buffer, &p );


    glColor4f(1,1,1,1);
    glEnable( GL_TEXTURE_2D );
    vertex_buffer_render( terminal->buffer, GL_TRIANGLES, "vtc" );
    glDisable( GL_TEXTURE_2D );
    vertex_buffer_render( lines_buffer, GL_LINES, "vc" );
}



// ------------------------------------------------------- terminal_connect ---
void
terminal_connect( Terminal *self,
                  const char *signal,
                  void (*handler)(Terminal *, wchar_t *))
{
    if( strcmp( signal,"activate" ) == 0 )
    {
        self->handlers[__SIGNAL_ACTIVATE__] = handler;
    }
    else if( strcmp( signal,"complete" ) == 0 )
    {
        self->handlers[__SIGNAL_COMPLETE__] = handler;
    }
    else if( strcmp( signal,"history-next" ) == 0 )
    {
        self->handlers[__SIGNAL_HISTORY_NEXT__] = handler;
    }
    else if( strcmp( signal,"history-prev" ) == 0 )
    {
        self->handlers[__SIGNAL_HISTORY_PREV__] = handler;
    }
}



// --------------------------------------------------------- terminal_print ---
void
terminal_print( Terminal *self,
                wchar_t *text )
{
    // fwprintf( stderr, L"Printing %ls\n", text );

    // Make sure there is at least one line
    if( self->lines->size == 0 )
    {
        wchar_t *line = wcsdup( L"" );
        vector_push_back( self->lines, &line );
    }

    // Make sure last line does not end with '\n'
    wchar_t *last_line = *(wchar_t **) vector_get( self->lines, self->lines->size-1 ) ;
    if( wcslen( last_line ) != 0 )
    {
        if( last_line[wcslen( last_line ) - 1] == L'\n' )
        {
            wchar_t *line = wcsdup( L"" );
            vector_push_back( self->lines, &line );
        }
    }
    last_line = *(wchar_t **) vector_get( self->lines, self->lines->size-1 ) ;

    wchar_t *start = text;
    wchar_t *end   = wcschr(start, L'\n');
    size_t len = wcslen( last_line );
    if( end != NULL)
    {
        wchar_t *line = (wchar_t *) malloc( (len + end - start + 2)*sizeof( wchar_t ) );
        wcpncpy( line, last_line, len );
        wcpncpy( line + len, text, end-start+1 );

        line[len+end-start+1] = L'\0';

        vector_set( self->lines, self->lines->size-1, &line );
        free( last_line );
        if( (end-start)  < (wcslen( text )-1) )
        {
            terminal_print(self, end+1 );
        }
        return;
    }
    else
    {
        wchar_t *line = (wchar_t *) malloc( (len + wcslen(text) + 1) * sizeof( wchar_t ) );
        wcpncpy( line, last_line, len );
        wcpcpy( line + len, text );
        vector_set( self->lines, self->lines->size-1, &line );
        free( last_line );
        return;
    }
}



// ------------------------------------------------------- terminal_process ---
void
terminal_process( Terminal *self,
                  const char *action,
                  const unsigned char key )
{
    size_t len = wcslen(self->input);

    if( strcmp(action, "type") == 0 )
    {
        if( len < MAX_LINE_LENGTH )
        {
            memmove( self->input + self->cursor+1,
                     self->input + self->cursor, 
                     (len - self->cursor+1)*sizeof(wchar_t) );
            self->input[self->cursor] = (wchar_t) key;
            self->cursor++;
        }
        else
        {
            fprintf( stderr, "Input buffer is full\n" );
        }
    }
    else
    {
        if( strcmp( action, "enter" ) == 0 )
        {
            if( terminal->handlers[__SIGNAL_ACTIVATE__] )
            {
                (*terminal->handlers[__SIGNAL_ACTIVATE__])(terminal, terminal->input);
            }
            terminal_print( self, self->prompt );
            terminal_print( self, self->input );
            terminal_print( self, L"\n" );
            self->input[0] = L'\0';
            self->cursor = 0;
        }
        else if( strcmp( action, "right" ) == 0 )
        {
            if( self->cursor < wcslen(self->input) )
            {
                self->cursor += 1;
            }
        }
        else if( strcmp( action, "left" ) == 0 )
        {
            if( self->cursor > 0 )
            {
                self->cursor -= 1;
            }
        }
        else if( strcmp( action, "delete" ) == 0 )
        {
            memmove( self->input + self->cursor,
                     self->input + self->cursor+1, 
                     (len - self->cursor)*sizeof(wchar_t) );
        }
        else if( strcmp( action, "backspace" ) == 0 )
        {
            if( self->cursor > 0 )
            {
                memmove( self->input + self->cursor-1,
                         self->input + self->cursor, 
                         (len - self->cursor+1)*sizeof(wchar_t) );
                self->cursor--;
            }
        }
        else if( strcmp( action, "kill" ) == 0 )
        {
            if( self->cursor < len )
            {
                wcpcpy(self->killring, self->input + self->cursor );
                self->input[self->cursor] = L'\0';
                fwprintf(stderr, L"Kill ring: %ls\n", self->killring);
            }
            
        }
        else if( strcmp( action, "yank" ) == 0 )
        {
            size_t l = wcslen(self->killring);
            if( (len + l) < MAX_LINE_LENGTH )
            {
                memmove( self->input + self->cursor + l,
                         self->input + self->cursor, 
                         (len - self->cursor)*sizeof(wchar_t) );
                memcpy( self->input + self->cursor,
                        self->killring, l*sizeof(wchar_t));
                self->cursor += l;
            }
        }
        else if( strcmp( action, "home" ) == 0 )
        {
            self->cursor = 0;
        }
        else if( strcmp( action, "end" ) == 0 )
        {
            self->cursor = wcslen( self->input );
        }
        else if( strcmp( action, "clear" ) == 0 )
        {
        }
        else if( strcmp( action, "history-prev" ) == 0 )
        {
            if( terminal->handlers[__SIGNAL_HISTORY_PREV__] )
            {
                (*terminal->handlers[__SIGNAL_HISTORY_PREV__])(terminal, terminal->input);
            }
        }
        else if( strcmp( action, "history-next" ) == 0 )
        {
            if( terminal->handlers[__SIGNAL_HISTORY_NEXT__] )
            {
                (*terminal->handlers[__SIGNAL_HISTORY_NEXT__])(terminal, terminal->input);
            }
        }
        else if( strcmp( action, "complete" ) == 0 )
        {
            if( terminal->handlers[__SIGNAL_COMPLETE__] )
            {
                (*terminal->handlers[__SIGNAL_COMPLETE__])(terminal, terminal->input);
            }
        }
    }
}



// ----------------------------------------------------------- on_key_press ---
void
on_key_press ( unsigned char key, int x, int y )
{
    // fprintf( stderr, "key: %d\n", key);
    if (key == 1)
    {
        terminal_process( terminal, "home", 0 );
    }
    else if (key == 4)
    { 
        terminal_process( terminal, "delete", 0 );
    }
    else if (key == 5)
    { 
        terminal_process( terminal, "end", 0 );
    }
    else if (key == 8)
    { 
        terminal_process( terminal, "backspace", 0 );
    }
    else if (key == 9)
    {
        terminal_process( terminal, "complete", 0 );
    }
    else if (key == 11)
    {
        terminal_process( terminal, "kill", 0 );
    }
    else if (key == 12)
    {
        terminal_process( terminal, "clear", 0 );
    }
    else if (key == 13)
    {
        terminal_process( terminal, "enter", 0 );
    }
    else if (key == 25)
    {
        terminal_process( terminal, "yank", 0 );
    }
    else if (key == 27)
    {
        terminal_process( terminal, "escape", 0 );
    }
    else if (key == 127)
    {
        terminal_process( terminal, "backspace", 0 );
    }
    else if( key > 31)
    {
        terminal_process( terminal, "type", key );
    }
    glutPostRedisplay();
}



// --------------------------------------------------- on_special_key_press ---
void
on_special_key_press( int key, int x, int y )
{
    switch (key)
    {
    case GLUT_KEY_UP:
        terminal_process( terminal, "history-prev", 0 );
        break;
    case GLUT_KEY_DOWN:
        terminal_process( terminal, "history-next", 0 );
        break;
    case GLUT_KEY_LEFT:
        terminal_process( terminal,  "left", 0 );
        break;
    case GLUT_KEY_RIGHT:
        terminal_process( terminal, "right", 0 );
        break;
    case GLUT_KEY_HOME:
        terminal_process( terminal, "home", 0 );
        break;
    case GLUT_KEY_END:
        terminal_process( terminal, "end", 0 );
        break;
    default:
        break;
    }
    glutPostRedisplay();
}



// ------------------------------------------------------------- on_display ---
void on_display (void) {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terminal_render( terminal );
    glutSwapBuffers();
}



// ------------------------------------------------------------- on_reshape ---
void on_reshape (int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}



// ---------------------------------------------------------------- on_init ---
void on_init( void )
{
    glClearColor( 1.00, 1.00, 0.95, 1.00 );
    glDisable( GL_DEPTH_TEST ); 
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
}


// ----------------------------------------------------------------------------
void terminal_activate (Terminal *self, wchar_t * input)
{
    //terminal_print( self, L"Activate callback\n" );
    fwprintf( stderr, L"Activate callback : %ls\n", input );
}
void terminal_complete (Terminal *self, wchar_t *input)
{
    // terminal_print( self, L"Complete callback\n" );
    fwprintf( stderr, L"Complete callback : %ls\n", input );
}
void terminal_history_prev (Terminal *self, wchar_t *input)
{
    // terminal_print( self, L"History prev callback\n" );
    fwprintf( stderr, L"History prev callback : %ls\n", input );
}
void terminal_history_next (Terminal *self, wchar_t *input)
{
    // terminal_print( self, L"History next callback\n" );
    fwprintf( stderr, L"History next callback : %ls\n", input );
}



int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "GL Terminal" );
    glutReshapeFunc( on_reshape );
    glutDisplayFunc( on_display );
    glutKeyboardFunc( on_key_press );
    glutSpecialFunc( on_special_key_press );
    glutReshapeWindow( 600,400 );


    lines_buffer = vertex_buffer_new( "v2f:c4f" ); 
    terminal = terminal_new();
    terminal_print( terminal,
                    L"OpenGL Freetype console\n"
                    L"Copyright 2011 Nicolas P. Rougier. All rights reserved.\n \n" );
    terminal_connect( terminal, "activate",     terminal_activate );
    terminal_connect( terminal, "complete",     terminal_complete );
    terminal_connect( terminal, "history-prev", terminal_history_prev );
    terminal_connect( terminal, "history-next", terminal_history_next );

    on_init();
    glutMainLoop();
    return 0;
}
