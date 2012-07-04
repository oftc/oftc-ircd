# Inspired by work Copyright (C) 2006 Luca Filipozzi
# vim: set fdm=marker ts=2 sw=2 et:

AC_PREREQ(2.59)
AC_INIT([oftc-ircd],[1.0.0])
AM_INIT_AUTOMAKE([oftc-ircd],[1.0.0])
AC_CONFIG_HEADERS([include/setup.h])
AM_MAINTAINER_MODE

#ltdl stuff

# Checks for programs.
AC_PROG_INSTALL
AC_GNU_SOURCE
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"
AC_PROG_AWK
AC_PROG_EGREP
AC_PATH_PROG([TAIL],[tail])
AM_PROG_CC_C_O
AX_ARG_ENABLE_DEBUGGING

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([sys/resource.h]) # ick

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Argument processing.

AC_DEFINE_DIR([PREFIX],[prefix],[Set to prefix.])
AC_DEFINE_DIR([SYSCONFDIR],[sysconfdir],[Set to sysconfdir.])
AC_DEFINE_DIR([LIBDIR],[libdir],[Set to libdir.])
AC_DEFINE_DIR([DATADIR],[datadir],[Set to datadir.])
AC_DEFINE_DIR([LOCALSTATEDIR],[localstatedir],[Set to localstatedir.])

AC_CONFIG_SUBDIRS(libevent)
#AC_CONFIG_SUBDIRS(jsoncpp)

AC_CONFIG_FILES(                \
       Makefile                 \
       include/Makefile         \
       src/Makefile             \
)

AC_OUTPUT
