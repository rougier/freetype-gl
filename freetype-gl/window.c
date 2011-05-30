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
#include "window.h"

Window *__active_window__ = 0;

Window *
window_new( int width, int height, char *title, float fps )
{
    Window *window =  glut_window_new( width, height, title, fps );
    __active_window__ = window;
    return window;
}


Window *
window_get_active( void )
{
    return __active_window__;
}



Window *
glut_window_new( int width, int height, char *title, float fps )
{
    Window *window = (Window *) malloc( sizeof(Window) );
    memset( window, 0,  sizeof(Window) );

    int argc = 1;
    char *argv[1] = { (char *) "title"};
    glutInit( &argc, argv );
    int screen_width = glutGet( GLUT_SCREEN_WIDTH );
    int screen_height= glutGet( GLUT_SCREEN_HEIGHT );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowPosition( (screen_width-width)/2,
                            (screen_height-height)/2 );
    glutCreateWindow( title );
    glutReshapeWindow( width, height );
    glutReshapeFunc( glut_window_reshape_handler );
    glutDisplayFunc( glut_window_display_handler );
    glutKeyboardFunc( glut_window_keyboard_handler );

    window->fps   = fps;
    window->run   = glut_window_run;
    window->clear = glut_window_clear;
    window->draw  = glut_window_draw;

    return window;
}



void
glut_window_clear( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}



void
glut_window_draw( void )
{
    glutPostRedisplay( );
}



void
glut_window_run( void )
{
    Window *window = window_get_active( );
    if (!window)
    {
        return;
    }
    GLfloat ambient[]  = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat position[] = {0.0f, 1.0f, 2.0f, 1.0f};
    GLfloat specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glPolygonOffset( 1, 1 );
    glClearColor(1.0,1.0,1.0,1);
    //glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable( GL_DEPTH_TEST ); 
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_LIGHT0 );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
    glLightfv( GL_LIGHT0, GL_POSITION, position );
    glEnable( GL_LINE_SMOOTH );

    glut_window_set_handlers( window );
    if ( window->on_init )
    {
        window->on_init( window );
    }
    if (window->fps > 0)
    {
        glutTimerFunc( (int) 1000.0/window->fps, glut_window_timer_handler, 0 );
    }
    glutMainLoop();
}


void glut_window_timer_handler( int value )
{
    Window *window = window_get_active( );
    if (!window)
    {
        return;
    }
    if (window->on_draw)
    {
        window->on_draw(window);
    }
    glutSwapBuffers();
    if (window->fps > 0) 
    { 
        glutTimerFunc( (int) 1000.0/window->fps, glut_window_timer_handler, 0 );
    } 
}


void glut_window_display_handler( void )
{
    Window *window = window_get_active( );
    if (!window)
    {
        return;
    }

    
    if (window->on_draw)
    {
        window->on_draw(window);
    }
    glutSwapBuffers();
}



void glut_window_reshape_handler( int width, int height )
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    Window *window = window_get_active( );
    if (!window)
    {
        return;
    }
    window->width = width;
    window->height = height;
    if (window->on_resize)
    {
        window->on_resize( window, width, height );
    }
}



void glut_window_mouse_handler( int button, int state, int x, int y )
{
    Window *window = window_get_active( );
    if (!window)
    {
        return;
    }
    window->mouse_button = glut_convert_button( button );
    window->mouse_x = x;
    window->mouse_y = y;
    int modifiers = glut_convert_modifiers( glutGetModifiers() );
    window->keyboard_modifiers = modifiers;
    if ( (state == GLUT_DOWN) && (window->on_mouse_press) ) 
    {
        window->on_mouse_press( window, x, y, window->mouse_button, modifiers );
    }
    else if ( (state == GLUT_UP) && (window->on_mouse_release) ) 
    {
        window->on_mouse_release( window, x, y, window->mouse_button, modifiers );
    }
}



void glut_window_motion_handler( int x, int y )
{
    Window *window = window_get_active( );
    if ( (! window) || (!window->on_mouse_drag) )
    {
        return;
    }
    int dx = x - window->mouse_x;
    int dy = y - window->mouse_y;
    window->mouse_x = x;
    window->mouse_y = y;
    window->on_mouse_drag( window, x, y, dx, dy,
                           window->mouse_button, window->keyboard_modifiers);
}



