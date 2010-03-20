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

include $(top_srcdir)/src/build-flags.mk
