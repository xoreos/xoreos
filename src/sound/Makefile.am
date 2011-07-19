include $(top_srcdir)/Makefile.common

SUBDIRS = decoders

noinst_LTLIBRARIES = libsound.la

noinst_HEADERS = types.h \
                 sound.h \
                 audiostream.h \
                 interleaver.h

libsound_la_SOURCES = sound.cpp \
                      audiostream.cpp \
                      interleaver.cpp

libsound_la_LIBADD = decoders/libdecoders.la
