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

# NWScript scripting system in The Witcher.

src_engines_witcher_libwitcher_la_SOURCES += \
    src/engines/witcher/nwscript/container.h \
    src/engines/witcher/nwscript/functions.h \
    src/engines/witcher/nwscript/function_tables.h \
    $(EMPTY)

src_engines_witcher_libwitcher_la_SOURCES += \
    src/engines/witcher/nwscript/container.cpp \
    src/engines/witcher/nwscript/functions.cpp \
    src/engines/witcher/nwscript/functions_math.cpp \
    src/engines/witcher/nwscript/functions_string.cpp \
    src/engines/witcher/nwscript/functions_module.cpp \
    src/engines/witcher/nwscript/functions_object.cpp \
    src/engines/witcher/nwscript/functions_situated.cpp \
    src/engines/witcher/nwscript/functions_creature.cpp \
    src/engines/witcher/nwscript/functions_action.cpp \
    src/engines/witcher/nwscript/functions_conversation.cpp \
    src/engines/witcher/nwscript/functions_sound.cpp \
    $(EMPTY)
