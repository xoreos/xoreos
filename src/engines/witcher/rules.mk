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

# The Witcher.

noinst_LTLIBRARIES += src/engines/witcher/libwitcher.la
src_engines_witcher_libwitcher_la_SOURCES =

src_engines_witcher_libwitcher_la_SOURCES += \
    src/engines/witcher/witcher.h \
    src/engines/witcher/probes.h \
    src/engines/witcher/types.h \
    src/engines/witcher/modelloader.h \
    src/engines/witcher/console.h \
    src/engines/witcher/game.h \
    src/engines/witcher/campaign.h \
    src/engines/witcher/objectcontainer.h \
    src/engines/witcher/module.h \
    src/engines/witcher/area.h \
    src/engines/witcher/location.h \
    src/engines/witcher/object.h \
    src/engines/witcher/waypoint.h \
    src/engines/witcher/situated.h \
    src/engines/witcher/door.h \
    src/engines/witcher/placeable.h \
    src/engines/witcher/creature.h \
    $(EMPTY)

src_engines_witcher_libwitcher_la_SOURCES += \
    src/engines/witcher/witcher.cpp \
    src/engines/witcher/probes.cpp \
    src/engines/witcher/modelloader.cpp \
    src/engines/witcher/console.cpp \
    src/engines/witcher/game.cpp \
    src/engines/witcher/campaign.cpp \
    src/engines/witcher/objectcontainer.cpp \
    src/engines/witcher/module.cpp \
    src/engines/witcher/area.cpp \
    src/engines/witcher/location.cpp \
    src/engines/witcher/object.cpp \
    src/engines/witcher/waypoint.cpp \
    src/engines/witcher/situated.cpp \
    src/engines/witcher/door.cpp \
    src/engines/witcher/placeable.cpp \
    src/engines/witcher/creature.cpp \
    $(EMPTY)

include src/engines/witcher/nwscript/rules.mk
include src/engines/witcher/lua/rules.mk
