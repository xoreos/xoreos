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

# Video format decoders.

noinst_LTLIBRARIES += src/video/codecs/libcodecs.la
src_video_codecs_libcodecs_la_SOURCES =

src_video_codecs_libcodecs_la_SOURCES += \
    src/video/codecs/codec.h \
    src/video/codecs/wmv2data.h \
    src/video/codecs/xmvwmv2.h \
    $(EMPTY)

src_video_codecs_libcodecs_la_SOURCES += \
    src/video/codecs/codec.cpp \
    src/video/codecs/wmv2data.cpp \
    src/video/codecs/xmvwmv2.cpp \
    $(EMPTY)

vpx_source = \
    src/video/codecs/vpx.cpp \
    src/video/codecs/vpx.h \
    $(EMPTY)

xvid_source = \
    src/video/codecs/h263.cpp \
    src/video/codecs/h263.h \
    $(EMPTY)

if ENABLE_XVIDCORE
src_video_codecs_libcodecs_la_SOURCES += $(xvid_source)
else
EXTRA_DIST += $(xvid_source)
endif

if ENABLE_VPX
src_video_codecs_libcodecs_la_SOURCES += $(vpx_source)
else
EXTRA_DIST += $(vpx_source)
endif