void glut_window_passive_motion_handler( int x, int y )
{
    Window *window = window_get_active( );
    if ( (!window) || (!window->on_mouse_motion) )
    {
        return;
    }
    int dx = x - window->mouse_x;
    int dy = y - window->mouse_y;
    window->mouse_x = x;
    window->mouse_y = y;
    window->on_mouse_motion( window, x, y, dx, dy );                             
}



void glut_window_special_handler( int key, int x, int y )
{
    Window *window = window_get_active( );
    if ( (!window) || (!window->on_key_press) )
    {
        return;
    }
    int modifiers = glut_convert_key( glutGetModifiers() );
    window->keyboard_modifiers = modifiers;
    int symbol = glut_convert_key( key );
    window->on_key_press( window, symbol, modifiers );    
}



void glut_window_special_up_handler( int key, int x, int y )
{
    Window *window = window_get_active( );
    if ( (!window) || (!window->on_key_release) )
    {
        return;
    }
    int modifiers = glut_convert_key( glutGetModifiers() );
    window->keyboard_modifiers = modifiers;
    int symbol = glut_convert_key( key );
    window->on_key_release( window, symbol, modifiers );
}



void glut_window_keyboard_handler( unsigned char key, int x, int y )
{
    Window *window = window_get_active( );
    if (!window)
    {
        return;
    }
    int modifiers = glut_convert_key( glutGetModifiers() );
    window->keyboard_modifiers = modifiers;
    int symbol = glut_convert_key( key );
    if (window->on_key_press)
    {
        int status = window->on_key_press( window, symbol, modifiers );
        if ( status )
        {
            return;
        }
    }
    if (symbol == key_ESCAPE)
    {
        exit( 0 );
    }
}



void glut_window_keyboard_up_handler( unsigned char key, int x, int y )
{
    Window *window = window_get_active( );
    if ( (!window) || (!window->on_key_release) )
    {
        return;
    }
    int modifiers = glut_convert_key( glutGetModifiers() );
    window->keyboard_modifiers = modifiers;
    int symbol = glut_convert_key( key );
    window->on_key_release( window, symbol, modifiers );
}



void glut_window_entry_handler( int state )
{
    Window *window = window_get_active( );
    if (!window)
    {
        return;
    }
    if ( (state == GLUT_ENTERED) && (window->on_mouse_enter) )
    {
        window->on_mouse_enter( window, window->mouse_x, window->mouse_y );
    }
    else if ( (state == GLUT_LEFT) && (window->on_mouse_enter) )
    {
        window->on_mouse_leave( window, window->mouse_x, window->mouse_y );
    }
}



void glut_window_visibility_handler( int state )
{
    Window *window = window_get_active( );
    if (!window)
    {
        return;
    }
    if ( (state == GLUT_VISIBLE) && (window->on_show) )
    {
        window->on_show( window );
    }
    else if ( (state == GLUT_NOT_VISIBLE) && (window->on_hide) )
        {
        window->on_hide( window );
        }
}



void glut_window_idle_handler( void )
{
    Window *window = window_get_active( );
    if ( (!window) || (!window->on_idle) )
    {
        glutIdleFunc(0);
        return;
    }
    window->on_idle( window );
}




void glut_window_exit_handler( void )
{
    Window *window = window_get_active( );
    if ( (!window) || (!window->on_exit) )
    {
        exit(0);
    }
    window->on_exit( window );
    exit(0);
}



