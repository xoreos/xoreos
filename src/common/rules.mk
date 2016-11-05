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

noinst_LTLIBRARIES += src/common/libcommon.la
src_common_libcommon_la_SOURCES =

src_common_libcommon_la_SOURCES += \
    src/common/system.h \
    src/common/noreturn.h \
    src/common/types.h \
    src/common/endianness.h \
    src/common/deallocator.h \
    src/common/scopedptr.h \
    src/common/disposableptr.h \
    src/common/ptrlist.h \
    src/common/ptrvector.h \
    src/common/ptrmap.h \
    src/common/singleton.h \
    src/common/maths.h \
    src/common/sinetables.h \
    src/common/cosinetables.h \
    src/common/sinewindows.h \
    src/common/fft.h \
    src/common/rdft.h \
    src/common/dct.h \
    src/common/mdct.h \
    src/common/threads.h \
    src/common/thread.h \
    src/common/mutex.h \
    src/common/ustring.h \
    src/common/hash.h \
    src/common/md5.h \
    src/common/blowfish.h \
    src/common/deflate.h \
    src/common/lzma.h \
    src/common/error.h \
    src/common/util.h \
    src/common/strutil.h \
    src/common/encoding.h \
    src/common/platform.h \
    src/common/debugman.h \
    src/common/debug.h \
    src/common/atomic.h \
    src/common/uuid.h \
    src/common/datetime.h \
    src/common/readstream.h \
    src/common/memreadstream.h \
    src/common/writestream.h \
    src/common/memwritestream.h \
    src/common/streamtokenizer.h \
    src/common/stringmap.h \
    src/common/readline.h \
    src/common/readfile.h \
    src/common/writefile.h \
    src/common/filepath.h \
    src/common/filelist.h \
    src/common/binsearch.h \
    src/common/bitstream.h \
    src/common/huffman.h \
    src/common/vector3.h \
    src/common/matrix4x4.h \
    src/common/boundingbox.h \
    src/common/configfile.h \
    src/common/configman.h \
    src/common/foxpro.h \
    src/common/zipfile.h \
    src/common/pe_exe.h \
    src/common/systemfonts.h \
    src/common/changeid.h \
    src/common/xml.h \
    $(EMPTY)

src_common_libcommon_la_SOURCES += \
    src/common/maths.cpp \
    src/common/sinetables.cpp \
    src/common/cosinetables.cpp \
    src/common/sinewindows.cpp \
    src/common/fft.cpp \
    src/common/rdft.cpp \
    src/common/dct.cpp \
    src/common/mdct.cpp \
    src/common/threads.cpp \
    src/common/thread.cpp \
    src/common/mutex.cpp \
    src/common/ustring.cpp \
    src/common/md5.cpp \
    src/common/blowfish.cpp \
    src/common/deflate.cpp \
    src/common/lzma.cpp \
    src/common/error.cpp \
    src/common/util.cpp \
    src/common/strutil.cpp \
    src/common/encoding.cpp \
    src/common/platform.cpp \
    src/common/debugman.cpp \
    src/common/debug.cpp \
    src/common/uuid.cpp \
    src/common/datetime.cpp \
    src/common/readstream.cpp \
    src/common/memreadstream.cpp \
    src/common/writestream.cpp \
    src/common/memwritestream.cpp \
    src/common/streamtokenizer.cpp \
    src/common/stringmap.cpp \
    src/common/readline.cpp \
    src/common/readfile.cpp \
    src/common/writefile.cpp \
    src/common/filepath.cpp \
    src/common/filelist.cpp \
    src/common/huffman.cpp \
    src/common/matrix4x4.cpp \
    src/common/boundingbox.cpp \
    src/common/configfile.cpp \
    src/common/configman.cpp \
    src/common/foxpro.cpp \
    src/common/zipfile.cpp \
    src/common/pe_exe.cpp \
    src/common/systemfonts.cpp \
    src/common/changeid.cpp \
    src/common/xml.cpp \
    $(EMPTY)
