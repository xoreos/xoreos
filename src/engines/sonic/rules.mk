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

# Sonic Chronicles: The Dark Brotherhood.

noinst_LTLIBRARIES += src/engines/sonic/libsonic.la
src_engines_sonic_libsonic_la_SOURCES =

src_engines_sonic_libsonic_la_SOURCES += \
    src/engines/sonic/sonic.h \
    src/engines/sonic/probes.h \
    src/engines/sonic/types.h \
    src/engines/sonic/files.h \
    src/engines/sonic/modelloader.h \
    src/engines/sonic/console.h \
    src/engines/sonic/util.h \
    src/engines/sonic/game.h \
    src/engines/sonic/areabackground.h \
    src/engines/sonic/areaminimap.h \
    src/engines/sonic/area.h \
    src/engines/sonic/objectcontainer.h \
    src/engines/sonic/module.h \
    src/engines/sonic/object.h \
    src/engines/sonic/placeable.h \
    $(EMPTY)

src_engines_sonic_libsonic_la_SOURCES += \
    src/engines/sonic/sonic.cpp \
    src/engines/sonic/probes.cpp \
    src/engines/sonic/modelloader.cpp \
    src/engines/sonic/console.cpp \
    src/engines/sonic/util.cpp \
    src/engines/sonic/game.cpp \
    src/engines/sonic/areabackground.cpp \
    src/engines/sonic/areaminimap.cpp \
    src/engines/sonic/area.cpp \
    src/engines/sonic/objectcontainer.cpp \
    src/engines/sonic/module.cpp \
    src/engines/sonic/object.cpp \
    src/engines/sonic/placeable.cpp \
    $(EMPTY)
