include $(top_srcdir)/Makefile.common

SUBDIRS = codecs aurora

noinst_LTLIBRARIES = libvideo.la

noinst_HEADERS = decoder.h \
                 bink.h \
                 binkdata.h \
                 fader.h \
                 quicktime.h \
                 xmv.h \
                 actimagine.h


libvideo_la_SOURCES = decoder.cpp \
                      bink.cpp \
                      fader.cpp \
                      quicktime.cpp \
                      xmv.cpp \
                      actimagine.cpp

libvideo_la_LIBADD = aurora/libaurora.la codecs/libcodecs.la
