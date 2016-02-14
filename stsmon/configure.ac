dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

m4_define([stsmon_suffix], [m4_esyscmd(test -n "$program_suffix" && echo -n $program_suffix || echo -n pixmaip1500)])
m4_define([stsmon_progname], [m4_esyscmd(test -n "$program_suffix" && echo -n stsmon$program_suffix || echo -n stsmonpixmaip1500)])
dnl m4_define([stsmon_suffix], [m4_esyscmd(test -n "$program_suffix" && echo -n $program_suffix)])
dnl m4_define([stsmon_progname], [m4_esyscmd(test -n "$program_suffix" && echo -n stsmon$program_suffix)])

AC_INIT(stsmon_progname(), 2.50)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

program_suffix=stsmon_suffix()
program_transform_name="s,\$\$,${program_suffix},;"

AC_DEFINE_UNQUOTED(PACKAGE_PRINTER_MODEL, "${program_suffix}", "Printer model")

AM_SILENT_RULES([yes])

if test "x${program_suffix}" = "x"; then
  AC_MSG_ERROR([program_suffix is not set while running autogen.sh])
fi

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



GTK1_MINIMUM=1.2.0
AC_SUBST(GTK1_MINIMUM)
GTK2_MINIMUM=2.0.0
AC_SUBST(GTK2_MINIMUM)
GTK3_MINIMUM=3.0.0
AC_SUBST(GTK3_MINIMUM)

gtk_version="none"
with_gtk_default="no"
have_gtk1="yes"
have_gtk2="yes"
have_gtk3="yes"
PKG_CHECK_MODULES([GTK3],[gtk+-3.0 >= $GTK3_MINIMUM],[gtk_version="3" with_gtk_default="yes"], [have_gtk1="no"])
PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= $GTK2_MINIMUM],[gtk_version="2" with_gtk_default="yes"], [have_gtk2="no"])
PKG_CHECK_MODULES([GTK1],[gtk+ >= $GTK1_MINIMUM],[gtk_version="1" with_gtk_default="yes"], [have_gtk3="no"])
AC_ARG_WITH([gtk], AC_HELP_STRING([--with-gtk=1, 2 or 3],[Select GTK version, 1, 2 or 3]),
            [with_gtk=$withval],
            [with_gtk=$with_gtk_default])

if test "x$gtk_version" = "xnone" ; then
    AC_MSG_ERROR([GTK+ not found!])
fi

if test "x$with_gtk" = "xno" ; then
    AC_MSG_ERROR([GTK+ is needed for bjcupsmon])
elif test "x$with_gtk" = "xyes" ; then
	if test "x$have_gtk2" = "xyes" ; then
		with_gtk=2
	elif test "x$have_gtk1" = "xyes" ; then
		with_gtk=1
	elif test "x$have_gtk3" = "xyes" ; then
		with_gtk=3
	fi
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

AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
lgmon/Makefile
])
