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

# Dragon Age: Origins.

noinst_LTLIBRARIES += src/engines/dragonage/libdragonage.la
src_engines_dragonage_libdragonage_la_SOURCES =

src_engines_dragonage_libdragonage_la_SOURCES += \
    src/engines/dragonage/dragonage.h \
    src/engines/dragonage/probes.h \
    src/engines/dragonage/types.h \
    src/engines/dragonage/util.h \
    src/engines/dragonage/modelloader.h \
    src/engines/dragonage/console.h \
    src/engines/dragonage/event.h \
    src/engines/dragonage/game.h \
    src/engines/dragonage/campaigns.h \
    src/engines/dragonage/campaign.h \
    src/engines/dragonage/area.h \
    src/engines/dragonage/room.h \
    src/engines/dragonage/object.h \
    src/engines/dragonage/waypoint.h \
    src/engines/dragonage/placeable.h \
    src/engines/dragonage/creature.h \
    src/engines/dragonage/objectcontainer.h \
    $(EMPTY)

src_engines_dragonage_libdragonage_la_SOURCES += \
    src/engines/dragonage/dragonage.cpp \
    src/engines/dragonage/probes.cpp \
    src/engines/dragonage/util.cpp \
    src/engines/dragonage/modelloader.cpp \
    src/engines/dragonage/console.cpp \
    src/engines/dragonage/event.cpp \
    src/engines/dragonage/game.cpp \
    src/engines/dragonage/campaigns.cpp \
    src/engines/dragonage/campaign.cpp \
    src/engines/dragonage/area.cpp \
    src/engines/dragonage/room.cpp \
    src/engines/dragonage/object.cpp \
    src/engines/dragonage/waypoint.cpp \
    src/engines/dragonage/placeable.cpp \
    src/engines/dragonage/creature.cpp \
    src/engines/dragonage/objectcontainer.cpp \
    $(EMPTY)

include src/engines/dragonage/script/rules.mk
