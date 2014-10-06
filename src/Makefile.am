include $(top_srcdir)/Makefile.common

# Windows resources
.rc.o:
	$(AM_V_GEN)$(RC) -DHAVE_CONFIG_H -I$(top_srcdir) -o $@ $<

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

if WIN32
xoreos_SOURCES += ../dists/win32/xoreos.rc
endif

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
