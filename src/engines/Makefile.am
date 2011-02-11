include $(top_srcdir)/Makefile.common

SUBDIRS = nwn nwn2 kotor kotor2 thewitcher sonic dragonage jade

noinst_LTLIBRARIES = libengines.la

noinst_HEADERS = engine.h \
                 util.h \
                 engineprobe.h \
                 enginemanager.h \
                 gamethread.h

libengines_la_SOURCES = engine.cpp \
                        util.cpp \
                        enginemanager.cpp \
                        gamethread.cpp

libengines_la_LIBADD = nwn/libnwn.la nwn2/libnwn2.la kotor/libkotor.la kotor2/libkotor2.la thewitcher/libthewitcher.la sonic/libsonic.la dragonage/libdragonage.la jade/libjade.la
