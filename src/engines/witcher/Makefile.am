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

include $(top_srcdir)/Makefile.common

noinst_LTLIBRARIES = libwitcher.la

noinst_HEADERS = \
                 witcher.h \
                 probes.h \
                 types.h \
                 modelloader.h \
                 console.h \
                 game.h \
                 campaign.h \
                 objectcontainer.h \
                 module.h \
                 area.h \
                 location.h \
                 object.h \
                 waypoint.h \
                 situated.h \
                 door.h \
                 placeable.h \
                 creature.h \
                 nwscript/container.h \
                 nwscript/functions.h \
                 nwscript/function_tables.h \
                 lua/bindings.h \
                 $(EMPTY)

libwitcher_la_SOURCES = \
                        witcher.cpp \
                        probes.cpp \
                        modelloader.cpp \
                        console.cpp \
                        game.cpp \
                        campaign.cpp \
                        objectcontainer.cpp \
                        module.cpp \
                        area.cpp \
                        location.cpp \
                        object.cpp \
                        waypoint.cpp \
                        situated.cpp \
                        door.cpp \
                        placeable.cpp \
                        creature.cpp \
                        nwscript/container.cpp \
                        nwscript/functions.cpp \
                        nwscript/functions_math.cpp \
                        nwscript/functions_string.cpp \
                        nwscript/functions_module.cpp \
                        nwscript/functions_object.cpp \
                        nwscript/functions_situated.cpp \
                        nwscript/functions_creature.cpp \
                        nwscript/functions_action.cpp \
                        nwscript/functions_conversation.cpp \
                        nwscript/functions_sound.cpp \
                        lua/bindings.cpp \
                        $(EMPTY)
