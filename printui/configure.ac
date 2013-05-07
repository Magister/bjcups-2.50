dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

m4_define([printui_suffix], [m4_esyscmd(test -n "$program_suffix" && echo -n $program_suffix || echo -n pixmaip1500)])
m4_define([printui_progname], [m4_esyscmd(test -n "$program_suffix" && echo -n printui$program_suffix || echo -n printuipixmaip1500)])

AC_INIT(printui_progname(), 2.50)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

program_suffix=printui_suffix()
program_transform_name="s,\$\$,${program_suffix},;"

AC_DEFINE_UNQUOTED(PACKAGE_PRINTER_MODEL, "${program_suffix}", "Printer model")

AM_SILENT_RULES([yes])

if test "x${program_suffix}" = "x"; then
  AC_MSG_ERROR([program_suffix is not set while running autogen.sh])
fi

case "${program_suffix}" in
  pixus560i) CNCL_LIB_ID=189;;
  pixus860i) CNCL_LIB_ID=187;;
  pixus990i) CNCL_LIB_ID=191;;
  i250) CNCL_LIB_ID=180;;
  i255) CNCL_LIB_ID=182;;
  pixus550i) CNCL_LIB_ID=154;;
  pixus850i) CNCL_LIB_ID=156;;
  pixus950i) CNCL_LIB_ID=158;;
  bjs500) CNCL_LIB_ID=131;;
  bjs300) CNCL_LIB_ID=117;;
  bjf900) CNCL_LIB_ID=125;;
  bjf9000) CNCL_LIB_ID=127;;
  pixusip3100) CNCL_LIB_ID=218;;
  pixusip4100) CNCL_LIB_ID=221;;
  pixusip8600) CNCL_LIB_ID=238;;
  pixmaip1000) CNCL_LIB_ID=230;;
  pixmaip1500) CNCL_LIB_ID=214;;
esac
AC_SUBST(CNCL_LIB_ID)

AC_DEFINE(PROG_PATH, "${bindir}", "binaries dir")

AC_PROG_CC




dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=stsmon${program_suffix}
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])

AM_GLIB_GNU_GETTEXT



LT_INIT


PACKAGE_DATA_DIR=[${datadir}]
AC_SUBST(PACKAGE_DATA_DIR)

PACKAGE_LOCALEDIR=[${datadir}/locale]
AC_SUBST(PACKAGE_LOCALEDIR)



GTK1_MINIMUM=1.2.0
AC_SUBST(GTK1_MINIMUM)
GTK2_MINIMUM=2.0.0
AC_SUBST(GTK2_MINIMUM)
GTK3_MINIMUM=3.0.0
AC_SUBST(GTK3_MINIMUM)

gtk_version="none"
with_gtk_default="no"
PKG_CHECK_MODULES([GTK3],[gtk+-3.0 >= $GTK3_MINIMUM],[gtk_version="3" with_gtk_default="yes"])
PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= $GTK2_MINIMUM],[gtk_version="2" with_gtk_default="yes"])
PKG_CHECK_MODULES([GTK1],[gtk+ >= $GTK1_MINIMUM],[gtk_version="1" with_gtk_default="yes"])
AC_ARG_WITH([gtk], AC_HELP_STRING([--with-gtk=1, 2 or 3],[Select GTK version, 1, 2 or 3]),
            [with_gtk=$withval],
            [with_gtk=$with_gtk_default])

if test "x$gtk_version" = "xnone" ; then
    AC_MSG_ERROR([GTK+ not found!])
fi

if test "x$with_gtk" = "xno" ; then
    AC_MSG_ERROR([GTK+ is needed for bjcupsmon])
elif test "x$with_gtk" = "xyes" ; then
    with_gtk=1
fi

if test "x$with_gtk" = "x3"; then
    AC_MSG_ERROR([gtk 3 is not supported for now, use gtk 2])
    PKG_CHECK_MODULES(GTK,gtk+-3.0)
elif test "x$with_gtk" = "x2"; then
    PKG_CHECK_MODULES(GTK,gtk+-2.0)
elif test "x$with_gtk" = "x1"; then
    PKG_CHECK_MODULES(GTK,gtk+)
else
    AC_MSG_ERROR([Unknown gtk version requested: $with_gtk])
fi

PKG_CHECK_MODULES(LIBXML,libxml)

AC_OUTPUT([
Makefile
src/Makefile
cmd/Makefile
include/Makefile
include/cncl/Makefile
include/misc/Makefile
po/Makefile.in
])
