#!/bin/sh -e
test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

set -e
set -x
test -d m4 || mkdir -p m4
${AUTORECONF-autoreconf} --force --install --verbose "$srcdir"
${ACLOCAL-aclocal} -I m4
${AUTOHEADER-autoheader}
${AUTOMAKE-automake} --add-missing --copy --force-missing
${AUTOCONF-autoconf}
test -n "$NOCONFIGURE" || "$srcdir/configure" "$@"
