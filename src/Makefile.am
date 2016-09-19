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

include $(top_srcdir)/Makefile.common

# Windows resources
.rc.o:
	$(AM_V_GEN)$(RC) -DHAVE_CONFIG_H -I$(top_srcdir) -o $@ $<

SUBDIRS = \
          version \
          common \
          aurora \
          graphics \
          sound \
          video \
          events \
          engines \
          $(EMPTY)

noinst_HEADERS = \
                 cline.h \
                 $(EMPTY)

bin_PROGRAMS = xoreos

xoreos_SOURCES = \
                 cline.cpp \
                 xoreos.cpp \
                 $(EMPTY)

if WIN32
xoreos_SOURCES += ../dists/win32/xoreos.rc
endif

xoreos_LDADD = \
               engines/libengines.la \
               events/libevents.la \
               video/libvideo.la \
               sound/libsound.la \
               graphics/libgraphics.la \
               aurora/libaurora.la \
               common/libcommon.la \
               version/libversion.la \
               ../lua/liblua.la \
               ../toluapp/libtoluapp.la \
               $(LDADD) \
               $(EMPTY)
