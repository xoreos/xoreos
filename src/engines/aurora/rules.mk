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

# Common Aurora engine utility functions.

noinst_LTLIBRARIES += src/engines/aurora/libaurora.la
src_engines_aurora_libaurora_la_SOURCES =

src_engines_aurora_libaurora_la_SOURCES += \
    src/engines/aurora/util.h \
    src/engines/aurora/resources.h \
    src/engines/aurora/tokenman.h \
    src/engines/aurora/modelloader.h \
    src/engines/aurora/model.h \
    src/engines/aurora/widget.h \
    src/engines/aurora/gui.h \
    src/engines/aurora/console.h \
    src/engines/aurora/loadprogress.h \
    src/engines/aurora/camera.h \
    $(EMPTY)

src_engines_aurora_libaurora_la_SOURCES += \
    src/engines/aurora/util.cpp \
    src/engines/aurora/resources.cpp \
    src/engines/aurora/tokenman.cpp \
    src/engines/aurora/modelloader.cpp \
    src/engines/aurora/model.cpp \
    src/engines/aurora/widget.cpp \
    src/engines/aurora/gui.cpp \
    src/engines/aurora/console.cpp \
    src/engines/aurora/loadprogress.cpp \
    src/engines/aurora/camera.cpp \
    $(EMPTY)
