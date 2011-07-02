include $(top_srcdir)/Makefile.common

SUBDIRS = common graphics sound video events aurora engines

noinst_HEADERS = cline.h

bin_PROGRAMS = eos

eos_SOURCES = cline.cpp \
              eos.cpp

eos_LDADD = engines/libengines.la events/libevents.la video/libvideo.la sound/libsound.la graphics/libgraphics.la aurora/libaurora.la common/libcommon.la ../lua/liblua.la
