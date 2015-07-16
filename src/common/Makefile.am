include $(top_srcdir)/Makefile.common

# Pass the git version information to the compiler

VER_REVDESC = $(shell cat $(top_srcdir)/gitstamp/gitstamp)
VERSION_FLAGS = $(shell if test x$(VER_REVDESC) != "x"; then echo "-DXOREOS_REVDESC=\\\"$(VER_REVDESC)\\\""; fi)

VER_REV = $(shell cat $(top_srcdir)/gitstamp/gitstamp | cut -d '+' -f 2-)
VERSION_FLAGS += $(shell if test x$(VER_REV) != "x"; then echo "-DXOREOS_REV=\\\"$(VER_REV)\\\""; fi)

VER_DATE = $(shell date -u '+%Y-%m-%dT%H:%M:%SZ')
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
                 md5.h \
                 blowfish.h \
                 error.h \
                 util.h \
                 strutil.h \
                 encoding.h \
                 platform.h \
                 debugman.h \
                 debug.h \
                 atomic.h \
                 uuid.h \
                 readstream.h \
                 memreadstream.h \
                 writestream.h \
                 memwritestream.h \
                 streamtokenizer.h \
                 stringmap.h \
                 readline.h \
                 readfile.h \
                 writefile.h \
                 filepath.h \
                 filelist.h \
                 bitstream.h \
                 huffman.h \
                 vector3.h \
                 matrix.h \
                 transmatrix.h \
                 boundingbox.h \
                 configfile.h \
                 configman.h \
                 foxpro.h \
                 zipfile.h \
                 pe_exe.h \
                 systemfonts.h \
                 changeid.h \
                 xml.h \
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
                       md5.cpp \
                       blowfish.cpp \
                       error.cpp \
                       util.cpp \
                       strutil.cpp \
                       encoding.cpp \
                       platform.cpp \
                       debugman.cpp \
                       debug.cpp \
                       uuid.cpp \
                       readstream.cpp \
                       memreadstream.cpp \
                       writestream.cpp \
                       memwritestream.cpp \
                       streamtokenizer.cpp \
                       stringmap.cpp \
                       readline.cpp \
                       readfile.cpp \
                       writefile.cpp \
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
                       changeid.cpp \
                       xml.cpp \
                       $(EMPTY)
