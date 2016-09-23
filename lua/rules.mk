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

# Lua 5.0.3 (<http://www.lua.org/>).

noinst_LTLIBRARIES += lua/liblua.la
lua_liblua_la_SOURCES =

lua_liblua_la_SOURCES += \
    lua/lapi.h \
    lua/lauxlib.h \
    lua/lcode.h \
    lua/ldebug.h \
    lua/ldo.h \
    lua/lfunc.h \
    lua/lgc.h \
    lua/llex.h \
    lua/llimits.h \
    lua/lmem.h \
    lua/lobject.h \
    lua/lopcodes.h \
    lua/lparser.h \
    lua/lstate.h \
    lua/lstring.h \
    lua/ltable.h \
    lua/ltm.h \
    lua/lua.h \
    lua/lualib.h \
    lua/lundump.h \
    lua/lvm.h \
    lua/lzio.h \
    $(EMPTY)

lua_liblua_la_SOURCES += \
    lua/lapi.cpp \
    lua/lauxlib.cpp \
    lua/lbaselib.cpp \
    lua/lcode.cpp \
    lua/ldblib.cpp \
    lua/ldebug.cpp \
    lua/ldo.cpp \
    lua/ldump.cpp \
    lua/lfunc.cpp \
    lua/lgc.cpp \
    lua/liolib.cpp \
    lua/llex.cpp \
    lua/lmathlib.cpp \
    lua/lmem.cpp \
    lua/loadlib.cpp \
    lua/lobject.cpp \
    lua/lopcodes.cpp \
    lua/lparser.cpp \
    lua/lstate.cpp \
    lua/lstring.cpp \
    lua/lstrlib.cpp \
    lua/ltable.cpp \
    lua/ltablib.cpp \
    lua/ltests.cpp \
    lua/ltm.cpp \
    lua/lundump.cpp \
    lua/lvm.cpp \
    lua/lzio.cpp \
    $(EMPTY)

EXTRA_DIST += \
    lua/README.xoreos \
    lua/COPYRIGHT \
    $(EMPTY)
