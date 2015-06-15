include $(top_srcdir)/Makefile.common

SUBDIRS = \
          nwn \
          nwn2 \
          kotor \
          kotor2 \
          jade \
          witcher \
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
                 aurora/modelloader.h \
                 aurora/model.h \
                 aurora/widget.h \
                 aurora/gui.h \
                 aurora/console.h \
                 aurora/loadprogress.h \
                 aurora/camera.h \
                 $(EMPTY)

libengines_la_SOURCES = \
                        engine.cpp \
                        enginemanager.cpp \
                        gamethread.cpp \
                        aurora/util.cpp \
                        aurora/resources.cpp \
                        aurora/tokenman.cpp \
                        aurora/modelloader.cpp \
                        aurora/model.cpp \
                        aurora/widget.cpp \
                        aurora/gui.cpp \
                        aurora/console.cpp \
                        aurora/loadprogress.cpp \
                        aurora/camera.cpp \
                        $(EMPTY)

libengines_la_LIBADD = \
                       nwn/libnwn.la \
                       nwn2/libnwn2.la \
                       kotor/libkotor.la \
                       kotor2/libkotor2.la \
                       jade/libjade.la \
                       witcher/libwitcher.la \
                       sonic/libsonic.la \
                       dragonage/libdragonage.la \
                       dragonage2/libdragonage2.la \
                       $(EMPTY)
