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

# Sound format decoders.

noinst_LTLIBRARIES += src/sound/decoders/libdecoders.la
src_sound_decoders_libdecoders_la_SOURCES =

src_sound_decoders_libdecoders_la_SOURCES += \
    src/sound/decoders/util.h \
    src/sound/decoders/adpcm.h \
    src/sound/decoders/wave_types.h \
    src/sound/decoders/wave.h \
    src/sound/decoders/pcm.h \
    src/sound/decoders/asf.h \
    src/sound/decoders/wma.h \
    src/sound/decoders/wmadata.h \
    src/sound/decoders/wwriffvorbis.h \
    src/sound/decoders/wwriffvorbisdata.h \
    $(EMPTY)

src_sound_decoders_libdecoders_la_SOURCES += \
    src/sound/decoders/adpcm.cpp \
    src/sound/decoders/wave.cpp \
    src/sound/decoders/pcm.cpp \
    src/sound/decoders/asf.cpp \
    src/sound/decoders/wma.cpp \
    src/sound/decoders/wwriffvorbis.cpp \
    $(EMPTY)

aac_sources = \
	src/sound/decoders/aac.cpp \
	src/sound/decoders/aac.h \
	$(EMPTY)

if ENABLE_FAAD
src_sound_decoders_libdecoders_la_SOURCES += $(aac_sources)
else
EXTRA_DIST += $(aac_sources)
endif

mp3_sources = \
	src/sound/decoders/mp3.cpp \
	src/sound/decoders/mp3.h \
	$(EMPTY)

if ENABLE_MAD
src_sound_decoders_libdecoders_la_SOURCES += $(mp3_sources)
else
EXTRA_DIST += $(mp3_sources)
endif

vorbis_sources = \
	src/sound/decoders/vorbis.cpp \
	src/sound/decoders/vorbis.h \
	$(EMPTY)

if ENABLE_VORBIS
src_sound_decoders_libdecoders_la_SOURCES += $(vorbis_sources)
else
EXTRA_DIST += $(vorbis_sources)
endif
