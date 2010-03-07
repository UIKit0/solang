bin_PROGRAMS = solang

solang_SOURCES = \
	application.cpp \
	application.h \
	deletion-queue.cpp \
	deletion-queue.h \
	engine.cpp \
	engine.h \
	main.cpp \
	main-window.cpp \
	main-window.h \
	renderer-registry.cpp \
	renderer-registry.h

solang_LDFLAGS = 

solang_LDADD = \
	$(top_builddir)/src/exporter/libexporter.la \
	$(top_builddir)/src/renderer/librenderer.la \
	$(top_builddir)/src/attribute/libattribute.la \
	$(top_builddir)/src/editor/libeditor.la \
	$(top_builddir)/src/common/libcommon.la \
	$(ARCHIVE_LIBS) \
	$(BRASERO_LIBS) \
	$(DBUS_LIBS) \
	$(GTKIMAGEVIEW_LIBS) \
	$(GTKMM_LIBS) \
	$(GDL_LIBS) \
	$(BABL_LIBS) \
	$(GEGLMM_LIBS) \
	$(TRACKER_LIBS)

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""${datadir}/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_DOC_DIR=\""$(docdir)"\" \
	-I$(top_builddir) \
	-I$(top_srcdir)/src/application \
	-I$(top_srcdir)/src/attribute \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/database \
	-I$(top_srcdir)/src/editor \
	-I$(top_srcdir)/src/exporter \
	-I$(top_srcdir)/src/renderer \
	$(SOLANG_CFLAGS) \
	$(BRASERO_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GTKIMAGEVIEW_CFLAGS) \
	$(GTKMM_CFLAGS) \
	$(GDL_CFLAGS) \
	$(GEGLMM_CFLAGS) \
	$(TRACKER_CFLAGS)

AM_CXXFLAGS = \
	 -Wall
