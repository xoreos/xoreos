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

# Odyssey engine, a common base for KotOR games and Jade Empire.

noinst_LTLIBRARIES += src/engines/odyssey/libodyssey.la
src_engines_odyssey_libodyssey_la_SOURCES =

src_engines_odyssey_libodyssey_la_SOURCES += \
    src/engines/odyssey/button.h \
    src/engines/odyssey/checkbox.h \
    src/engines/odyssey/widget.h \
    src/engines/odyssey/label.h \
    src/engines/odyssey/listbox.h \
    src/engines/odyssey/panel.h \
    src/engines/odyssey/progressbar.h \
    src/engines/odyssey/protoitem.h \
    src/engines/odyssey/scrollbar.h \
    src/engines/odyssey/slider.h \
    $(EMPTY)

src_engines_odyssey_libodyssey_la_SOURCES += \
    src/engines/odyssey/button.cpp \
    src/engines/odyssey/checkbox.cpp \
    src/engines/odyssey/widget.cpp \
    src/engines/odyssey/label.cpp \
    src/engines/odyssey/listbox.cpp \
    src/engines/odyssey/panel.cpp \
    src/engines/odyssey/progressbar.cpp \
    src/engines/odyssey/protoitem.cpp \
    src/engines/odyssey/scrollbar.cpp \
    src/engines/odyssey/slider.cpp \
    $(EMPTY)
