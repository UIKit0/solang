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

include $(top_srcdir)/src/build-flags.mk
