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

# Jade Empire.

noinst_LTLIBRARIES += src/engines/jade/libjade.la
src_engines_jade_libjade_la_SOURCES =

src_engines_jade_libjade_la_SOURCES += \
    src/engines/jade/jade.h \
    src/engines/jade/probes.h \
    src/engines/jade/types.h \
    src/engines/jade/modelloader.h \
    src/engines/jade/console.h \
    src/engines/jade/game.h \
    src/engines/jade/module.h \
    src/engines/jade/area.h \
    src/engines/jade/arealayout.h \
    src/engines/jade/room.h \
    src/engines/jade/object.h \
    src/engines/jade/objectcontainer.h \
    src/engines/jade/creature.h \
    src/engines/jade/placeable.h \
    src/engines/jade/trigger.h \
    src/engines/jade/waypoint.h \
    src/engines/jade/location.h \
    src/engines/jade/event.h \
    $(EMPTY)

src_engines_jade_libjade_la_SOURCES += \
    src/engines/jade/jade.cpp \
    src/engines/jade/probes.cpp \
    src/engines/jade/modelloader.cpp \
    src/engines/jade/console.cpp \
    src/engines/jade/game.cpp \
    src/engines/jade/module.cpp \
    src/engines/jade/area.cpp \
    src/engines/jade/arealayout.cpp \
    src/engines/jade/room.cpp \
    src/engines/jade/object.cpp \
    src/engines/jade/objectcontainer.cpp \
    src/engines/jade/creature.cpp \
    src/engines/jade/placeable.cpp \
    src/engines/jade/trigger.cpp \
    src/engines/jade/waypoint.cpp \
    src/engines/jade/location.cpp \
    src/engines/jade/event.cpp \
    $(EMPTY)

include src/engines/jade/script/rules.mk
include src/engines/jade/gui/rules.mk
