PACKAGE=docklight
VERSION=5.2.64
INSTALLDIR=/usr/local/bin
LOCALEDIR=/usr/local/bin/docklight-5/locale

AC_INIT([docklight], [5.1.64], [bug-automake@gnu.org])
# this flags are for automake
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects foreign])
# find & probe c compiler
AC_PROG_CC
# find & probe g++ compiler
AC_PROG_CXX
# make config headers
AC_CONFIG_HEADERS([config.h])

AC_LANG(C++)
ax_cxx_compile_stdcxx_11AC_PROG_CXX
IT_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19])
PROGRAMNAME_LOCALEDIR=$LOCALEDIR
AC_SUBST(PROGRAMNAME_LOCALEDIR)

#------
AC_SUBST(LIBBAMF_VER, 3)
PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.38.0 gio-2.0 >= 2.30.0 gio-unix-2.0 gobject-2.0 gmodule-2.0)
AC_CHECK_LIB([bamf3], [main])
#----
#
AC_CONFIG_MACRO_DIR([m4])
# This checks for the presence of gtkmm-3.0 and defines
# GTKMM_LIBS and GTKMM_CFLAGS for use in Makefile.am files.
PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0 >= 3.8.0])
# This checks for the presence of libwnck-3.0  and defines
# WNCK_LIBS and WNCK_CFLAGS for use in Makefile.am files.
PKG_CHECK_MODULES([WNCK], [libwnck-3.0 ] )
# MM_PKG_CONFIG_SUBST([GTHREAD_LIBS], [--libs libwnck-3.0])
AC_PATH_PROG(PKG_CONFIG, pkg-config)
# check X11 this add -lX11 and substiture the X11_CFLAGS and X11_LIBS variables.
AC_CHECK_LIB([X11], [XOpenDisplay])
# check wnck-3 this add -lwnck-3 and substiture the wnck-3_CFLAGS and wnck-3_LIBS variables.
AC_CHECK_LIB([wnck-3],[wnck_screen_get_default])
# AC_CHECK_LIB([pthreads])
AC_CHECK_LIB(pthread, pthread_create, [PTHREAD_LIBS+=-lpthread])
# Checks for extra libraries needed
AC_CHECK_LIB([stdc++], [main])
# For applications using autoconf the standard macro AC_CHECK_LIB can be used to link with GSL #automatically from a configure script. The library itself depends on the presence of a CBLAS #and math library as well, so these must also be located before linking with the main libgsl #file. The following commands should be placed in the configure.ac file to perform these #tests,
# checking for cos in -lm... yes  math funcs
AC_CHECK_LIB([m],[cos])
LIBS="$PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CC="$PTHREAD_CC"

# configure create makefiles
AC_CONFIG_FILES([
 Makefile
 data/Makefile
 data/icons/Makefile
 po/Makefile.in
 src/Makefile
])

AC_OUTPUT
echo "
    ${PACKAGE} ${VERSION}
    Prefix......................:  ${prefix}
    Compiler....................:  ${CXX}
    C++ Compiler Flags..........:  ${CFLAGS}
    Libs........................:  ${LIBS}
"
