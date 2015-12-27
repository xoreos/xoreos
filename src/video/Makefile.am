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

include $(top_srcdir)/Makefile.common

SUBDIRS = \
          codecs \
          aurora \
          $(EMPTY)

noinst_LTLIBRARIES = libvideo.la

noinst_HEADERS = \
                 decoder.h \
                 bink.h \
                 binkdata.h \
                 fader.h \
                 quicktime.h \
                 xmv.h \
                 actimagine.h \
                 $(EMPTY)


libvideo_la_SOURCES = \
                      decoder.cpp \
                      bink.cpp \
                      fader.cpp \
                      quicktime.cpp \
                      xmv.cpp \
                      actimagine.cpp \
                      $(EMPTY)

libvideo_la_LIBADD = \
                     aurora/libaurora.la \
                     codecs/libcodecs.la \
                     $(EMPTY)
