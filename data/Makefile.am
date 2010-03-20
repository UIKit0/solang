SUBDIRS = icons

desktopdir = $(datadir)/applications
desktop_in_files = $(PACKAGE_TARNAME).desktop.in.in
desktop_DATA = $(desktop_in_files:.desktop.in.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

layoutdir = $(datadir)/$(PACKAGE_TARNAME)
layout_DATA = \
	solang-layout.xml

pixmapdir = $(datadir)/$(PACKAGE_TARNAME)/pixmaps
pixmap_DATA = \
	camera-photo-16.png \
	camera-photo-22.png \
	camera-photo-24.png \
	camera-photo-32.png \
	camera-photo.svg \
	flickr.png \
	mode-browse-16.png \
	mode-browse-22.png \
	mode-browse-24.png \
	mode-browse-32.png \
	mode-browse.svg \
	mode-image-edit-16.png \
	mode-image-edit-22.png \
	mode-image-edit-24.png \
	tag-16.png \
	tag-22.png \
	tag-new-16.png \
	tag-new-22.png \
	tag-new-24.png

uidir = $(datadir)/$(PACKAGE_TARNAME)/ui
ui_DATA = \
	solang.ui \
	solang-browser-renderer.ui \
	solang-browser-renderer-thumbnail.ui \
	solang-editor.ui \
	solang-editor-renderer.ui \
	solang-enlarged-renderer.ui \
	solang-tag-manager.ui \
	solang-importer.ui \
	solang-search-basket.ui \
	solang-search-manager.ui \
	solang-slideshow.ui \
	solang-slideshow-renderer.ui \
	solang-thumbnail-popup.ui

EXTRA_DIST = \
	$(desktop_in_files) \
	$(layout_DATA) \
	$(pixmap_DATA) \
	$(ui_DATA)

DISTCLEANFILES = \
	$(desktop_DATA)
