MAINTAINERCLEANFILES=Makefile.in
sbin_PROGRAMS=ircd
ircd_SOURCES=main.c config.c config_logging.c config_general.c logging.c
ircd_LDFLAGS=-export-dynamic
ircd_CFLAGS=-std=c99 -I$(top_srcdir)/include -I$(top_srcdir)/libevent/include -I$(top_srcdir)/json-c
INCLUDES=
ircd_LDADD=$(top_srcdir)/libevent/libevent.la $(top_srcdir)/json-c/libjson.la
AM_YFLAGS=-d
