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

# libmspack (<https://www.cabextract.org.uk/libmspack/>)

noinst_LTLIBRARIES += external/mspack/libmspack.la
external_mspack_libmspack_la_SOURCES =

external_mspack_libmspack_la_CFLAGS = -I$(srcdir)/external/mspack

external_mspack_libmspack_la_SOURCES += \
    external/mspack/cab.h \
    external/mspack/chm.h \
    external/mspack/crc32.h \
    external/mspack/des.h \
    external/mspack/hlp.h \
    external/mspack/kwaj.h \
    external/mspack/lit.h \
    external/mspack/lzss.h \
    external/mspack/lzx.h \
    external/mspack/macros.h \
    external/mspack/mspack.h \
    external/mspack/mszip.h \
    external/mspack/oab.h \
    external/mspack/qtm.h \
    external/mspack/readbits.h \
    external/mspack/readhuff.h \
    external/mspack/sha.h \
    external/mspack/system.h \
    external/mspack/szdd.h \
    $(EMPTY)

external_mspack_libmspack_la_SOURCES += \
    external/mspack/cabc.c \
    external/mspack/cabd.c \
    external/mspack/chmc.c \
    external/mspack/chmd.c \
    external/mspack/crc32.c \
    external/mspack/hlpc.c \
    external/mspack/hlpd.c \
    external/mspack/kwajc.c \
    external/mspack/kwajd.c \
    external/mspack/litc.c \
    external/mspack/litd.c \
    external/mspack/lzssd.c \
    external/mspack/lzxc.c \
    external/mspack/lzxd.c \
    external/mspack/mszipc.c \
    external/mspack/mszipd.c \
    external/mspack/oabc.c \
    external/mspack/oabd.c \
    external/mspack/qtmc.c \
    external/mspack/qtmd.c \
    external/mspack/system.c \
    external/mspack/szddc.c \
    external/mspack/szddd.c \
    $(EMPTY)

EXTRA_DIST += \
    external/mspack/README.xoreos \
    external/mspack/COPYRIGHT \
    $(EMPTY)
