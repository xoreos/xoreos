include $(top_srcdir)/Makefile.common

SUBDIRS = common sound aurora engines

bin_PROGRAMS = eos

eos_SOURCES = eos.cpp
eos_LDADD = engines/libengines.la aurora/libaurora.la sound/libsound.la common/libcommon.la
