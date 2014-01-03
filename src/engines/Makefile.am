include $(top_srcdir)/Makefile.common

SUBDIRS = \
          nwn \
          nwn2 \
          kotor \
          kotor2 \
          jade \
          thewitcher \
          sonic \
          dragonage \
          dragonage2 \
          $(EMPTY)

noinst_LTLIBRARIES = libengines.la

noinst_HEADERS = \
                 engine.h \
                 engineprobe.h \
                 enginemanager.h \
                 gamethread.h \
                 aurora/util.h \
                 aurora/resources.h \
                 aurora/tokenman.h \
                 aurora/widget.h \
                 aurora/gui.h \
                 $(EMPTY)

libengines_la_SOURCES = \
                        engine.cpp \
                        enginemanager.cpp \
                        gamethread.cpp \
                        aurora/util.cpp \
                        aurora/resources.cpp \
                        aurora/tokenman.cpp \
                        aurora/widget.cpp \
                        aurora/gui.cpp \
                        $(EMPTY)

libengines_la_LIBADD = \
                       nwn/libnwn.la \
                       nwn2/libnwn2.la \
                       kotor/libkotor.la \
                       kotor2/libkotor2.la \
                       jade/libjade.la \
                       thewitcher/libthewitcher.la \
                       sonic/libsonic.la \
                       dragonage/libdragonage.la \
                       dragonage2/libdragonage2.la \
                       $(EMPTY)
