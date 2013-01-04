#! /bin/sh

mkdir m4 &> /dev/null
autoheader || exit 1
aclocal || exit 1
automake --add-missing --copy || exit 1
autoconf || exit 1

exit 0

