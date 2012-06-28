dnl Inspired by work Copyright (C) 2006 Luca Filipozzi
dnl vim: set fdm=marker sw=2 ts=2 et si:
dnl {{{ ac_define_dir
dnl http://autoconf-archive.cryp.to/ac_define_dir.html
AC_DEFUN([AC_DEFINE_DIR], [
  prefix_NONE=
  exec_prefix_NONE=
  test "x$prefix" = xNONE && prefix_NONE=yes && prefix=$ac_default_prefix
  test "x$exec_prefix" = xNONE && exec_prefix_NONE=yes && exec_prefix=$prefix
  eval ac_define_dir="\"[$]$2\""
  eval ac_define_dir="\"$ac_define_dir\""
  AC_SUBST($1, "$ac_define_dir")
  AC_DEFINE_UNQUOTED($1, "$ac_define_dir", [$3])
  test "$prefix_NONE" && prefix=NONE
  test "$exec_prefix_NONE" && exec_prefix=NONE
])dnl }}}
]) dnl }}}
dnl {{{ ax_arg_enable_debugging
AC_DEFUN([AX_ARG_ENABLE_DEBUGGING],[
  AC_ARG_ENABLE([debugging],[AC_HELP_STRING([--enable-debugging],[Enable debugging.])],[debugging="$enableval"],[debugging="no"])
  if test "$debugging" = "yes" ; then
    AC_DEFINE([DEBUG],[1],[Define to 1 to enable debugging.])
    CFLAGS="$CFLAGS -Wall -g -O0"
  else
    AC_DEFINE([NDEBUG],[1],[Define to 1 to disable debugging.])
  fi
])dnl }}}
