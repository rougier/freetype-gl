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
#include "font-manager.h"
#include "texture-font.h"
#include "texture-atlas.h"



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

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}

void keyboard( unsigned char key, int x, int y )
{
    if ( key == 27 )
    {
        exit( 1 );
    }
}

int main( int argc, char **argv )
{
    int bold   = 0;
    int italic = 0;
    char * family = "Bitstream Vera Sans";
    float minsize = 8, maxsize = 28;
    size_t count = maxsize - minsize;
    wchar_t *cache = L" !\"#$%&'()*+,-./0123456789:;<=>?"
                     L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                     L"`abcdefghijklmnopqrstuvwxyz{|}~";
    char * filename;
    TextureAtlas *atlas;
    TextureFont *font;
    size_t i, missed = 0;

    glutInit( &argc, argv );
    glutInitWindowSize( 512, 512 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    atlas = texture_atlas_new( 512, 512 );
    for( i=0; i < count; ++i)
    {
        filename = font_manager_match_description( 0, family, minsize+i, bold, italic );
        font = texture_font_new( atlas, filename, minsize+i );
        missed += texture_font_cache_glyphs( font, cache );
        texture_font_delete(font);
    }
    glBindTexture( GL_TEXTURE_2D, atlas->texid );
    printf( "Matched font               : %s\n", filename );
    printf( "Number of fonts            : %ld\n", count );
    printf( "Number of glyphs per font  : %ld\n", wcslen(cache) );
    printf( "Number of missed glyphs    : %ld\n", missed );
    printf( "Total number of glyphs     : %ld/%ld\n",
            wcslen(cache)*count - missed, wcslen(cache)*count );
    printf( "Texture size               : %ldx%ld\n", atlas->width, atlas->height );
    printf( "Texture occupancy          : %.2f%%\n", 
            100.0*atlas->used/(float)(atlas->width*atlas->height) );
    glutMainLoop( );
    return 0;
}
