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

# Base engine for KotOR games.

noinst_LTLIBRARIES += src/engines/kotorbase/libkotorbase.la
src_engines_kotorbase_libkotorbase_la_SOURCES =

src_engines_kotorbase_libkotorbase_la_SOURCES += \
    src/engines/kotorbase/types.h \
    src/engines/kotorbase/room.h \
    src/engines/kotorbase/object.h \
    $(EMPTY)

src_engines_kotorbase_libkotorbase_la_SOURCES += \
    src/engines/kotorbase/room.cpp \
    src/engines/kotorbase/object.cpp \
    $(EMPTY)

include src/engines/kotorbase/script/rules.mk
