include $(top_srcdir)/Makefile.common

noinst_HEADERS = system.h types.h endianness.h util.h stream.h file.h filelist.h
noinst_LTLIBRARIES = libcommon.la

libcommon_la_SOURCES = stream.cpp file.cpp filelist.cpp
libcommon_la_CXXFLAGS = $(AM_CXXFLAGS) $(BOOST_CPPFLAGS)
libcommon_la_LDFLAGS = $(BOOST_FILESYSTEM_LDFLAGS) $(BOOST_REGEX_LDFLAGS)
libcommon_la_LIBS = $(BOOST_FILESYSTEM_LIBS) $(BOOST_REGEX_LIBS)

