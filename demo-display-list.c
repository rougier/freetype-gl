/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * -------------------------------------------------------------------------
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
 * =========================================================================
 *
 * Example usage with display lists
 *
 * ========================================================================= */
#include "freetype-gl.h"


// ------------------------------------------------------- global variables ---
vector_t * fonts;
texture_atlas_t *atlas;
char *filename = "fonts/Vera.ttf";
wchar_t *text = L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789";



// --------------------------------------------------------------- gl_print ---
void gl_print( texture_font_t * font,
               wchar_t * text )
{
    size_t i;
    texture_glyph_t * glyph = 0;
    texture_atlas_t * atlas = font->atlas;

    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, atlas->id );
    glEnable( GL_BLEND );

    // For ALPHA texture (atlas depth = 1)
    if( atlas->depth == 1 )
    {
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glColor4f( 0, 0, 0, 1 );  // Actual font color
    }
    // For RGB texture (atlas depth = 3)
    else if( atlas->depth == 3 )
    {
        glEnable( GL_COLOR_MATERIAL );
        glBlendFunc( GL_CONSTANT_COLOR_EXT, GL_ONE_MINUS_SRC_COLOR );
        glColor4f( 1, 1, 1, 1 );
        glBlendColor( 0, 0, 0, 1 );
    }

    glPushMatrix();
    for( i=0; i<wcslen(text); ++i)
    {
        glyph = (texture_glyph_t *) texture_font_get_glyph( font, text[i] );
        if( i > 0 )
        {
            glTranslatef(0, texture_glyph_get_kerning( glyph, text[i-1] ), 0);
        }
        glCallList( glyph->id );
        glTranslatef( (int)glyph->advance_x, 0, 0 );
    }
    glPopMatrix();
}


// ------------------------------------------------------------ gl_print_at ---
void gl_print_at( texture_font_t * font,
                  int x, int y, wchar_t * text )
{
    glPushMatrix();  
    glTranslatef(x, y-(int)font->height, 0);
    gl_print( font, text );
    glPopMatrix();
}


// ---------------------------------------------------------------- display ---
void display( void )
{
    size_t i;
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPushMatrix();
    glTranslatef(5, viewport[3], 0);
    for( i=0; i<vector_size(fonts); ++i)
    {
        texture_font_t * font = (texture_font_t *) vector_get(fonts, i);
        glTranslatef(0, -(int)font->height, 0);
        gl_print( font, text );
    }
    glPopMatrix();
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


// ----------------------------------------------------- make_display_lists ---
void
make_display_lists( texture_font_t * self )
{
    size_t i;
    texture_glyph_t * glyph;

    for( i=0; i<vector_size(self->glyphs) ; ++i)
    {
        glyph = *(texture_glyph_t **) vector_get( self->glyphs, i);
        int x  = glyph->offset_x;
        int y  = glyph->offset_y;
        int w  = glyph->width;
        int h  = glyph->height;
        float s0 = glyph->s0;
        float t0 = glyph->t0;
        float s1 = glyph->s1;
        float t1 = glyph->t1;
        if( glyph->id )
        {
            glDeleteLists( glyph->id, 1 );
        }
        glyph->id = glGenLists( 1 );
        glNewList( glyph->id,GL_COMPILE );
        glBegin( GL_TRIANGLES );
        glTexCoord2f( s0, t0 ); glVertex2i( x,   y   );
        glTexCoord2f( s0, t1 ); glVertex2i( x,   y-h );
        glTexCoord2f( s1, t1 ); glVertex2i( x+w, y-h );
        glTexCoord2f( s0, t0 ); glVertex2i( x,   y   );
        glTexCoord2f( s1, t1 ); glVertex2i( x+w, y-h );
        glTexCoord2f( s1, t0 ); glVertex2i( x+w, y   );
        glEnd();
        glEndList();
    }
};


// ------------------------------------------------------------------- main ---
int main( int argc, char **argv )
{
    size_t i;

    glutInit( &argc, argv );
    glutInitWindowSize( 800, 400 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL / display lists" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    // Build fonts and corresponding display lists
    fonts = vector_new( sizeof(texture_font_t) );
    atlas = texture_atlas_new( 512, 512, 1 );
    for( i=7; i < 27; ++i)
    {
        texture_font_t * font = texture_font_new( atlas, filename, i );
        texture_font_load_glyphs( font, text );
        make_display_lists( font );
        vector_push_back( fonts, font );
        free( font ); // We're only freeing the local copy of the font
    }
    glutMainLoop( );
    texture_atlas_delete( atlas );
    return 0;
}
