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
void window_clear( Window *self );
void window_run( Window *self );
void window_draw( Window *self );


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


#define mouse_NONE   (1 << 0)
#define mouse_LEFT   (1 << 1)
#define mouse_MIDDLE (1 << 2)
#define mouse_RIGHT  (1 << 3)


#define key_MOD_SHIFT       (1 << 0)
#define key_MOD_CTRL        (1 << 1)
#define key_MOD_ALT         (1 << 2)
#define key_MOD_CAPSLOCK    (1 << 3)
#define key_MOD_NUMLOCK     (1 << 4)
#define key_MOD_WINDOWS     (1 << 5)
#define key_MOD_COMMAND     (1 << 6)
#define key_MOD_OPTION      (1 << 7)
#define key_MOD_SCROLLLOCK  (1 << 8)

/*
 * Accelerator modifier:
 *   on Windows and Linux, this is MOD_CTRL
 *   on Mac OS X it's MOD_COMMAND
 */
#if defined(__APPLE__)
#    define key_MOD_ACCEL   (key_MOD_COMMAND)
#else
#    define key_MOD_ACCEL   (key_MOD_CTRL)
#endif

/*
 * Key symbol constants
 */

/*
 * ASCII commands
 */
#define key_BACKSPACE     (0xff08)
#define key_TAB           (0xff09)
#define key_LINEFEED      (0xff0a)
#define key_CLEAR         (0xff0b)
#define key_RETURN        (0xff0d)
#define key_ENTER         (0xff0d) /* synonym */
#define key_PAUSE         (0xff13)
#define key_SCROLLLOCK    (0xff14)
#define key_SYSREQ        (0xff15)
#define key_ESCAPE        (0xff1b)
#define key_SPACE         (0xff20)

/*
 * Cursor control and motion
 */
#define key_HOME          (0xff50)
#define key_LEFT          (0xff51)
#define key_UP            (0xff52)
#define key_RIGHT         (0xff53)
#define key_DOWN          (0xff54)
#define key_PAGEUP        (0xff55)
#define key_PAGEDOWN      (0xff56)
#define key_END           (0xff57)
#define key_BEGIN         (0xff58)

/*
 * Misc functions
 */
#define key_DELETE        (0xffff)
#define key_SELECT        (0xff60)
#define key_PRINT         (0xff61)
#define key_EXECUTE       (0xff62)
#define key_INSERT        (0xff63)
#define key_UNDO          (0xff65)
#define key_REDO          (0xff66)
#define key_MENU          (0xff67)
#define key_FIND          (0xff68)
#define key_CANCEL        (0xff69)
#define key_HELP          (0xff6a)
#define key_BREAK         (0xff6b)
#define key_MODESWITCH    (0xff7e)
#define key_SCRIPTSWITCH  (0xff7e)

/* 
 * Text motion constants: these are allowed to clash with key constants
 */
#define key_MOTION_UP                (key_UP)
#define key_MOTION_RIGHT             (key_RIGHT)
#define key_MOTION_DOWN              (key_DOWN)
#define key_MOTION_LEFT              (key_LEFT)
#define key_MOTION_NEXT_WORD         (1)
#define key_MOTION_PREVIOUS_WORD     (2)
#define key_MOTION_BEGINNING_OF_LINE (3)
#define key_MOTION_END_OF_LINE       (4)
#define key_MOTION_NEXT_PAGE         (key_PAGEDOWN)
#define key_MOTION_PREVIOUS_PAGE     (key_PAGEUP)
#define key_MOTION_BEGINNING_OF_FILE (5)
#define key_MOTION_END_OF_FILE       (6)
#define key_MOTION_BACKSPACE         (key_BACKSPACE)
#define key_MOTION_DELETE            (key_DELETE)

/* 
 * Number pad
 */
#define key_NUMLOCK       (0xff7f)
#define key_NUM_SPACE     (0xff80)
#define key_NUM_TAB       (0xff89)
#define key_NUM_ENTER     (0xff8d)
#define key_NUM_F1        (0xff91)
#define key_NUM_F2        (0xff92)
#define key_NUM_F3        (0xff93)
#define key_NUM_F4        (0xff94)
#define key_NUM_HOME      (0xff95)
#define key_NUM_LEFT      (0xff96)
#define key_NUM_UP        (0xff97)
#define key_NUM_RIGHT     (0xff98)
#define key_NUM_DOWN      (0xff99)
#define key_NUM_PRIOR     (0xff9a)
#define key_NUM_PAGE_UP   (0xff9a)
#define key_NUM_NEXT      (0xff9b)
#define key_NUM_PAGE_DOWN (0xff9b)
#define key_NUM_END       (0xff9c)
#define key_NUM_BEGIN     (0xff9d)
#define key_NUM_INSERT    (0xff9e)
#define key_NUM_DELETE    (0xff9f)
#define key_NUM_EQUAL     (0xffbd)
#define key_NUM_MULTIPLY  (0xffaa)
#define key_NUM_ADD       (0xffab)
#define key_NUM_SEPARATOR (0xffac)
#define key_NUM_SUBTRACT  (0xffad)
#define key_NUM_DECIMAL   (0xffae)
#define key_NUM_DIVIDE    (0xffaf)

