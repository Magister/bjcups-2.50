filterdir=$(libdir)/cups/filter

filter_PROGRAMS= pstocanonbj

pstocanonbj_SOURCES= \
	pstocanonbj.c canonopt.c \
	canonopt.h paramlist.c \
	paramlist.h 

pstocanonbj_LDADD= -lcups ../../libs/buftool/libbuftool.a
pstocanonbj_LDFLAGS = -L../../libs/buftool

AM_CFLAGS= -O2 -Wall -I../../libs/buftool

