## Process this file with automake to produce Makefile.in


AM_CPPFLAGS = \
        -I$(top_srcdir)/include/cncl \
        -I$(top_srcdir)/include/misc \
        $(GTK_CFLAGS) \
        $(LIBXML_CFLAGS) \
        $(LIBGLADE_CFLAGS) \
        -DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
        -DPACKAGE_LOCALE_DIR=\""$(localedir)"\"

##        -DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED \
##        -DGDK_PIXBUF_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED \
##        -DGSEAL_ENABLE \
##        -DG_DISABLE_SINGLE_INCLUDES -DATK_DISABLE_SINGLE_INCLUDES \
##        -DGDK_PIXBUF_DISABLE_SINGLE_INCLUDES -DGTK_DISABLE_SINGLE_INCLUDES \
CFLAGS += \
        -DUSE_LIB_GLADE \
        -export-dynamic

bin_PROGRAMS = printui

printui_SOURCES =  main.c \
  loadres.c loadres.h \
  bjuidefs.h \
  msgtable.h \
  globals.h \
  modeltbl.c modeltbl.h \
  callbacks.c callbacks.h \
  comdb.c comdb.h \
  widgets.c widgets.h \
  gdktool.c gdktool.h \
  keytext.c keytext.h \
  utility.c utility.h utildef.h msgicon.h \
  dialog.c dialog.h \
  uimain.c uimain.h \
  qualitydlg.c qualitydlg.h \
  colordlg.c colordlg.h \
  usrsizedlg.c usrsizedlg.h \
  versiondlg.c versiondlg.h \
  mediasizedlg.c mediasizedlg.h \
  mediatypedlg.c mediatypedlg.h \
  mediaborderdlg.c mediaborderdlg.h \
  xpmproc.c xpmproc.h \
  ipcproc.c ipcproc.h \
  support.c support.h
#  interface.c interface.h 

printui_LDFLAGS = \
        $(GTK_LIBS) \
        $(LIBXML_LIBS) \
        -lpopt \
        -lcnbpcnclui@CNCL_LIB_ID@ -lcnbpcmcm@CNCL_LIB_ID@

printui_LDADD = \
        $(LIBGLADE_LIBS) \
        -L../../@CNCL_LIB_ID@/libs_bin
