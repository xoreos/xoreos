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

# Base engine for KotOR games.

noinst_LTLIBRARIES += src/engines/kotorbase/libkotorbase.la
src_engines_kotorbase_libkotorbase_la_SOURCES =

src_engines_kotorbase_libkotorbase_la_SOURCES += \
    src/engines/kotorbase/types.h \
    src/engines/kotorbase/room.h \
    src/engines/kotorbase/object.h \
    src/engines/kotorbase/action.h \
    src/engines/kotorbase/item.h \
    src/engines/kotorbase/inventory.h \
    src/engines/kotorbase/creature.h \
    src/engines/kotorbase/situated.h \
    src/engines/kotorbase/placeable.h \
    src/engines/kotorbase/door.h \
    src/engines/kotorbase/module.h \
    src/engines/kotorbase/trigger.h \
    src/engines/kotorbase/waypoint.h \
    src/engines/kotorbase/sound.h \
    $(EMPTY)

src_engines_kotorbase_libkotorbase_la_SOURCES += \
    src/engines/kotorbase/room.cpp \
    src/engines/kotorbase/object.cpp \
    src/engines/kotorbase/action.cpp \
    src/engines/kotorbase/item.cpp \
    src/engines/kotorbase/inventory.cpp \
    src/engines/kotorbase/creature.cpp \
    src/engines/kotorbase/situated.cpp \
    src/engines/kotorbase/placeable.cpp \
    src/engines/kotorbase/door.cpp \
    src/engines/kotorbase/module.cpp \
    src/engines/kotorbase/trigger.cpp \
    src/engines/kotorbase/waypoint.cpp \
    src/engines/kotorbase/sound.cpp \
    $(EMPTY)

include src/engines/kotorbase/script/rules.mk
include src/engines/kotorbase/gui/rules.mk
include src/engines/kotorbase/path/rules.mk
