## ExampleLib Example: an example of using Automake to link with a library

AC_INIT([freetype-gl], [1.0], [bernd@net2o.de], [freetype-gl for Linux/Android],
        [http://lonesock.net/soil.html])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CC
AM_PROG_AR
LT_INIT

case $CC in
     *android*)
	LIBADD="-llog -lglib-android-1.0"
	LIBGL=GLESv2
	;;
     *)
	LIBGL=GL
	;;
esac
AC_SUBST(LIBADD)
AC_SUBST(LIBGL)
LIBTOOL=${CC%gcc}libtool
AC_SUBST(LIBTOOL)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
