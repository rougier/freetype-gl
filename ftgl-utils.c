/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#include "ftgl-utils.h"

error_callback_t log_error = error_callback_default;

// ------------------------------------------------- error_callback_default ---
void
error_callback_default(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

// ----------------------------------------------------- set_error_callback ---
void
set_error_callback(error_callback_t error_cb)
{
    log_error = error_cb;
}
