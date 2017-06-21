# Automake file for libfreetype-gl

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

## Set the default command-line flags for the C preprocessor to the value
## obtained from pkg-config via PKG_CHECK_MODULES in configure.ac.  These
## flags are passed to the compiler for both C and C++, in addition to the
## language-specific options.
AM_CPPFLAGS = $(EXAMPLE_CFLAGS) @FTINC@ -DSOURCE_PATH_SIZE=@SOURCE_PATH_SIZE@

lib_LTLIBRARIES = libfreetype-gl.la
libfreetype_gl_la_SOURCES = texture-atlas.c texture-font.c vector.c utf8-utils.c freetype-gl-err.c
include_HEADERS = freetype-gl.h vec234.h vector.h texture-atlas.h texture-font.h opengl.h freetype-gl-err.h freetype-gl-errdef.h
libfreetype_gl_la_LIBADD = -lm -l@LIBGL@ -lfreetype -lz @LIBADD@

distclean-am: clean-am distclean-compile distclean-hdr distclean-libtool distclean-tags

install-exec-hook:
	-/sbin/ldconfig $(libdir)
