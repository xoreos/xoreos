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

include $(top_srcdir)/Makefile.common

noinst_LTLIBRARIES = libdecoders.la

noinst_HEADERS = \
                 util.h \
                 codec.h \
                 mp3.h \
                 vorbis.h \
                 adpcm.h \
                 wave_types.h \
                 wave.h \
                 pcm.h \
                 aac.h \
                 asf.h \
                 wma.h \
                 wmadata.h \
                 $(EMPTY)

libdecoders_la_SOURCES = \
                         codec.cpp \
                         mp3.cpp \
                         vorbis.cpp \
                         adpcm.cpp \
                         wave.cpp \
                         pcm.cpp \
                         aac.cpp \
                         asf.cpp \
                         wma.cpp \
                         $(EMPTY)
