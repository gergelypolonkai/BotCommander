#! /bin/sh

aclocal || exit 1
automake --add-missing --copy || exit 1
autoconf || exit 1

exit 0

