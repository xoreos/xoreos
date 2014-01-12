include $(top_srcdir)/Makefile.common

SUBDIRS = \
          decoders \
          $(EMPTY)

noinst_LTLIBRARIES = libsound.la

noinst_HEADERS = \
                 types.h \
                 sound.h \
                 audiostream.h \
                 interleaver.h \
                 $(EMPTY)

libsound_la_SOURCES = \
                      sound.cpp \
                      audiostream.cpp \
                      interleaver.cpp \
                      $(EMPTY)

libsound_la_LIBADD = \
                     decoders/libdecoders.la \
                     $(EMPTY)
