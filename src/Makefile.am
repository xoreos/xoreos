include $(top_srcdir)/Makefile.common

SUBDIRS = common aurora sound

bin_PROGRAMS = eos

eos_SOURCES = eos.cpp
eos_LDADD = aurora/libaurora.la common/libcommon.la sound/libsound.la
