# xoreos - A reimplementation of BioWare's Aurora engine
#
# xoreos is the legal property of its developers, whose names
# can be found in the AUTHORS file distributed with this source
# distribution.
#
# xoreos is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# xoreos is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with xoreos. If not, see <http://www.gnu.org/licenses/>.

# Common support code used all over the codebase.

include $(top_srcdir)/Makefile.common

# Pass the git version information to the compiler

VER_REVDESC = $(shell cat $(top_builddir)/gitstamp/gitstamp)
VERSION_FLAGS = $(shell if test x$(VER_REVDESC) != "x"; then echo "-DXOREOS_REVDESC=\\\"$(VER_REVDESC)\\\""; fi)

VER_REV = $(shell cat $(top_builddir)/gitstamp/gitstamp | cut -d '+' -f 2-)
VERSION_FLAGS += $(shell if test x$(VER_REV) != "x"; then echo "-DXOREOS_REV=\\\"$(VER_REV)\\\""; fi)

VER_DATE = $(shell date -u '+%Y-%m-%dT%H:%M:%SZ')
VERSION_FLAGS += $(shell if test x$(VER_DATE) != "x"; then echo "-DXOREOS_BUILDDATE=\\\"$(VER_DATE)\\\""; fi)

AM_CXXFLAGS += $(VERSION_FLAGS)
AM_CFLAGS   += $(VERSION_FLAGS)

# Rebuild the version.lo when the git version changed
version.lo: $(top_builddir)/gitstamp/gitstamp

noinst_LTLIBRARIES = libcommon.la

noinst_HEADERS = \
                 system.h \
                 noreturn.h \
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
                 datetime.h \
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
                       datetime.cpp \
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
