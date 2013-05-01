## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
        -I$(top_srcdir)/src

bin_PROGRAMS =  \
	lgmon

lgmon_SOURCES =  \
	bscc2sts.h \
	langmon.c \
	lm.h \
	lm_print.c \
	lmsmsock.h \
	lm_status.c \
	lm_sub.c

lgmon_LDADD = libbscc2sts.a 

noinst_LIBRARIES =  \
	libbscc2sts.a

libbscc2sts_a_SOURCES =  \
        bscc2sts.c \
        commonfunc.c \
        f9xxfunc.c \
        s300func.c \
        s500func.c \
        550ifunc.c \
        850ifunc.c \
        950ifunc.c \
        560ifunc.c \
        860ifunc.c \
        990ifunc.c \
        ip1000func.c \
        ip3100func.c \
        ip4100func.c \
        ip8600func.c \
        f9xxtbl.c \
        s300tbl.c \
        s500tbl.c \
        550itbl.c \
        850itbl.c \
        950itbl.c \
        560itbl.c \
        860itbl.c \
        990itbl.c \
        ip1000tbl.c \
        ip3100tbl.c \
        ip4100tbl.c \
        ip8600tbl.c \
        bscc2sts.h \
        commonfunc.h
