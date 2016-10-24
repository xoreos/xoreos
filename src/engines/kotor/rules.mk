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

# Star Wars: Knights of the Old Republic.

noinst_LTLIBRARIES += src/engines/kotor/libkotor.la
src_engines_kotor_libkotor_la_SOURCES =

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/kotor.h \
    src/engines/kotor/probes.h \
    src/engines/kotor/types.h \
    src/engines/kotor/console.h \
    src/engines/kotor/modelloader.h \
    src/engines/kotor/game.h \
    src/engines/kotor/object.h \
    src/engines/kotor/situated.h \
    src/engines/kotor/waypoint.h \
    src/engines/kotor/placeable.h \
    src/engines/kotor/door.h \
    src/engines/kotor/creature.h \
    src/engines/kotor/area.h \
    src/engines/kotor/room.h \
    src/engines/kotor/objectcontainer.h \
    src/engines/kotor/module.h \
    $(EMPTY)

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/kotor.cpp \
    src/engines/kotor/probes.cpp \
    src/engines/kotor/console.cpp \
    src/engines/kotor/modelloader.cpp \
    src/engines/kotor/game.cpp \
    src/engines/kotor/object.cpp \
    src/engines/kotor/waypoint.cpp \
    src/engines/kotor/situated.cpp \
    src/engines/kotor/placeable.cpp \
    src/engines/kotor/door.cpp \
    src/engines/kotor/creature.cpp \
    src/engines/kotor/area.cpp \
    src/engines/kotor/room.cpp \
    src/engines/kotor/objectcontainer.cpp \
    src/engines/kotor/module.cpp \
    $(EMPTY)

include src/engines/kotor/script/rules.mk
include src/engines/kotor/gui/rules.mk
