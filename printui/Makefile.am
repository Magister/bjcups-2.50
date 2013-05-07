## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = po src cmd include

INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = \
	$(INTLTOOL_FILES) \
	$(PIXMAPS_DATA)

DISTCLEANFILES = \
        intltool-extract.* \
        intltool-merge.* \
        intltool-update.* \
        po/.intltool-merge-cache

PIXMAPSdir = $(pkgdatadir)/pixmaps

PIXMAPS_DATA = \
        pixmaps/*.xpm
