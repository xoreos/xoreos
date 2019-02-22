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

# Base scripting system for KotOR games.

src_engines_kotorbase_libkotorbase_la_SOURCES += \
    src/engines/kotorbase/script/container.h \
    src/engines/kotorbase/script/event.h \
    src/engines/kotorbase/script/functions.h \
    $(EMPTY)

src_engines_kotorbase_libkotorbase_la_SOURCES += \
    src/engines/kotorbase/script/container.cpp \
    src/engines/kotorbase/script/event.cpp \
    src/engines/kotorbase/script/functions.cpp \
    src/engines/kotorbase/script/functions_action.cpp \
    src/engines/kotorbase/script/functions_creatures.cpp \
    src/engines/kotorbase/script/functions_events.cpp \
    src/engines/kotorbase/script/functions_global.cpp \
    src/engines/kotorbase/script/functions_local.cpp \
    src/engines/kotorbase/script/functions_math.cpp \
    src/engines/kotorbase/script/functions_module.cpp \
    src/engines/kotorbase/script/functions_movie.cpp \
    src/engines/kotorbase/script/functions_object.cpp \
    src/engines/kotorbase/script/functions_party.cpp \
    src/engines/kotorbase/script/functions_situated.cpp \
    src/engines/kotorbase/script/functions_sound.cpp \
    src/engines/kotorbase/script/functions_string.cpp \
    $(EMPTY)
