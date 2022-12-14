# BEGIN COPYRIGHT BLOCK
# Copyright (C) 2007 Red Hat, Inc.
#               2018 Helge Kreutzmann
# All rights reserved.
#
# License: GPL (version 3 or any later version).
# See LICENSE for details. 
# END COPYRIGHT BLOCK

AC_CHECKING(for PCRE)

dnl  - check for --with-pcre
AC_MSG_CHECKING(for --with-pcre)
AC_ARG_WITH(pcre, AS_HELP_STRING([--with-pcre@<:@=PATH@:>@],[Perl Compatible Regular Expression directory]),
[
  if test "$withval" = "yes"; then
    AC_MSG_RESULT(yes)
  elif test "$withval" = "no"; then
    AC_MSG_RESULT(no)
    AC_MSG_RESULT([not using PCRE])
    with_pcre=no
  elif test -d "$withval"/include -a -d "$withval"/lib; then
    AC_MSG_RESULT([using $withval])
    dnl - check the user provided location
    PCREDIR=$withval
    pcre_lib="-L$PCREDIR/lib"
    pcre_libdir="$PCREDIR/lib"
    pcre_incdir="$PCREDIR/include"
    if ! test -e "$pcre_incdir/pcre.h" ; then
      AC_MSG_ERROR([$withval include dir not found])
    fi
    pcre_inc="-I$pcre_incdir"
    AC_DEFINE([use_regexp])
   else
    echo
    AC_MSG_ERROR([$withval not found])
  fi
],
[with_pcre=no
 AC_MSG_RESULT(no)])

if test "x$with_pcre" != "xno"; then
#
# if PCRE is not found yet, try pkg-config
if test -z "$pcre_inc" -o -z "$pcre_lib" -o -z "$pcre_libdir"; then
  AC_PATH_PROG(PKG_CONFIG, pkg-config)
  AC_MSG_CHECKING(for pcre with pkg-config)
  if test -n "$PKG_CONFIG"; then
    if $PKG_CONFIG --exists pcre; then
      pcre_inc=`$PKG_CONFIG --cflags-only-I pcre`
      pcre_lib=`$PKG_CONFIG --libs-only-L pcre`
      pcre_libdir=`$PKG_CONFIG --libs-only-L pcre | sed -e s/-L// | sed -e s/\ .*$//`
      AC_MSG_RESULT([using system PCRE])
      AC_DEFINE([use_regexp])
    elif $PKG_CONFIG --exists libpcre2-8; then
      pcre_inc=`$PKG_CONFIG --cflags libpcre2-8`
      pcre_lib=`$PKG_CONFIG --libs libpcre2-8`
      pcre_libdir=`$PKG_CONFIG --libs-only-L libpcre | sed -e s/-L// | sed -e s/\ .*$//`
      AC_MSG_RESULT([using system LIBPCRE 2.8])
      AC_DEFINE([use_regexp])
    else
      AC_MSG_RESULT([PCRE not found, not using PCRE or specify with --with-pcre.])
    fi
  fi
fi

dnl last resort
dnl - check in system locations
if test -z "$pcre_lib"; then
  AC_MSG_CHECKING(for pcre.h)
  if test -f "/usr/include/pcre/pcre.h"; then
    AC_MSG_RESULT([using /usr/include/pcre/pcre.h])
    AC_DEFINE([use_regexp])
    pcre_incdir="/usr/include/pcre"
    pcre_inc="-I/usr/include/pcre"
    pcre_lib='-L$(libdir)'
    pcre_libdir='$(libdir)'
  elif test -f "/usr/include/pcre.h"; then
    AC_MSG_RESULT([using /usr/include/pcre.h])
    AC_DEFINE([use_regexp])
    pcre_incdir="/usr/include"
    pcre_inc="-I/usr/include"
    pcre_lib='-L$(libdir)'
    pcre_libdir='$(libdir)'
  else
    AC_MSG_RESULT(no)
    AC_MSG_RESULT([not using PCRE])
  fi
fi
fi
