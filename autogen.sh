#! /bin/sh -e
test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

autoreconf --force --install --verbose --warnings=all "$srcdir"
echo "Running intltoolize --copy --force --automake"
intltoolize --copy --force --automake
test -n "$NOCONFIGURE" || "$srcdir/configure" "$@"

#autoreconf --force --install || exit 1
#intltoolize --force || exit 1