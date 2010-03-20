SOLANG_CFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir)/src/application \
	-I$(top_srcdir)/src/attribute \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/editor \
	-I$(top_srcdir)/src/exporter \
	-I$(top_srcdir)/src/renderer

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""${datadir}/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_DOC_DIR=\""$(docdir)"\" \
	$(SOLANG_CFLAGS) \
	$(BABL_CFLAGS) \
	$(BRASERO_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GDL_CFLAGS) \
	$(GEGLMM_CFLAGS) \
	$(GTKIMAGEVIEW_CFLAGS) \
	$(GTKMM_CFLAGS) \
	$(TRACKER_CFLAGS)

AM_CXXFLAGS = \
	 -Wall
