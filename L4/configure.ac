#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([lab3], [0.1])
AC_CONFIG_SRCDIR([src/list.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

# Add libtool
LT_INIT

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Add our profile packages
export PKG_CONFIG_PATH=$HOME/.local/lib/pkgconfig

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
