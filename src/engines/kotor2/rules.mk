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

# Star Wars: Knights of the Old Republic II - The Sith Lords.

noinst_LTLIBRARIES += src/engines/kotor2/libkotor2.la
src_engines_kotor2_libkotor2_la_SOURCES =

src_engines_kotor2_libkotor2_la_SOURCES += \
    src/engines/kotor2/kotor2.h \
    src/engines/kotor2/probes.h \
    src/engines/kotor2/types.h \
    src/engines/kotor2/console.h \
    src/engines/kotor2/modelloader.h \
    src/engines/kotor2/game.h \
    src/engines/kotor2/object.h \
    src/engines/kotor2/waypoint.h \
    src/engines/kotor2/situated.h \
    src/engines/kotor2/placeable.h \
    src/engines/kotor2/door.h \
    src/engines/kotor2/creature.h \
    src/engines/kotor2/area.h \
    src/engines/kotor2/room.h \
    src/engines/kotor2/objectcontainer.h \
    src/engines/kotor2/module.h \
    $(EMPTY)

src_engines_kotor2_libkotor2_la_SOURCES += \
    src/engines/kotor2/kotor2.cpp \
    src/engines/kotor2/probes.cpp \
    src/engines/kotor2/console.cpp \
    src/engines/kotor2/modelloader.cpp \
    src/engines/kotor2/game.cpp \
    src/engines/kotor2/object.cpp \
    src/engines/kotor2/waypoint.cpp \
    src/engines/kotor2/situated.cpp \
    src/engines/kotor2/placeable.cpp \
    src/engines/kotor2/door.cpp \
    src/engines/kotor2/creature.cpp \
    src/engines/kotor2/area.cpp \
    src/engines/kotor2/room.cpp \
    src/engines/kotor2/objectcontainer.cpp \
    src/engines/kotor2/module.cpp \
    $(EMPTY)

include src/engines/kotor2/script/rules.mk
include src/engines/kotor2/gui/rules.mk
