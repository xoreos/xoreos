include $(top_srcdir)/Makefile.common

SUBDIRS = nwn nwn2 kotor kotor2 thewitcher

noinst_LTLIBRARIES = libengines.la

noinst_HEADERS = engine.h \
                 engineprobe.h \
                 enginemanager.h

libengines_la_SOURCES = engine.cpp \
                        enginemanager.cpp

libengines_la_LIBADD = nwn/libnwn.la nwn2/libnwn2.la kotor/libkotor.la kotor2/libkotor2.la thewitcher/libthewitcher.la
