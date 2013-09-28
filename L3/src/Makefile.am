AM_CFLAGS = --pedantic -Wall -std=c99

lib_LTLIBRARIES = liblab3.la
liblab3_la_SOURCES = kernel.c exception.c list.c user.c

bin_PROGRAMS = lab3
lab3_SOURCES = main.c
lab3_LDADD = liblab3.la
