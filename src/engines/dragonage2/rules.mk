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

# Dragon Age II.

noinst_LTLIBRARIES += src/engines/dragonage2/libdragonage2.la
src_engines_dragonage2_libdragonage2_la_SOURCES =

src_engines_dragonage2_libdragonage2_la_SOURCES += \
    src/engines/dragonage2/dragonage2.h \
    src/engines/dragonage2/probes.h \
    src/engines/dragonage2/types.h \
    src/engines/dragonage2/util.h \
    src/engines/dragonage2/modelloader.h \
    src/engines/dragonage2/console.h \
    src/engines/dragonage2/event.h \
    src/engines/dragonage2/game.h \
    src/engines/dragonage2/campaigns.h \
    src/engines/dragonage2/campaign.h \
    src/engines/dragonage2/area.h \
    src/engines/dragonage2/room.h \
    src/engines/dragonage2/object.h \
    src/engines/dragonage2/waypoint.h \
    src/engines/dragonage2/placeable.h \
    src/engines/dragonage2/creature.h \
    src/engines/dragonage2/objectcontainer.h \
    $(EMPTY)

src_engines_dragonage2_libdragonage2_la_SOURCES += \
    src/engines/dragonage2/dragonage2.cpp \
    src/engines/dragonage2/probes.cpp \
    src/engines/dragonage2/util.cpp \
    src/engines/dragonage2/modelloader.cpp \
    src/engines/dragonage2/console.cpp \
    src/engines/dragonage2/event.cpp \
    src/engines/dragonage2/game.cpp \
    src/engines/dragonage2/campaigns.cpp \
    src/engines/dragonage2/campaign.cpp \
    src/engines/dragonage2/area.cpp \
    src/engines/dragonage2/room.cpp \
    src/engines/dragonage2/object.cpp \
    src/engines/dragonage2/waypoint.cpp \
    src/engines/dragonage2/placeable.cpp \
    src/engines/dragonage2/creature.cpp \
    src/engines/dragonage2/objectcontainer.cpp \
    $(EMPTY)

include src/engines/dragonage2/script/rules.mk
