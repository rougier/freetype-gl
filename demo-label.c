/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * API version: 1.0
 * WWW:         http://code.google.com/p/freetype-gl/
 * -------------------------------------------------------------------------
 * Copyright (c) 2011 Nicolas P. Rougier <Nicolas.Rougier@inria.fr>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 * ========================================================================= */
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <stdlib.h>

void display( void )
{
    int viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glColor4f(1,1,1,1);

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
    glutInit( &argc, argv );
    glutInitWindowSize( 800, 400 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow( "Freetype OpenGL" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glClearColor(1,1,1,1);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

/*
    TextTag *tag = text_tag_new();
    label = label_new();

    text_tag_set_font_desc( tag, "Monotype");
    text_tag_set_font_size( tag, 16);
    text_tag_set_fg_color( tag, 0,0,0,1);
    text_tag_set_bg_color( tag, 0,0,0,1);
    text_tag_set_underline( tag, 0);
    text_tag_set_underline_color( tag, 0);
    text_tag_set_overline( tag, 0);
    text_tag_set_overline_color( tag, 0);
    text_tag_set_crossed_out( tag, 0);
    text_tag_set_crossed_out_color( tag, 0);
    label->add_text( label, L"A Quick Brown Fox Jumps Over The Lazy Dog", tag );
*/
    glutMainLoop( );
    return 0;
}
