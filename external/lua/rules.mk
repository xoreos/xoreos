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

noinst_LTLIBRARIES += external/lua/liblua.la
external_lua_liblua_la_SOURCES =

external_lua_liblua_la_SOURCES += \
    external/lua/lapi.h \
    external/lua/lauxlib.h \
    external/lua/lcode.h \
    external/lua/ldebug.h \
    external/lua/ldo.h \
    external/lua/lfunc.h \
    external/lua/lgc.h \
    external/lua/llex.h \
    external/lua/llimits.h \
    external/lua/lmem.h \
    external/lua/lobject.h \
    external/lua/lopcodes.h \
    external/lua/lparser.h \
    external/lua/lstate.h \
    external/lua/lstring.h \
    external/lua/ltable.h \
    external/lua/ltm.h \
    external/lua/lua.h \
    external/lua/lualib.h \
    external/lua/lundump.h \
    external/lua/lvm.h \
    external/lua/lzio.h \
    $(EMPTY)

external_lua_liblua_la_SOURCES += \
    external/lua/lapi.cpp \
    external/lua/lauxlib.cpp \
    external/lua/lbaselib.cpp \
    external/lua/lcode.cpp \
    external/lua/ldblib.cpp \
    external/lua/ldebug.cpp \
    external/lua/ldo.cpp \
    external/lua/ldump.cpp \
    external/lua/lfunc.cpp \
    external/lua/lgc.cpp \
    external/lua/liolib.cpp \
    external/lua/llex.cpp \
    external/lua/lmathlib.cpp \
    external/lua/lmem.cpp \
    external/lua/loadlib.cpp \
    external/lua/lobject.cpp \
    external/lua/lopcodes.cpp \
    external/lua/lparser.cpp \
    external/lua/lstate.cpp \
    external/lua/lstring.cpp \
    external/lua/lstrlib.cpp \
    external/lua/ltable.cpp \
    external/lua/ltablib.cpp \
    external/lua/ltests.cpp \
    external/lua/ltm.cpp \
    external/lua/lundump.cpp \
    external/lua/lvm.cpp \
    external/lua/lzio.cpp \
    $(EMPTY)

EXTRA_DIST += \
    external/lua/README.xoreos \
    external/lua/COPYRIGHT \
    $(EMPTY)
