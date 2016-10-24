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

# Options menu in Neverwinter Nights.

src_engines_nwn_libnwn_la_SOURCES += \
    src/engines/nwn/gui/options/controls.h \
    src/engines/nwn/gui/options/feedback.h \
    src/engines/nwn/gui/options/game.h \
    src/engines/nwn/gui/options/gorepass.h \
    src/engines/nwn/gui/options/resolution.h \
    src/engines/nwn/gui/options/sound.h \
    src/engines/nwn/gui/options/soundadv.h \
    src/engines/nwn/gui/options/video.h \
    src/engines/nwn/gui/options/videoadv.h \
    $(EMPTY)

src_engines_nwn_libnwn_la_SOURCES += \
    src/engines/nwn/gui/options/controls.cpp \
    src/engines/nwn/gui/options/feedback.cpp \
    src/engines/nwn/gui/options/game.cpp \
    src/engines/nwn/gui/options/gorepass.cpp \
    src/engines/nwn/gui/options/resolution.cpp \
    src/engines/nwn/gui/options/sound.cpp \
    src/engines/nwn/gui/options/soundadv.cpp \
    src/engines/nwn/gui/options/video.cpp \
    src/engines/nwn/gui/options/videoadv.cpp \
    $(EMPTY)
