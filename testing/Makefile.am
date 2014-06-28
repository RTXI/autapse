#CLEANFILES = *~
#DISTCLEANFILES =
#MAINTAINERCLEANFILES = Makefile.in

#include $(top_srcdir)/.buildvars

#pkglib_LTLIBRARIES = Autapse.la

#Autapse_la_LDFLAGS = -module -avoid-version

PLUGIN_NAME = autapse

SOURCES = \
					autapse.cpp \
					moc_autapse.cpp \
					PSC.cpp \
					PSC.h \
					SpikeDetect.h \
					SpikeDetect.cpp

LIBS = 

include $(shell rtxi_plugin_config --pkgdata-dir)/Makefile.plugin_compile
