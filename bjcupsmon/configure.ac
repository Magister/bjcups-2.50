dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(bjcupsmon, 2.50)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 foreign])

AM_SILENT_RULES([yes])

AC_PROG_CC


LT_INIT



[]


# Gettext support start
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=bjcupsmon
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [The domain to use with gettext])
AM_GLIB_GNU_GETTEXT

PACKAGE_LOCALEDIR=[${datadir}/locale]
AC_SUBST(PACKAGE_LOCALEDIR)

AC_CONFIG_FILES(po/Makefile.in)
# Gettext end

PKG_CHECK_MODULES(LIBGLADE,libglade-2.0)

PKG_CHECK_MODULES(LIBXML,libxml)

GTK2_MINIMUM=2.0.0
AC_SUBST(GTK2_MINIMUM)
GTK3_MINIMUM=3.0.0
AC_SUBST(GTK3_MINIMUM)

gtk_version="none"
with_gtk_default="no"
PKG_CHECK_MODULES([GTK3],[gtk+-3.0 >= $GTK3_MINIMUM],[gtk_version="3" with_gtk_default="yes"])
PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= $GTK2_MINIMUM],[gtk_version="2" with_gtk_default="yes"])
AC_ARG_WITH([gtk], AC_HELP_STRING([--with-gtk=2 or 3],[Select GTK version, 2 or 3]),
            [with_gtk=$withval],
            [with_gtk=$with_gtk_default])

if test "x$gtk_version" = "xnone" ; then
    AC_MSG_ERROR([GTK+ not found!])
fi

if test "x$with_gtk" = "xno" ; then
    AC_MSG_ERROR([GTK+ is needed for bjcupsmon])
elif test "x$with_gtk" = "xyes" ; then
    with_gtk=2
fi

if test "x$with_gtk" = "x3"; then
    AC_MSG_ERROR([gtk 3 is not supported for now, use gtk 2])
    PKG_CHECK_MODULES(GTK,gtk+-3.0)
elif test "x$with_gtk" = "x2"; then
    PKG_CHECK_MODULES(GTK,gtk+-2.0)
else
    AC_MSG_ERROR([Unknown gtk version requested: $with_gtk])
fi

AC_OUTPUT([
Makefile
backend/Makefile
src/Makefile
po/Makefile
])

