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

# Lua script system.

noinst_LTLIBRARIES += src/aurora/lua/libluascript.la
src_aurora_lua_libluascript_la_SOURCES =

src_aurora_lua_libluascript_la_SOURCES += \
    src/aurora/lua/scriptman.h \
    src/aurora/lua/stack.h \
    src/aurora/lua/variable.h \
    src/aurora/lua/table.h \
    src/aurora/lua/function.h \
    src/aurora/lua/stackguard.h \
    src/aurora/lua/util.h \
    src/aurora/lua/types.h \
    $(EMPTY)

src_aurora_lua_libluascript_la_SOURCES += \
    src/aurora/lua/scriptman.cpp \
    src/aurora/lua/stack.cpp \
    src/aurora/lua/variable.cpp \
    src/aurora/lua/table.cpp \
    src/aurora/lua/function.cpp \
    src/aurora/lua/stackguard.cpp \
    src/aurora/lua/util.cpp \
    $(EMPTY)
