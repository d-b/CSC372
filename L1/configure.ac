#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Add our profile packages
export PKG_CONFIG_PATH=$HOME/.local/lib/pkgconfig

AC_PREREQ([2.59])
AC_INIT([lab1], [0.1])
AC_CONFIG_SRCDIR([src/list.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

# Add libtool
LT_INIT

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
