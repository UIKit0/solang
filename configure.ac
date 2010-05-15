AC_INIT([Solang],[0.4.1],
        [https://bugzilla.gnome.org/enter_bug.cgi?product=solang],
        [solang])
AC_PREREQ(2.63)

AM_INIT_AUTOMAKE(AC_PACKAGE_TARNAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_SEARCH_LIBS([strerror], [cposix])
AC_PROG_CXX
AC_PROG_CC
AC_HEADER_STDC

GNOME_DOC_INIT([0.18.0])

IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=solang
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [GETTEXT package name])
AM_GLIB_GNU_GETTEXT


LT_INIT


AC_ARG_VAR(
    [DBUS_BINDING_TOOL],
    [Tool to generate C bindings from XML D-Bus interface definitions])
AC_PATH_PROG([DBUS_BINDING_TOOL], [dbus-binding-tool], [no])
if test x"$DBUS_BINDING_TOOL" = x"no"; then
    AC_MSG_ERROR([could not find dbus-binding-tool in \$PATH.])
fi

AC_CHECK_HEADERS([archive.h],
                 [ARCHIVE_LIBS=-larchive
                  AC_SUBST([ARCHIVE_LIBS])],
                 [AC_MSG_FAILURE([Can't find libarchive.])])

PKG_CHECK_MODULES(BABL, [babl])
PKG_CHECK_MODULES(DBUS, [dbus-glib-1])
PKG_CHECK_MODULES(GDL, [gdl-1.0])
PKG_CHECK_MODULES(GEGLMM, [geglmm >= 0.1.0])
PKG_CHECK_MODULES(GTKIMAGEVIEW, [gtkimageview])
PKG_CHECK_MODULES(GTKMM, [gtkmm-2.4 >= 2.8])
PKG_CHECK_MODULES(TRACKER, [tracker-client-0.8])
PKG_CHECK_MODULES(GCONF, [gconf-2.0 >= 2.31.1])

AC_ARG_ENABLE(
    [brasero-exporter],
    [AS_HELP_STRING(
        [--disable-brasero-exporter],
        [disable the Brasero-based CD/DVD exporter (default=auto)])],
    [],
    [enable_brasero_exporter=auto])
AS_IF([test "x$enable_brasero_exporter" != xno],
      [PKG_CHECK_MODULES(
           [BRASERO],
           [libbrasero-burn >= 2.28.3 libbrasero-media >= 2.28.3],
           [have_brasero=yes
            AC_DEFINE([HAVE_BRASERO],
                      [1],
                      [Define to 1 if you have Brasero >= 2.28.3])],
           [if test "x$enable_brasero_exporter" != xauto; then
                AC_MSG_FAILURE(
                    [Can't enable the Brasero-based CD/DVD exporter])
            fi])],
      [have_brasero=no])
AM_CONDITIONAL([BUILD_BRASERO_EXPORTER],
               [test "x$have_brasero" = xyes])

AC_CONFIG_FILES([
Makefile
data/Makefile
data/solang.desktop.in
data/icons/Makefile
data/icons/16x16/Makefile
data/icons/22x22/Makefile
data/icons/24x24/Makefile
data/icons/32x32/Makefile
data/icons/48x48/Makefile
data/icons/96x96/Makefile
data/icons/128x128/Makefile
data/icons/scalable/Makefile
help/Makefile
src/Makefile
src/application/Makefile
src/attribute/Makefile
src/common/Makefile
src/editor/Makefile
src/exporter/Makefile
src/renderer/Makefile
po/Makefile.in
])
AC_OUTPUT

AC_MSG_RESULT([
Building $PACKAGE_NAME with prefix=$prefix
Brasero-based CD/DVD exporter: $have_brasero
])
