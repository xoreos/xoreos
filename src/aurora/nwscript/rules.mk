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

# NWScript interpreter.

noinst_LTLIBRARIES += src/aurora/nwscript/libnwscript.la
src_aurora_nwscript_libnwscript_la_SOURCES =

src_aurora_nwscript_libnwscript_la_SOURCES += \
    src/aurora/nwscript/types.h \
    src/aurora/nwscript/util.h \
    src/aurora/nwscript/variable.h \
    src/aurora/nwscript/variablecontainer.h \
    src/aurora/nwscript/functioncontext.h \
    src/aurora/nwscript/enginetype.h \
    src/aurora/nwscript/object.h \
    src/aurora/nwscript/objectcontainer.h \
    src/aurora/nwscript/functionman.h \
    src/aurora/nwscript/ncsfile.h \
    $(EMPTY)

src_aurora_nwscript_libnwscript_la_SOURCES += \
    src/aurora/nwscript/util.cpp \
    src/aurora/nwscript/variable.cpp \
    src/aurora/nwscript/variablecontainer.cpp \
    src/aurora/nwscript/functioncontext.cpp \
    src/aurora/nwscript/objectcontainer.cpp \
    src/aurora/nwscript/functionman.cpp \
    src/aurora/nwscript/ncsfile.cpp \
    $(EMPTY)
