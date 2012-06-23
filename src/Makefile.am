MAINTAINERCLEANFILES=Makefile.in
sbin_PROGRAMS=ircd
ircd_SOURCES=main.c
ircd_LDFLAGS=-export-dynamic
ircd_CFLAGS=-I$(top_srcdir)/include -I$(top_srcdir)/libevent/include
INCLUDES=
ircd_LDADD=$(top_srcdir)/libevent/libevent.la
AM_YFLAGS=-d
