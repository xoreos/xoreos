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

# Ingame system in Star Wars: Knights of the Old Republic.

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/gui/ingame/ingame.h \
    src/engines/kotor/gui/ingame/hud.h \
    src/engines/kotor/gui/ingame/menu.h \
    src/engines/kotor/gui/ingame/menu_equ.h \
    src/engines/kotor/gui/ingame/menu_inv.h \
    src/engines/kotor/gui/ingame/menu_char.h \
    src/engines/kotor/gui/ingame/menu_abi.h \
    src/engines/kotor/gui/ingame/menu_msg.h \
    src/engines/kotor/gui/ingame/menu_jou.h \
    src/engines/kotor/gui/ingame/menu_map.h \
    src/engines/kotor/gui/ingame/menu_opt.h \
    $(EMPTY)

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/gui/ingame/ingame.cpp \
    src/engines/kotor/gui/ingame/hud.cpp \
    src/engines/kotor/gui/ingame/menu.cpp \
    src/engines/kotor/gui/ingame/menu_equ.cpp \
    src/engines/kotor/gui/ingame/menu_inv.cpp \
    src/engines/kotor/gui/ingame/menu_char.cpp \
    src/engines/kotor/gui/ingame/menu_abi.cpp \
    src/engines/kotor/gui/ingame/menu_msg.cpp \
    src/engines/kotor/gui/ingame/menu_jou.cpp \
    src/engines/kotor/gui/ingame/menu_map.cpp \
    src/engines/kotor/gui/ingame/menu_opt.cpp \
    $(EMPTY)
