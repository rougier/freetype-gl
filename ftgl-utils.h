/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#ifndef __FTGL_UTILS_H__
#define __FTGL_UTILS_H__
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
namespace ftgl {
#endif


typedef void (*error_callback_t) (const char *fmt, ...);
extern error_callback_t log_error;

/**
 * Prints input to stderr
 * This is fallback function for error reporting if ftgl_set_error_callback() wans't called
 *
 * @param fmt       cstring to be printed matching C-style printing syntax
 * @param ...       va_list fmt supplying arguments
 */
  void
  error_callback_default(const char *fmt, ...);

/**
 * Set function to call on error handling
 * This is fallback function for error reporting if ftgl_set_error_callback() wans't called
 *
 * @param error_cb  callback function to call on error, see error_callback_default for reference
 */
  void
  set_error_callback(error_callback_t error_cb);

#ifdef __cplusplus
}
}
#endif

#endif
