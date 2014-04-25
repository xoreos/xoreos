include $(top_srcdir)/Makefile.common

SUBDIRS = \
          common \
          graphics \
          sound \
          video \
          events \
          aurora \
          engines \
          $(EMPTY)

noinst_HEADERS = \
                 cline.h \
                 $(EMPTY)

bin_PROGRAMS = xoreos

xoreos_SOURCES = \
                 cline.cpp \
                 xoreos.cpp \
                 $(EMPTY)

xoreos_LDADD = \
               engines/libengines.la \
               events/libevents.la \
               video/libvideo.la \
               sound/libsound.la \
               graphics/libgraphics.la \
               aurora/libaurora.la \
               common/libcommon.la \
               ../lua/liblua.la \
               $(LDADD) \
               $(EMPTY)
