include $(top_srcdir)/Makefile.common

SUBDIRS = decoders

noinst_LTLIBRARIES = libsound.la

noinst_HEADERS = sound.h \
                 audiostream.h

libsound_la_SOURCES = sound.cpp \
                      audiostream.cpp

libsound_la_LIBADD = decoders/libdecoders.la