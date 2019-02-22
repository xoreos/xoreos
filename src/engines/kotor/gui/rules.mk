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

# GUI system in Star Wars: Knights of the Old Republic.

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/gui/saveload.h \
    src/engines/kotor/gui/dialog.h \
    $(EMPTY)

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/gui/saveload.cpp \
    src/engines/kotor/gui/dialog.cpp \
    $(EMPTY)

include src/engines/kotor/gui/main/rules.mk
include src/engines/kotor/gui/options/rules.mk
include src/engines/kotor/gui/chargen/rules.mk
include src/engines/kotor/gui/ingame/rules.mk
include src/engines/kotor/gui/dialogs/rules.mk
include src/engines/kotor/gui/loadscreen/rules.mk
