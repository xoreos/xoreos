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

# Character generation menu in Star Wars: Knights of the Old Republic.

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/gui/chargen/classselection.h \
    src/engines/kotor/gui/chargen/charactergeneration.h \
    src/engines/kotor/gui/chargen/quickorcustom.h \
    src/engines/kotor/gui/chargen/quickchar.h \
    src/engines/kotor/gui/chargen/customchar.h \
    src/engines/kotor/gui/chargen/chargeninfo.h \
    src/engines/kotor/gui/chargen/chargenbase.h \
    src/engines/kotor/gui/chargen/chargenportrait.h \
    src/engines/kotor/gui/chargen/chargenname.h \
    $(EMPTY)

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/gui/chargen/classselection.cpp \
    src/engines/kotor/gui/chargen/charactergeneration.cpp \
    src/engines/kotor/gui/chargen/quickorcustom.cpp \
    src/engines/kotor/gui/chargen/quickchar.cpp \
    src/engines/kotor/gui/chargen/customchar.cpp \
    src/engines/kotor/gui/chargen/chargeninfo.cpp \
    src/engines/kotor/gui/chargen/chargenbase.cpp \
    src/engines/kotor/gui/chargen/chargenportrait.cpp \
    src/engines/kotor/gui/chargen/chargenname.cpp \
    $(EMPTY)
