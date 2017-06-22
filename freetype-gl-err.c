/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */

#include <stdio.h>
#include <stdarg.h>
#include "freetype-gl-err.h"

__THREAD int freetype_gl_errno=0;
__THREAD char * freetype_gl_message=NULL;

#ifdef __ANDROID__
#include <android/log.h>
#define  LOG_TAG    "freetype-gl"
void freetype_gl_errhook_default(int errno, char* message, char* fmt, ...)
{
  va_list myargs;
  va_start(myargs, fmt);
  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
		      "Freetype GL Error %03x %s:\n", errno, message);
  __android_log_vprint(ANDROID_LOG_ERROR, LOG_TAG,
		       fmt, myargs);
  va_end(myargs);
}
#else
void freetype_gl_errhook_default(int errno, char* message, char* fmt, ...)
{
  va_list myargs;
  va_start(myargs, fmt);
  fprintf(stderr, "Freetype GL Error %03x %s:\n", errno, message);
  vfprintf(stderr, fmt, myargs);
  va_end(myargs);
}
#endif

extern const struct {
    int          code;
    const char*  message;
} FT_Errors[];

char* freetype_gl_errstr(int errno)
{
  if(errno >= FTGL_ERR_BASE)
    return freetype_gl_errstrs[errno-FTGL_ERR_BASE];
  else
    return (char*)FT_Errors[errno].message;
}

void (*freetype_gl_errhook)(int errno, char* message, char* fmt, ...) = freetype_gl_errhook_default;

#undef FTGL_ERRORDEF_
#define FTGL_ERRORDEF_( e, v, s ) s,
#undef __FREETYPE_GL_ERRORS_H__
#undef FTGL_ERROR_START_LIST
#undef FTGL_ERROR_END_LIST

#define FTGL_ERROR_START_LIST char* freetype_gl_errstrs[] = {
#define FTGL_ERROR_END_LIST };

#include "freetype-gl-errdef.h"

