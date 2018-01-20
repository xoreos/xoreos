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

# Options menu in Jade Empire.

src_engines_jade_libjade_la_SOURCES += \
    src/engines/jade/gui/options/audio.h \
    src/engines/jade/gui/options/video.h \
    src/engines/jade/gui/options/diff.h \
    src/engines/jade/gui/options/feed.h \
    src/engines/jade/gui/options/control.h \
    $(EMPTY)

src_engines_jade_libjade_la_SOURCES += \
    src/engines/jade/gui/options/audio.cpp \
    src/engines/jade/gui/options/video.cpp \
    src/engines/jade/gui/options/diff.cpp \
    src/engines/jade/gui/options/feed.cpp \
    src/engines/jade/gui/options/control.cpp \
    $(EMPTY)
