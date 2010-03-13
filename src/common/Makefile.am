noinst_LTLIBRARIES =	libcommon.la

libcommon_la_built_sources = \
	solang-marshal.c \
	solang-marshal.h \
	org-freedesktop-thumbnails-thumbnailer1-dbus-bindings.h

BUILT_SOURCES = \
	$(libcommon_la_built_sources)

libcommon_la_SOURCES = \
	$(libcommon_la_built_sources) \
	archive-maker.cpp \
	archive-maker.h \
	database.cpp \
	database.h \
	delete-action.cpp \
	delete-action.h \
	db-object.cpp \
	db-object.h \
	finally.cpp \
	finally.h \
	tracker-client.cpp \
	tracker-client.h \
	content-type-repo.cpp \
	content-type-repo.h \
	cursor-changer.cpp \
	cursor-changer.h \
	error.cpp \
	error.h \
	exif-data-key.cpp \
	exif-data-key.h \
	exif-data.cpp \
	exif-data.h \
	export-queue-operations.cpp \
	export-queue-operations.h \
	histogram-viewer.cpp \
	histogram-viewer.h \
	histogram.cpp \
	histogram.h \
	id-base.h \
	id-repo.cpp \
	id-repo.h \
	i-operation.cpp \
	i-operation.h \
	i-photo-destination.cpp \
	i-photo-destination.h \
	i-photo-search-criteria.cpp \
	i-photo-search-criteria.h \
	i-photo-source.cpp \
	i-photo-source.h \
	i-plugin.cpp \
	i-plugin.h \
	i-progress-observer.cpp \
	i-progress-observer.h \
	i-renderer.cpp \
	i-renderer.h \
	i-renderer-selector.cpp \
	i-renderer-selector.h \
	i-storage.cpp \
	i-storage.h \
	non-copyable.cpp \
	non-copyable.h \
	operation.cpp \
	operation.h \
	photo.cpp \
	photo.h \
	photo-factory.cpp \
	photo-factory.h \
	plugin.cpp \
	plugin.h \
	pixbuf-maker.cpp \
	pixbuf-maker.h \
	progress-dialog.cpp \
	progress-dialog.h \
	progress-observer.h \
	progress-widget.cpp \
	progress-widget.h \
	renderer-selector.h \
	scale-action.cpp \
	scale-action.h \
	scale-tool-item.cpp \
	scale-tool-item.h \
	singleton.h \
	spinner.cpp \
	spinner.h \
	spinner-dialog.cpp \
	spinner-dialog.h \
	spinner-tool-item.cpp \
	spinner-tool-item.h \
	thumbbuf-maker.cpp \
	thumbbuf-maker.h \
	thumbnailer.cpp \
	thumbnailer.h \
	thumbnailer-proxy.cpp \
	thumbnailer-proxy.h \
	types.h \
	utils.cpp \
	utils.h

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""${datadir}/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-I$(top_builddir) \
	-I$(top_srcdir)/src/application \
	-I$(top_srcdir)/src/attribute \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/database \
	-I$(top_srcdir)/src/editor \
	-I$(top_srcdir)/src/importer \
	-I$(top_srcdir)/src/renderer \
	-I$(top_srcdir)/src/storage \
	$(SOLANG_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GTKMM_CFLAGS) \
	$(GDL_CFLAGS) \
	$(GEGLMM_CFLAGS) \
	$(BABL_CFLAGS) \
	$(TRACKER_CFLAGS)

AM_CXXFLAGS = \
	 -Wall

EXTRA_DIST = \
	solang-marshal.list \
	org-freedesktop-thumbnails-thumbnailer1-dbus.xml

if MAINTAINER_MODE
CLEANFILES = \
	xgen-tmc \
	xgen-tmh

DISTCLEANFILES = \
	$(libcommon_la_built_sources) \
	stamp-solang-marshal.h

org-freedesktop-thumbnails-thumbnailer1-dbus-bindings.h: org-freedesktop-thumbnails-thumbnailer1-dbus.xml Makefile
	$(AM_V_GEN) $(DBUS_BINDING_TOOL) --mode=glib-client --prefix=org-freedesktop-thumbnails-thumbnailer1 $< > $@

solang-marshal.h: stamp-solang-marshal.h
	@true

stamp-solang-marshal.h: solang-marshal.list Makefile
	$(AM_V_GEN) ( \
		cd $(srcdir) \
		&& ( glib-genmarshal \
			--prefix=solang_marshal \
			--header solang-marshal.list ) >> xgen-tmh \
		&& ( cmp -s xgen-tmh solang-marshal.h || cp xgen-tmh solang-marshal.h ) \
		&& rm -f xgen-tmh \
		&& echo timestamp > $(@F) \
	)

solang-marshal.c: solang-marshal.list solang-marshal.h Makefile
	$(AM_V_GEN) ( \
		cd $(srcdir) \
		&& echo "#include \"solang-marshal.h\"" > xgen-tmc \
		&& ( glib-genmarshal \
			--prefix=solang_marshal \
			--body solang-marshal.list ) >> xgen-tmc \
		&& cp xgen-tmc solang-marshal.c \
		&& rm -f xgen-tmc \
	)
endif
