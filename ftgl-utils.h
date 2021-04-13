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

 /**
  * This function sets
  *
  * @param atlas     A texture atlas
  * @param pt_size   Size of font to be created (in points)
  * @param filename  A font filename
  *
  * @return A new empty font (no glyph inside yet)
  *
  */
typedef void (*error_callback_t) (const char *fmt, ...);
extern error_callback_t log_error;
void
error_callback_default(const char *fmt, ...);
void
ftgl_set_error_callback(error_callback_t error_callback);

#ifdef __cplusplus
}
}
#endif

#endif
