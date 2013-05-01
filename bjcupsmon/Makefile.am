## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DPACKAGE_LOCALEDIR=\""$(PACKAGE_LOCALEDIR)"\"

AM_CFLAGS =\
	 -Wall\
	 -g


SUBDIRS = \
	backend \
	src \
	po

INTLTOOL_FILES = \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = \
	$INTLTOOL_FILES

DISTCLEANFILES = \
	intltool-extract.* \
	intltool-merge.* \
	intltool-update.* \
	po/.intltool-merge-cache
