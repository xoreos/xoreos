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

include $(top_srcdir)/Makefile.common

noinst_LTLIBRARIES = libkotor2.la

noinst_HEADERS = \
                 kotor2.h \
                 probes.h \
                 types.h \
                 console.h \
                 modelloader.h \
                 game.h \
                 object.h \
                 waypoint.h \
                 situated.h \
                 placeable.h \
                 door.h \
                 creature.h \
                 area.h \
                 room.h \
                 objectcontainer.h \
                 module.h \
                 gui/widgets/kotorwidget.h \
                 gui/gui.h \
                 gui/main/main.h \
                 script/container.h \
                 script/functions.h \
                 script/function_tables.h \
                 $(EMPTY)

libkotor2_la_SOURCES = \
                       kotor2.cpp \
                       probes.cpp \
                       console.cpp \
                       modelloader.cpp \
                       game.cpp \
                       object.cpp \
                       waypoint.cpp \
                       situated.cpp \
                       placeable.cpp \
                       door.cpp \
                       creature.cpp \
                       area.cpp \
                       room.cpp \
                       objectcontainer.cpp \
                       module.cpp \
                       gui/main/main.cpp \
                       script/container.cpp \
                       script/container.cpp \
                       script/functions.cpp \
                       script/functions_math.cpp \
                       script/functions_string.cpp \
                       script/functions_module.cpp \
                       script/functions_object.cpp \
                       script/functions_situated.cpp \
                       script/functions_action.cpp \
                       script/functions_sound.cpp \
                       script/functions_movie.cpp \
                       $(EMPTY)

libkotor2_la_LIBADD = \
                      ../kotor/libkotor.la \
                      $(EMPTY)
