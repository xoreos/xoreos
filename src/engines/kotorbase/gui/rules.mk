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

# Common base for the GUI system of KotOR games.

src_engines_kotorbase_libkotorbase_la_SOURCES += \
    src/engines/kotorbase/gui/inventoryitem.h \
    src/engines/kotorbase/gui/chargeninfo.h \
    src/engines/kotorbase/gui/ingame.h \
    src/engines/kotorbase/gui/dialog.h \
    src/engines/kotorbase/gui/partyselection.h \
    src/engines/kotorbase/gui/gui.h \
    src/engines/kotorbase/gui/guibackground.h \
    src/engines/kotorbase/gui/loadscreen.h \
    $(EMPTY)

src_engines_kotorbase_libkotorbase_la_SOURCES += \
    src/engines/kotorbase/gui/inventoryitem.cpp \
    src/engines/kotorbase/gui/chargeninfo.cpp \
    src/engines/kotorbase/gui/ingame.cpp \
    src/engines/kotorbase/gui/dialog.cpp \
    src/engines/kotorbase/gui/partyselection.cpp \
    src/engines/kotorbase/gui/gui.cpp \
    src/engines/kotorbase/gui/guibackground.cpp \
    src/engines/kotorbase/gui/loadscreen.cpp \
    $(EMPTY)
