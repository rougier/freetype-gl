## ExampleLib Example: an example of using Automake to link with a library

AC_INIT([freetype-gl], [1.3], [bernd@net2o.de], [freetype-gl for Linux/Android],
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
	LIBADD="-llog"
	LIBGL=GLESv2
	;;
     *)
	LIBGL=GL
	;;
esac

>conftest.$ac_ext
FTINC=`eval $ac_compiler -E -Wp,-v conftest.$ac_ext 2>&1 | grep '^ ' | while read i; do test -f $i/freetype2/ft2build.h && echo -I$i/freetype2; done`
rm conftest.$ac_ext

SOURCE_PATH_SIZE=$(echo $PWD | wc -c)

AC_SUBST(FTINC)
AC_SUBST(LIBADD)
AC_SUBST(LIBGL)
AC_SUBST(SOURCE_PATH_SIZE)

PRECC=${CC%gcc*}
PRECLANG=${PRECC%clang*}
AC_CHECK_PROGS(GNU_LIBTOOL,"glibtool --tag=CC" libtool)
AC_SUBST(LIBTOOL)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([debian/control])
AC_CONFIG_FILES([debian/changelog])
AC_OUTPUT
