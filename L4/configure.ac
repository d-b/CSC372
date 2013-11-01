#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([lab3], [0.1])
AC_CONFIG_SRCDIR([src/list.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE

# Add libtool
LT_INIT

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Processor detection
case "$host_cpu" in
    x86*)
        AC_DEFINE([PLATFORM_X86], [1], [x86 CPU platform])
        AM_CONDITIONAL([PLATFORM_X86], [true])
        AM_CONDITIONAL([PLATFORM_ARM], [false])
        ;;
    arm*)
        AC_DEFINE([PLATFORM_ARM], [1], [ARM CPU platform])
        AM_CONDITIONAL([PLATFORM_X86], [false])
        AM_CONDITIONAL([PLATFORM_ARM], [true])
        ;;
    *)
        AM_CONDITIONAL([PLATFORM_X86], [false])
        AM_CONDITIONAL([PLATFORM_ARM], [false])
        ;;
esac

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
