AM_CFLAGS = --pedantic -Wall -std=c99

lib_LTLIBRARIES = liblab4.la
liblab4_la_SOURCES = kernel.c exception.c irq.c list.c user.c debug.c ringbuffer.c audio.c

bin_PROGRAMS = lab4
lab4_SOURCES = main.c
lab4_LDADD = liblab4.la

if PLATFORM_ARM
    AM_CFLAGS += -I$(top_srcdir)/rpi/include -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
    AM_LDFLAGS = -L/opt/vc/lib
    lab4_LDADD += -lbcm_host -lopenmaxil
    lab4_LDADD += $(top_srcdir)/rpi/lib/audio.a
endif
