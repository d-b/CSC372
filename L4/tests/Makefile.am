CFLAGS = --pedantic -Wall -std=c99

TESTS = check_lab4
check_PROGRAMS = check_lab4
check_lab4_SOURCES = check_lab4.c check_lab4_lifo.c check_lab4_priority.c check_lab4_waitlist.c
check_lab4_CFLAGS = @CHECK_CFLAGS@
check_lab4_LDADD = $(top_builddir)/src/liblab4.la @CHECK_LIBS@
