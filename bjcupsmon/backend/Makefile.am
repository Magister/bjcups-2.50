## Process this file with automake to produce Makefile.in

backend_bindir = /usr/lib/cups/backend
backend_bin_PROGRAMS = canon_parallel canon_usb

canon_parallel_SOURCES = canon_backend_common.c canon_parallel.c
canon_usb_SOURCES = canon_backend_common.c canon_usb.c

CFLAGS = -O2 -Wall

canon_parallel_LDADD = -lcups
canon_usb_LDADD = -lcups

