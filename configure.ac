dnl Process this file with autoconf to produce a configure script.
AC_INIT([Linuxinfo], [3.3.3])
AC_CONFIG_SRCDIR([linuxinfo.c])
AM_INIT_AUTOMAKE

dnl Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
LINUXINFO_PROG_PO4A

dnl Checks for libraries.
m4_include(m4/pcre.m4)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)

dnl Checks for library functions.
AC_CHECK_FUNCS(strstr uname)

AM_GNU_GETTEXT_VERSION([0.18.2])
AM_GNU_GETTEXT([external])

dnl Write out paths for PCRE
AC_SUBST(pcre_inc)
AC_SUBST(pcre_lib)
AC_SUBST(pcre_libdir)

AC_CONFIG_FILES([ Makefile po/Makefile.in po4a/Makefile ])
AC_OUTPUT
dnl AC_OUTPUT(Makefile po/Makefile.in)
