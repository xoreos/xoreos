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

# Video (movies, cutscenes) subsystem.

noinst_LTLIBRARIES += src/video/libvideo.la
src_video_libvideo_la_SOURCES =

src_video_libvideo_la_SOURCES += \
    src/video/decoder.h \
    src/video/bink.h \
    src/video/binkdata.h \
    src/video/fader.h \
    src/video/quicktime.h \
    src/video/xmv.h \
    src/video/actimagine.h \
    $(EMPTY)

src_video_libvideo_la_SOURCES += \
    src/video/decoder.cpp \
    src/video/bink.cpp \
    src/video/fader.cpp \
    src/video/quicktime.cpp \
    src/video/xmv.cpp \
    src/video/actimagine.cpp \
    $(EMPTY)

src_video_libvideo_la_LIBADD = \
    src/video/aurora/libaurora.la \
    src/video/codecs/libcodecs.la \
    $(EMPTY)

# Subdirectories

include src/video/codecs/rules.mk
include src/video/aurora/rules.mk
