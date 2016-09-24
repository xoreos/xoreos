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

# Engines, code specific to the individual games.

noinst_LTLIBRARIES += src/engines/libengines.la
src_engines_libengines_la_SOURCES =

src_engines_libengines_la_SOURCES += \
    src/engines/engine.h \
    src/engines/engineprobe.h \
    src/engines/enginemanager.h \
    src/engines/gamethread.h \
    $(EMPTY)

src_engines_libengines_la_SOURCES += \
    src/engines/engine.cpp \
    src/engines/enginemanager.cpp \
    src/engines/gamethread.cpp \
    $(EMPTY)

src_engines_libengines_la_LIBADD = \
    src/engines/aurora/libaurora.la \
    $(EMPTY)

# Subdirectories

include src/engines/aurora/rules.mk
include src/engines/nwn/rules.mk
include src/engines/nwn2/rules.mk
include src/engines/kotor/rules.mk
include src/engines/kotor2/rules.mk
include src/engines/jade/rules.mk
include src/engines/witcher/rules.mk
include src/engines/sonic/rules.mk
include src/engines/dragonage/rules.mk
include src/engines/dragonage2/rules.mk
