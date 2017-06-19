/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#ifndef __FREETYPE_GL_ERR_H__
#define __FREETYPE_GL_ERR_H__

#ifndef __THREAD
#if defined(__GNUC__) || defined(__clang__)
#define __THREAD __thread
#elif defined(_MSC_VER)
#define __THREAD __declspec( thread )
#else
#define __THREAD
#endif
#endif

#define __FILENAME__ (__FILE__ + SOURCE_PATH_SIZE)

#ifdef __cplusplus
extern "C" {
namespace ftgl {
#endif

/*********** public error API ***********/
/**
 * freetype_gl_errno    is the error number if a freetype-gl function fails
 *                      Errors < FTGL_ERR_BASE are pass-through from Freetype
 */
extern __THREAD int freetype_gl_errno;
/**
 * freetype_gl_message  is the error message if a freetype-gl function fails
 */
extern __THREAD char * freetype_gl_message;
/**
 * freetype_gl_errhook  is a function pointer to display error number
 *                      and message, default is using fprintf on stderr
 */
extern void (*freetype_gl_errhook)(int errno, char* message, char* string, ...);
/**
 * freetype_gl_errstr   converts an errno to the message (including FT_errors)
 */
extern char* freetype_gl_errstr(int errno);

#ifndef FTGL_ERR_PREFIX
# define FTGL_ERR_PREFIX  FTGL_Err_
#endif

#ifndef FTGL_ERR_CAT
# define FTGL_ERR_XCAT( x, y )  x ## y
# define FTGL_ERR_CAT( x, y )   FTGL_ERR_XCAT( x, y )
#endif
#define FTGL_ERR_BASE  0x100 /* Freetype GL errors start at 0x100 */
    
extern char* freetype_gl_errstrs[];

#define freetype_gl_error(errno, ...) {			     \
	freetype_gl_errno = FTGL_ERR_CAT( FTGL_ERR_PREFIX, errno);	\
	freetype_gl_message = freetype_gl_errstrs[FTGL_ERR_CAT( FTGL_ERR_PREFIX, errno)-FTGL_ERR_BASE]; \
	freetype_gl_errhook(freetype_gl_errno, freetype_gl_message, __VA_ARGS__); \
    }

#define freetype_error(error, ...) {	     \
	freetype_gl_errno = FT_Errors[error].code;			\
	freetype_gl_message = (char*)FT_Errors[error].message;		\
	freetype_gl_errhook(freetype_gl_errno, freetype_gl_message, __VA_ARGS__); \
    }

#define FTGL_ERRSTR_MAX 0x100

#ifndef FTGL_ERRORDEF_
# ifndef FTGL_ERRORDEF

#  define FTGL_ERRORDEF( e, v, s )  e = v,
#  define FTGL_ERROR_START_LIST     enum {
#  define FTGL_ERROR_END_LIST       FTGL_ERR_CAT( FTGL_ERR_PREFIX, Max ) };

#  ifdef __cplusplus
#   define FTGL_NEED_EXTERN_C
  extern "C" {
#  endif
    
# endif /* !FTGL_ERRORDEF */

    /* this macro is used to define an error */
# define FTGL_ERRORDEF_( e, v, s )						\
          FTGL_ERRORDEF( FTGL_ERR_CAT( FTGL_ERR_PREFIX, e ), v + FTGL_ERR_BASE, s )
# endif /* !FTGL_ERRORDEF_ */

#include <freetype-gl-errdef.h>
    
#ifdef __cplusplus
}
}
#endif

#endif /* FREETYPE_GL_ERR_H */
