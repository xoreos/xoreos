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

# ActionScript interpreter.

noinst_LTLIBRARIES += src/aurora/actionscript/libactionscript.la
src_aurora_actionscript_libactionscript_la_SOURCES =

src_aurora_actionscript_libactionscript_la_SOURCES += \
    src/aurora/actionscript/asbuffer.h \
    src/aurora/actionscript/avm.h \
    src/aurora/actionscript/function.h \
    src/aurora/actionscript/object.h \
    src/aurora/actionscript/variable.h \
    src/aurora/actionscript/array.h \
    src/aurora/actionscript/string.h \
    src/aurora/actionscript/movieclip.h \
    src/aurora/actionscript/textfield.h \
    src/aurora/actionscript/stage.h \
    src/aurora/actionscript/types.h \
    $(EMPTY)

src_aurora_actionscript_libactionscript_la_SOURCES += \
    src/aurora/actionscript/asbuffer.cpp \
    src/aurora/actionscript/avm.cpp \
    src/aurora/actionscript/avm_interval.cpp \
    src/aurora/actionscript/function.cpp \
    src/aurora/actionscript/object.cpp \
    src/aurora/actionscript/variable.cpp \
    src/aurora/actionscript/array.cpp \
    src/aurora/actionscript/string.cpp \
    src/aurora/actionscript/movieclip.cpp \
    src/aurora/actionscript/textfield.cpp \
    src/aurora/actionscript/stage.cpp \
    $(EMPTY)
