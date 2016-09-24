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

# Main xoreos entry point.

# Windows resources
.rc.o:
	$(AM_V_GEN)$(RC) -DHAVE_CONFIG_H -I$(srcdir) -o $@ $<

bin_PROGRAMS += src/xoreos
src_xoreos_SOURCES =

src_xoreos_SOURCES += \
    src/cline.h \
    src/engines.h \
    $(EMPTY)

src_xoreos_SOURCES += \
    src/cline.cpp \
    src/engines.cpp \
    src/xoreos.cpp \
    $(EMPTY)

if WIN32
src_xoreos_SOURCES += dists/win32/xoreos.rc
endif

src_xoreos_LDADD = \
    src/engines/dragonage2/libdragonage2.la \
    src/engines/dragonage/libdragonage.la \
    src/engines/sonic/libsonic.la \
    src/engines/witcher/libwitcher.la \
    src/engines/jade/libjade.la \
    src/engines/kotor2/libkotor2.la \
    src/engines/kotor/libkotor.la \
    src/engines/nwn2/libnwn2.la \
    src/engines/nwn/libnwn.la \
    src/engines/libengines.la \
    src/events/libevents.la \
    src/video/libvideo.la \
    src/sound/libsound.la \
    src/graphics/libgraphics.la \
    src/aurora/libaurora.la \
    src/common/libcommon.la \
    src/version/libversion.la \
    lua/liblua.la \
    toluapp/libtoluapp.la \
    $(LDADD) \
    $(EMPTY)

# Subdirectories

include src/version/rules.mk
include src/common/rules.mk
include src/aurora/rules.mk
include src/graphics/rules.mk
include src/sound/rules.mk
include src/video/rules.mk
include src/events/rules.mk
include src/engines/rules.mk