void glut_window_set_handlers( Window *window )
{
    if ( window->on_idle )
    {
        glutIdleFunc( glut_window_idle_handler );
    }
    else if ( (window->on_mouse_press) || (window->on_mouse_release) )
    {
        glutMouseFunc( glut_window_mouse_handler );
    }
    else if ( window->on_mouse_drag )
    {
        glutMouseFunc( glut_window_mouse_handler );
        glutMotionFunc( glut_window_motion_handler );
    }
    else if ( window->on_mouse_motion )
    {
        glutPassiveMotionFunc( glut_window_passive_motion_handler );
    }
    else if ( window->on_key_press )
    {
        glutSpecialFunc( glut_window_special_handler );
    }
    else if ( window->on_key_release )
    {
        glutKeyboardUpFunc( glut_window_keyboard_up_handler );
        glutSpecialUpFunc( glut_window_special_up_handler );
    }
    else if ((window->on_mouse_enter) || (window->on_mouse_leave))
    {
        glutEntryFunc( glut_window_entry_handler );
    }
    else if ((window->on_show) || (window->on_hide))
    {
        glutVisibilityFunc( glut_window_visibility_handler );
    }
    else if (window->on_exit)
    {
        atexit( glut_window_exit_handler );
    }
}



int glut_convert_modifiers( int modifiers )
{
    int modifiers_ = 0;
    if (modifiers & GLUT_ACTIVE_SHIFT)
        modifiers_ |= key_MOD_SHIFT;
    if (modifiers & GLUT_ACTIVE_CTRL)
        modifiers_ |= key_MOD_CTRL;
    if (modifiers & GLUT_ACTIVE_ALT)
        modifiers_ |= key_MOD_ALT;
    return modifiers_;
}



int glut_convert_key( unsigned char code )
{
    int ascii = tolower((int)code);
    if (((0x020 <= ascii) && (ascii <= 0x040)) ||
        ((0x05b <= ascii) && (ascii <= 0x07e)))
    {
        return ascii;
    }
    else if (ascii < 0x020)
    {
        if (ascii == 0x008)           return key_BACKSPACE;
        else if (ascii == 0x009)      return key_TAB;
        else if (ascii == 0x00A)      return key_LINEFEED;
        else if (ascii == 0x00C)      return key_CLEAR;
        else if (ascii == 0x00D)      return key_RETURN;
        else if (ascii == 0x018)      return key_CANCEL;
        else if (ascii == 0x01B)      return key_ESCAPE;
    }
    else if (code==GLUT_KEY_F1)       return key_F1;
    else if (code==GLUT_KEY_F2)       return key_F2;
    else if (code==GLUT_KEY_F3)       return key_F3;
    else if (code==GLUT_KEY_F4)       return key_F4;
    else if (code==GLUT_KEY_F5)       return key_F5;
    else if (code==GLUT_KEY_F6)       return key_F6;
    else if (code==GLUT_KEY_F7)       return key_F7;
    else if (code==GLUT_KEY_F8)       return key_F8;
    else if (code==GLUT_KEY_F9)       return key_F9;
    else if (code==GLUT_KEY_F10)      return key_F10;
    else if (code==GLUT_KEY_F11)      return key_F11;
    else if (code==GLUT_KEY_F12)      return key_F12;
    else if (code==GLUT_KEY_LEFT)     return key_LEFT;
    else if (code==GLUT_KEY_UP)       return key_UP;
    else if (code==GLUT_KEY_RIGHT)    return key_RIGHT;
    else if (code==GLUT_KEY_DOWN)     return key_DOWN;
    else if (code==GLUT_KEY_PAGE_UP)  return key_PAGEUP;
    else if (code==GLUT_KEY_PAGE_DOWN)return key_PAGEDOWN;
    else if (code==GLUT_KEY_HOME)     return key_HOME;
    else if (code==GLUT_KEY_END)      return key_END;
    else if (code==GLUT_KEY_INSERT)   return key_INSERT;
    return 0;
}



int glut_convert_button( int button )
{
    int button_ = mouse_NONE;
    if (button == GLUT_LEFT_BUTTON)
        button_ = mouse_LEFT;
    else if (button == GLUT_MIDDLE_BUTTON)
        button_ = mouse_MIDDLE;
    else if (button == GLUT_RIGHT_BUTTON)
        button_ = mouse_RIGHT;
    return button_;
}


//========================================================================
// main()
//========================================================================
#ifdef  __MAIN__
void on_draw( Window *window )
{
    window->clear( );
}
int main( int argc, char **argv)
{
    Window *window = window_new( 400, 400, "Window", 100.0 );
    window->on_draw = on_draw;
    window->run();
}
#endif /* __MAIN__ */
