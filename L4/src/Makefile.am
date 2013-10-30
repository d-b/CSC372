AM_CFLAGS = --pedantic -Wall -std=c99

lib_LTLIBRARIES = liblab4.la
liblab4_la_SOURCES = kernel.c exception.c irq.c list.c user.c debug.c ringbuffer.c audio.c

bin_PROGRAMS = lab4
lab4_SOURCES = main.c
lab4_LDADD = liblab4.la
