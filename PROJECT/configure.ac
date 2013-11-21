#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([modem], [1.0], [d.bloemendal@utoronto.ca])
AC_LANG([C++])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([build])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE

# Add libtool
LT_INIT

# Checks for programs.
AC_PROG_CXX
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

# Add package directories
export PKG_CONFIG_PATH=$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
PKG_CHECK_MODULES([FFTW3], [fftw3 >= 3.3.3])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_11

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
