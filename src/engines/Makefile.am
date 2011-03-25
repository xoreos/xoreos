include $(top_srcdir)/Makefile.common

SUBDIRS = nwn nwn2 kotor kotor2 thewitcher sonic dragonage jade

noinst_LTLIBRARIES = libengines.la

noinst_HEADERS = engine.h \
                 engineprobe.h \
                 enginemanager.h \
                 gamethread.h \
                 aurora/util.h \
                 aurora/resources.h \
                 aurora/modelloader.h \
                 aurora/model.h \
                 aurora/widget.h \
                 aurora/gui.h \
                 aurora/console.h

libengines_la_SOURCES = engine.cpp \
                        enginemanager.cpp \
                        gamethread.cpp \
                        aurora/util.cpp \
                        aurora/resources.cpp \
                        aurora/modelloader.cpp \
                        aurora/model.cpp \
                        aurora/widget.cpp \
                        aurora/gui.cpp \
                        aurora/console.cpp

libengines_la_LIBADD = nwn/libnwn.la nwn2/libnwn2.la kotor/libkotor.la kotor2/libkotor2.la thewitcher/libthewitcher.la sonic/libsonic.la dragonage/libdragonage.la jade/libjade.la
