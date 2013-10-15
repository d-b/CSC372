CFLAGS = --pedantic -Wall -std=c99

TESTS = check_lab3
check_PROGRAMS = check_lab3
check_lab3_SOURCES = check_lab3.c check_lab3_lifo.c check_lab3_priority.c check_lab3_waitlist.c
check_lab3_CFLAGS = @CHECK_CFLAGS@
check_lab3_LDADD = $(top_builddir)/src/liblab3.la @CHECK_LIBS@
