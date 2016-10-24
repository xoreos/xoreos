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

# Ingame menus in Neverwinter Nights.

src_engines_nwn_libnwn_la_SOURCES += \
    src/engines/nwn/gui/ingame/ingame.h \
    src/engines/nwn/gui/ingame/main.h \
    src/engines/nwn/gui/ingame/quickbar.h \
    src/engines/nwn/gui/ingame/quickchat.h \
    src/engines/nwn/gui/ingame/compass.h \
    src/engines/nwn/gui/ingame/charinfo.h \
    src/engines/nwn/gui/ingame/partyleader.h \
    src/engines/nwn/gui/ingame/dialog.h \
    $(EMPTY)

src_engines_nwn_libnwn_la_SOURCES += \
    src/engines/nwn/gui/ingame/ingame.cpp \
    src/engines/nwn/gui/ingame/main.cpp \
    src/engines/nwn/gui/ingame/quickbar.cpp \
    src/engines/nwn/gui/ingame/quickchat.cpp \
    src/engines/nwn/gui/ingame/compass.cpp \
    src/engines/nwn/gui/ingame/charinfo.cpp \
    src/engines/nwn/gui/ingame/partyleader.cpp \
    src/engines/nwn/gui/ingame/dialog.cpp \
    $(EMPTY)
