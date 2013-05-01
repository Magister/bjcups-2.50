bin_PROGRAMS=bjcups

bjcups_SOURCES=\
	bjcups.c getipc.c bjutil.c \
	bjutil.h

bjcups_LDADD= -lpopt -lcups -lparamlist 
bjcups_LDFLAGS = -L../../libs/paramlist

AM_CFLAGS= -O2 -Wall -I../include/cncl -I../include/misc -I../../libs/paramlist -I../../libs/bjexec

