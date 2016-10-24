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

# Scripting system in Neverwinter Nights 2.

src_engines_nwn2_libnwn2_la_SOURCES += \
    src/engines/nwn2/script/container.h \
    src/engines/nwn2/script/functions.h \
    src/engines/nwn2/script/function_tables.h \
    $(EMPTY)

src_engines_nwn2_libnwn2_la_SOURCES += \
    src/engines/nwn2/script/container.cpp \
    src/engines/nwn2/script/functions.cpp \
    src/engines/nwn2/script/functions_math.cpp \
    src/engines/nwn2/script/functions_string.cpp \
    src/engines/nwn2/script/functions_campaign.cpp \
    src/engines/nwn2/script/functions_module.cpp \
    src/engines/nwn2/script/functions_object.cpp \
    src/engines/nwn2/script/functions_situated.cpp \
    src/engines/nwn2/script/functions_creature.cpp \
    src/engines/nwn2/script/functions_action.cpp \
    src/engines/nwn2/script/functions_conversation.cpp \
    src/engines/nwn2/script/functions_sound.cpp \
    $(EMPTY)
