CLEANFILES = *~
DISTCLEANFILES =
MAINTAINERCLEANFILES = Makefile.in

include $(top_srcdir)/.buildvars

pkglib_LTLIBRARIES = Autapse.la

Autapse_la_LDFLAGS = -module -avoid-version

Autapse_la_SOURCES = \
		Autapse.cpp \
        ../include/PSC.cpp \
        ../include/PSC.h \
        ../include/SpikeDetect.h \
        ../include/SpikeDetect.cpp
