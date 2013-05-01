## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	$(LIBXML_CFLAGS) \
	$(GTK__CFLAGS)

AM_LDFLAGS = \
	$(LIBXML_LIBS) \
	$(GTK__LIBS)

CPPFLAGS = \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\"

CFLAGS += -DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED \
        -DGDK_PIXBUF_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED \
        -DGSEAL_ENABLE \
        -DG_DISABLE_SINGLE_INCLUDES -DATK_DISABLE_SINGLE_INCLUDES \
        -DGDK_PIXBUF_DISABLE_SINGLE_INCLUDES -DGTK_DISABLE_SINGLE_INCLUDES


bin_PROGRAMS =  \
	bjcupsmon

bjcupsmon_SOURCES =  \
	bjcupsmon_callbacks.c \
	bjcupsmon_common.h \
	bjcupsmon_cups.c \
	bjcupsmon_cups.h \
	bjcupsmon_draw.c \
	bjcupsmon_draw.h \
	bjcupsmon_err.h \
	bjcupsmon_funcprot.h \
	bjcupsmon_main.c \
	bjcupsmon_msgidset.c \
	bjcupsmon_transsts.c \
	bjcupsmon_transsts.h \
	bjcupsmon_ui.c \
	bjcupsmon_ui.h \
	interface.c \
	interface.h \
	support.c \
	support.h

bjcupsmon_LDADD = -lcups

