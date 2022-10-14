bin_PROGRAMS      = linuxinfo
linuxinfo_SOURCES = linuxinfo.c linuxinfo.h \
		    linuxinfo_common.c linuxinfo_arm.c \
		    linuxinfo_alpha.c linuxinfo_ia64.c linuxinfo_intel.c \
		    linuxinfo_m68k.c linuxinfo_ppc.c linuxinfo_sh.c \
		    linuxinfo_hppa.c linuxinfo_s390.c linuxinfo_avr.c \
		    linuxinfo_sparc.c linuxinfo_mips.c linuxinfo_unknown.c
man_MANS          = po4a/linuxinfo.1
EXTRA_DIST        = config.rpath CREDITS
VERSION	          = 3.3.3
SUBDIRS           = po po4a
PCRE_LINK         = @pcre_lib@

ACLOCAL_AMFLAGS   = -I m4
AM_CPPFLAGS       = -DLOCALEDIR=\"$(localedir)\"
linuxinfo_LDADD = $(LIBINTL) $(PCRE_LINK)
#linuxinfo_LDFLAGS = $(PCRE_LINK)

dist-hook:
	rm $(distdir)/po/stamp-po
	rm -rf `find $(distdir)/po -type f -name *.gmo`
