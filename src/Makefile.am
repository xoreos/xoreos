include $(top_srcdir)/Makefile.common

SUBDIRS = common graphics sound video events aurora engines

noinst_HEADERS = cline.h

bin_PROGRAMS = xoreos

xoreos_SOURCES = cline.cpp \
                 xoreos.cpp

xoreos_LDADD = engines/libengines.la events/libevents.la video/libvideo.la sound/libsound.la graphics/libgraphics.la aurora/libaurora.la common/libcommon.la ../lua/liblua.la
