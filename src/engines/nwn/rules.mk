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

# Neverwinter Nights.

noinst_LTLIBRARIES += src/engines/nwn/libnwn.la
src_engines_nwn_libnwn_la_SOURCES =

src_engines_nwn_libnwn_la_SOURCES += \
    src/engines/nwn/nwn.h \
    src/engines/nwn/probes.h \
    src/engines/nwn/types.h \
    src/engines/nwn/modelloader.h \
    src/engines/nwn/version.h \
    src/engines/nwn/console.h \
    src/engines/nwn/game.h \
    src/engines/nwn/location.h \
    src/engines/nwn/tileset.h \
    src/engines/nwn/objectcontainer.h \
    src/engines/nwn/module.h \
    src/engines/nwn/area.h \
    src/engines/nwn/object.h \
    src/engines/nwn/waypoint.h \
    src/engines/nwn/situated.h \
    src/engines/nwn/placeable.h \
    src/engines/nwn/door.h \
    src/engines/nwn/creature.h \
    src/engines/nwn/item.h \
    $(EMPTY)

src_engines_nwn_libnwn_la_SOURCES += \
    src/engines/nwn/nwn.cpp \
    src/engines/nwn/probes.cpp \
    src/engines/nwn/types.cpp \
    src/engines/nwn/modelloader.cpp \
    src/engines/nwn/version.cpp \
    src/engines/nwn/creature.cpp \
    src/engines/nwn/console.cpp \
    src/engines/nwn/game.cpp \
    src/engines/nwn/module.cpp \
    src/engines/nwn/area.cpp \
    src/engines/nwn/tileset.cpp \
    src/engines/nwn/objectcontainer.cpp \
    src/engines/nwn/object.cpp \
    src/engines/nwn/waypoint.cpp \
    src/engines/nwn/situated.cpp \
    src/engines/nwn/placeable.cpp \
    src/engines/nwn/door.cpp \
    src/engines/nwn/item.cpp \
    src/engines/nwn/location.cpp \
    $(EMPTY)

include src/engines/nwn/script/rules.mk
include src/engines/nwn/gui/rules.mk
