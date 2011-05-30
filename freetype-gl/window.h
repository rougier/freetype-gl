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
#ifndef __WINDOW_H__
#define __WINDOW_H__

#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "key.h"
#include "mouse.h"


 
typedef struct __Window__ Window;

struct __Window__
{
    float fps;
    int width, height;
    int keyboard_key, keyboard_modifiers;
    int mouse_x, mouse_y, mouse_button;

    void (* run)              ( void );
    void (* clear )           ( void );
    void (* draw )            ( void );

    int  (* on_init )         (struct __Window__ * );
    int  (* on_exit )         (struct __Window__ * );
    int  (* on_show )         (struct __Window__ * );
    int  (* on_hide )         (struct __Window__ * );
    int  (* on_close )        (struct __Window__ * );
    int  (* on_move )         (struct __Window__ *, int, int );
    void (* on_resize)        (struct __Window__ *, int, int );
    int  (* on_expose )       (struct __Window__ * );
    void (* on_draw )         (struct __Window__ * );
    int  (* on_idle )         (struct __Window__ * );
    int  (* on_key_press   )  (struct __Window__ *, int, int );
    int  (* on_key_release )  (struct __Window__ *, int, int );
    int  (* on_mouse_press )  (struct __Window__ *, int, int, int, int );
    int  (* on_mouse_release) (struct __Window__ *, int, int, int, int );
    int  (* on_mouse_enter )  (struct __Window__ *, int, int );
    int  (* on_mouse_leave )  (struct __Window__ *, int, int );
    int  (* on_mouse_motion)  (struct __Window__ *, int, int, int, int );
    int  (* on_mouse_drag)    (struct __Window__ *, int, int, int, int, int, int );
    int  (* on_mouse_scroll)  (struct __Window__ *, int, int, int, int );
};

Window *window_new( int width, int height, char *title, float fps );
Window *window_get_active( void );


/* GLUT wrappers */
Window *glut_window_new( int width, int height, char *title, float fps );
void glut_window_clear( void );
void glut_window_draw( void );
void glut_window_run( void );
void glut_window_timer_handler( int value );
void glut_window_display_handler( void );
void glut_window_reshape_handler( int width, int height );
void glut_window_mouse_handler( int button, int state, int x, int y );
void glut_window_motion_handler( int x, int y );
void glut_window_passive_motion_handler( int x, int y );
void glut_window_special_handler( int key, int x, int y );
void glut_window_special_up_handler( int key, int x, int y );
void glut_window_keyboard_handler( unsigned char key, int x, int y );
void glut_window_keyboard_up_handler( unsigned char key, int x, int y );
void glut_window_entry_handler( int state );
void glut_window_visibility_handler( int state );
void glut_window_idle_handler( void );
void glut_window_exit_handler( void );
void glut_window_set_handlers( Window *window );
int  glut_convert_modifiers( int modifiers );
int  glut_convert_key( unsigned char code );
int  glut_convert_button( int button );


/* GLFW wrappers */
/*
Window *glfw_window_new( int width, int height, char *title );
void glfw_window_clear( void );
void glfw_window_draw( void );
void glfw_window_run( void );
void glfw_window_display_handler( void );
void glfw_window_reshape_handler( int width, int height );
void glfw_window_key_handler( int key, int action );
void glfw_window_char_handler( int character, int action );
void glfw_window_mouse_pos( int x, int y );
void glfw_window_mouse_wheel( int pos );
void glfw_window_set_handlers( Window *window );
int  glfw_convert_modifiers( int modifiers );
int  glfw_convert_key( unsigned char code );
int  glfw_convert_button( int button );
*/

#endif /* __WINDOW_H__ */
