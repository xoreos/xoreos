AM_CXXFLAGS = -I../lib -ggdb -Wall -Wpointer-arith -Werror @EOS_CXXFLAGS@

SUBDIRS = common

bin_PROGRAMS = eos

eos_SOURCES = eos.cpp
eos_LDADD = common/libcommon.la
