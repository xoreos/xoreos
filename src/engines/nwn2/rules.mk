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

# Neverwinter Nights 2.

noinst_LTLIBRARIES += src/engines/nwn2/libnwn2.la
src_engines_nwn2_libnwn2_la_SOURCES =

src_engines_nwn2_libnwn2_la_SOURCES += \
    src/engines/nwn2/nwn2.h \
    src/engines/nwn2/probes.h \
    src/engines/nwn2/types.h \
    src/engines/nwn2/modelloader.h \
    src/engines/nwn2/util.h \
    src/engines/nwn2/trxfile.h \
    src/engines/nwn2/console.h \
    src/engines/nwn2/game.h \
    src/engines/nwn2/campaign.h \
    src/engines/nwn2/objectcontainer.h \
    src/engines/nwn2/module.h \
    src/engines/nwn2/area.h \
    src/engines/nwn2/location.h \
    src/engines/nwn2/object.h \
    src/engines/nwn2/waypoint.h \
    src/engines/nwn2/situated.h \
    src/engines/nwn2/door.h \
    src/engines/nwn2/placeable.h \
    src/engines/nwn2/creature.h \
    $(EMPTY)

src_engines_nwn2_libnwn2_la_SOURCES += \
    src/engines/nwn2/nwn2.cpp \
    src/engines/nwn2/probes.cpp \
    src/engines/nwn2/types.cpp \
    src/engines/nwn2/modelloader.cpp \
    src/engines/nwn2/util.cpp \
    src/engines/nwn2/trxfile.cpp \
    src/engines/nwn2/console.cpp \
    src/engines/nwn2/game.cpp \
    src/engines/nwn2/campaign.cpp \
    src/engines/nwn2/objectcontainer.cpp \
    src/engines/nwn2/module.cpp \
    src/engines/nwn2/area.cpp \
    src/engines/nwn2/location.cpp \
    src/engines/nwn2/object.cpp \
    src/engines/nwn2/waypoint.cpp \
    src/engines/nwn2/situated.cpp \
    src/engines/nwn2/door.cpp \
    src/engines/nwn2/placeable.cpp \
    src/engines/nwn2/creature.cpp \
    $(EMPTY)

include src/engines/nwn2/script/rules.mk
