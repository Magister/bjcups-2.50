## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_LDFLAGS = \
        $(GTK_LIBS)

AM_CPPFLAGS = \
        -I$(top_srcdir)/intl \
        -I$(top_srcdir)/lgmon \
        $(GTK_CFLAGS) \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\"

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = stsmon

stsmon_SOURCES =  \
	callbacks.c \
	callbacks.h \
	interface.c \
	interface.h \
	main.c \
	smstat.c \
	smstat.h \
	support.c \
	support.h

stsmon_LDADD = 

