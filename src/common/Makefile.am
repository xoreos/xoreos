include $(top_srcdir)/Makefile.common

# Pass the git version information to the compiler

VER_REVDESC = $(shell cat $(top_srcdir)/gitstamp/gitstamp)
VERSION_FLAGS = $(shell if test x$(VER_REVDESC) != "x"; then echo "-DXOREOS_REVDESC=\\\"$(VER_REVDESC)\\\""; fi)

VER_REV = $(shell cat $(top_srcdir)/gitstamp/gitstamp | cut -d '+' -f 2-)
VERSION_FLAGS += $(shell if test x$(VER_REV) != "x"; then echo "-DXOREOS_REV=\\\"$(VER_REV)\\\""; fi)

VER_DATE = $(shell date -u '+%Y-%m-%dT%H:%M:%S')
VERSION_FLAGS += $(shell if test x$(VER_DATE) != "x"; then echo "-DXOREOS_BUILDDATE=\\\"$(VER_DATE)\\\""; fi)

AM_CXXFLAGS += $(VERSION_FLAGS)
AM_CFLAGS   += $(VERSION_FLAGS)

# Rebuild the version.lo when the git version changed
version.lo: $(top_srcdir)/gitstamp/gitstamp

noinst_LTLIBRARIES = libcommon.la

noinst_HEADERS = \
                 system.h \
                 types.h \
                 endianness.h \
                 version.h \
                 noncopyable.h \
                 singleton.h \
                 maths.h \
                 sinetables.h \
                 cosinetables.h \
                 sinewindows.h \
                 fft.h \
                 rdft.h \
                 dct.h \
                 mdct.h \
                 threads.h \
                 thread.h \
                 mutex.h \
                 ustring.h \
                 hash.h \
                 error.h \
                 util.h \
                 strutil.h \
                 encoding.h \
                 debugman.h \
                 debug.h \
                 uuid.h \
                 stream.h \
                 streamtokenizer.h \
                 stringmap.h \
                 readline.h \
                 file.h \
                 filepath.h \
                 filelist.h \
                 bitstream.h \
                 huffman.h \
                 matrix.h \
                 transmatrix.h \
                 boundingbox.h \
                 configfile.h \
                 configman.h \
                 foxpro.h \
                 zipfile.h \
                 pe_exe.h \
                 systemfonts.h \
                 $(EMPTY)

libcommon_la_SOURCES = \
                       version.cpp \
                       maths.cpp \
                       sinetables.cpp \
                       cosinetables.cpp \
                       sinewindows.cpp \
                       fft.cpp \
                       rdft.cpp \
                       dct.cpp \
                       mdct.cpp \
                       threads.cpp \
                       thread.cpp \
                       mutex.cpp \
                       ustring.cpp \
                       error.cpp \
                       util.cpp \
                       strutil.cpp \
                       encoding.cpp \
                       debugman.cpp \
                       debug.cpp \
                       uuid.cpp \
                       stream.cpp \
                       streamtokenizer.cpp \
                       stringmap.cpp \
                       readline.cpp \
                       file.cpp \
                       filepath.cpp \
                       filelist.cpp \
                       huffman.cpp \
                       matrix.cpp \
                       transmatrix.cpp \
                       boundingbox.cpp \
                       configfile.cpp \
                       configman.cpp \
                       foxpro.cpp \
                       zipfile.cpp \
                       pe_exe.cpp \
                       systemfonts.cpp \
                       $(EMPTY)