#define key_NUM_0         (0xffb0)
#define key_NUM_1         (0xffb1)
#define key_NUM_2         (0xffb2)
#define key_NUM_3         (0xffb3)
#define key_NUM_4         (0xffb4)
#define key_NUM_5         (0xffb5)
#define key_NUM_6         (0xffb6)
#define key_NUM_7         (0xffb7)
#define key_NUM_8         (0xffb8)
#define key_NUM_9         (0xffb9)

// Function keys
#define key_F1            (0xffbe)
#define key_F2            (0xffbf)
#define key_F3            (0xffc0)
#define key_F4            (0xffc1)
#define key_F5            (0xffc2)
#define key_F6            (0xffc3)
#define key_F7            (0xffc4)
#define key_F8            (0xffc5)
#define key_F9            (0xffc6)
#define key_F10           (0xffc7)
#define key_F11           (0xffc8)
#define key_F12           (0xffc9)
#define key_F13           (0xffca)
#define key_F14           (0xffcb)
#define key_F15           (0xffcc)
#define key_F16           (0xffcd)

/*
 * Modifiers
 */
#define key_LSHIFT        (0xffe1)
#define key_RSHIFT        (0xffe2)
#define key_LCTRL         (0xffe3)
#define key_RCTRL         (0xffe4)
#define key_CAPSLOCK      (0xffe5)
#define key_LMETA         (0xffe7)
#define key_RMETA         (0xffe8)
#define key_LALT          (0xffe9)
#define key_RALT          (0xffea)
#define key_LWINDOWS      (0xffeb)
#define key_RWINDOWS      (0xffec)
#define key_LCOMMAND      (0xffed)
#define key_RCOMMAND      (0xffee)
#define key_LOPTION       (0xffd0)
#define key_ROPTION       (0xffd1)

/*
 * Latin-1
 */
#define key_EXCLAMATION   (0x021)
#define key_DOUBLEQUOTE   (0x022)
#define key_HASH          (0x023)
#define key_POUND         (0x023)  /* synonym */
#define key_DOLLAR        (0x024)
#define key_PERCENT       (0x025)
#define key_AMPERSAND     (0x026)
#define key_APOSTROPHE    (0x027)
#define key_PARENLEFT     (0x028)
#define key_PARENRIGHT    (0x029)
#define key_ASTERISK      (0x02a)
#define key_PLUS          (0x02b)
#define key_COMMA         (0x02c)
#define key_MINUS         (0x02d)
#define key_PERIOD        (0x02e)
#define key_SLASH         (0x02f)
#define key__0            (0x030)
#define key__1            (0x031)
#define key__2            (0x032)
#define key__3            (0x033)
#define key__4            (0x034)
#define key__5            (0x035)
#define key__6            (0x036)
#define key__7            (0x037)
#define key__8            (0x038)
#define key__9            (0x039)
#define key_COLON         (0x03a)
#define key_SEMICOLON     (0x03b)
#define key_LESS          (0x03c)
#define key_EQUAL         (0x03d)
#define key_GREATER       (0x03e)
#define key_QUESTION      (0x03f)
#define key_AT            (0x040)
#define key_BRACKETLEFT   (0x05b)
#define key_BACKSLASH     (0x05c)
#define key_BRACKETRIGHT  (0x05d)
#define key_ASCIICIRCUM   (0x05e)
#define key_UNDERSCORE    (0x05f)
#define key_GRAVE         (0x060)
#define key_QUOTELEFT     (0x060)
#define key_A             (0x061)
#define key_B             (0x062)
#define key_C             (0x063)
#define key_D             (0x064)
#define key_E             (0x065)
#define key_F             (0x066)
#define key_G             (0x067)
#define key_H             (0x068)
#define key_I             (0x069)
#define key_J             (0x06a)
#define key_K             (0x06b)
#define key_L             (0x06c)
#define key_M             (0x06d)
#define key_N             (0x06e)
#define key_O             (0x06f)
#define key_P             (0x070)
#define key_Q             (0x071)
#define key_R             (0x072)
#define key_S             (0x073)
#define key_T             (0x074)
#define key_U             (0x075)
#define key_V             (0x076)
#define key_W             (0x077)
#define key_X             (0x078)
#define key_Y             (0x079)
#define key_Z             (0x07a)
#define key_BRACELEFT     (0x07b)
#define key_BAR           (0x07c)
#define key_BRACERIGHT    (0x07d)
#define key_ASCIITILDE    (0x07e)

#endif /* __WINDOW_H__ */
