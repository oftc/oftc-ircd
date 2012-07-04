MAINTAINERCLEANFILES=Makefile.in
sbin_PROGRAMS=ircd
ircd_SOURCES=main.cc config.cc config_logging.cc logging.cc
ircd_LDFLAGS=-export-dynamic
ircd_CXXFLAGS=-I$(top_srcdir)/include -I$(top_srcdir)/libevent/include -I$(top_srcdir)/json-c
INCLUDES=
ircd_LDADD=$(top_srcdir)/libevent/libevent.la $(top_srcdir)/json-c/libjson.la
AM_YFLAGS=-d
