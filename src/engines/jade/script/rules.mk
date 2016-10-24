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

# Scripting system in Jade Empire.

src_engines_jade_libjade_la_SOURCES += \
    src/engines/jade/script/container.h \
    src/engines/jade/script/functions.h \
    src/engines/jade/script/function_tables.h \
    $(EMPTY)

src_engines_jade_libjade_la_SOURCES += \
    src/engines/jade/script/container.cpp \
    src/engines/jade/script/functions.cpp \
    src/engines/jade/script/functions_math.cpp \
    src/engines/jade/script/functions_string.cpp \
    src/engines/jade/script/functions_object.cpp \
    src/engines/jade/script/functions_event.cpp \
    src/engines/jade/script/functions_module.cpp \
    src/engines/jade/script/functions_creature.cpp \
    src/engines/jade/script/functions_action.cpp \
    src/engines/jade/script/functions_2da.cpp \
    src/engines/jade/script/functions_game.cpp \
    $(EMPTY)
