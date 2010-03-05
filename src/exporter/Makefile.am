noinst_LTLIBRARIES =	libexporter.la

libexporter_la_SOURCES = \
	exporter.cpp \
	exporter.h \
	exporter-dialog.cpp \
	exporter-dialog.h \
	photo-destination.h \
	photo-destination.cpp \
	photo-destination-enums.h \
	directory-destination.h \
	directory-destination.cpp

if BUILD_BRASERO_EXPORTER
libexporter_la_SOURCES += \
	brasero-destination.cpp \
	brasero-destination.h
endif

EXTRA_libexporter_la_SOURCES = \
	brasero-destination.cpp \
	brasero-destination.h

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""${datadir}/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-I$(top_builddir) \
	-I$(top_srcdir)/src/application \
	-I$(top_srcdir)/src/attribute \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/database \
	-I$(top_srcdir)/src/importer \
	-I$(top_srcdir)/src/renderer \
	-I$(top_srcdir)/src/storage \
	$(SOLANG_CFLAGS) \
	$(BRASERO_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GTKMM_CFLAGS) \
	$(GDL_CFLAGS) \
	$(GEGLMM_CFLAGS) \
	$(TRACKER_CFLAGS)

AM_CXXFLAGS = \
	 -Wall
