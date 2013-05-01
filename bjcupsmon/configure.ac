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

PKG_CHECK_MODULES(LIBXML,libxml)

PKG_CHECK_MODULES(GTK_,gtk+)

AC_OUTPUT([
Makefile
backend/Makefile
src/Makefile
po/Makefile
])

