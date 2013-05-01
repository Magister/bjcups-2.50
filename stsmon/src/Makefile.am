## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	-I$(top_srcdir)/lgmon \
	@GTK_CFLAGS@

bin_PROGRAMS = stsmon

stsmon_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	smstat.c smstat.h 


stsmon_LDADD = @GTK_LIBS@ $(INTLLIBS)

