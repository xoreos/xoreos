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

# Options menu in Star Wars: Knights of the Old Republic.

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/gui/options/gameplay.h \
    src/engines/kotor/gui/options/mousesettings.h \
    src/engines/kotor/gui/options/keyboardconfig.h \
    src/engines/kotor/gui/options/feedback.h \
    src/engines/kotor/gui/options/autopause.h \
    src/engines/kotor/gui/options/graphics.h \
    src/engines/kotor/gui/options/graphicsadv.h \
    src/engines/kotor/gui/options/sound.h \
    src/engines/kotor/gui/options/soundadv.h \
    $(EMPTY)

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/gui/options/gameplay.cpp \
    src/engines/kotor/gui/options/mousesettings.cpp \
    src/engines/kotor/gui/options/keyboardconfig.cpp \
    src/engines/kotor/gui/options/feedback.cpp \
    src/engines/kotor/gui/options/autopause.cpp \
    src/engines/kotor/gui/options/graphics.cpp \
    src/engines/kotor/gui/options/graphicsadv.cpp \
    src/engines/kotor/gui/options/sound.cpp \
    src/engines/kotor/gui/options/soundadv.cpp \
    $(EMPTY)
