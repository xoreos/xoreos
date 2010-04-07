include $(top_srcdir)/Makefile.common

SUBDIRS = common graphics sound events aurora engines

bin_PROGRAMS = eos

eos_SOURCES = eos.cpp
eos_LDADD = engines/libengines.la aurora/libaurora.la events/libevents.la sound/libsound.la graphics/libgraphics.la common/libcommon.la
