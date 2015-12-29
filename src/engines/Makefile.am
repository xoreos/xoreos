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

include $(top_srcdir)/Makefile.common

SUBDIRS = \
          aurora \
          nwn \
          nwn2 \
          kotor \
          kotor2 \
          jade \
          witcher \
          sonic \
          dragonage \
          dragonage2 \
          $(EMPTY)

noinst_LTLIBRARIES = libengines.la

noinst_HEADERS = \
                 engine.h \
                 engineprobe.h \
                 enginemanager.h \
                 gamethread.h \
                 $(EMPTY)

libengines_la_SOURCES = \
                        engine.cpp \
                        enginemanager.cpp \
                        gamethread.cpp \
                        $(EMPTY)

libengines_la_LIBADD = \
                       nwn/libnwn.la \
                       nwn2/libnwn2.la \
                       kotor/libkotor.la \
                       kotor2/libkotor2.la \
                       jade/libjade.la \
                       witcher/libwitcher.la \
                       sonic/libsonic.la \
                       dragonage/libdragonage.la \
                       dragonage2/libdragonage2.la \
                       aurora/libaurora.la \
                       $(EMPTY)
