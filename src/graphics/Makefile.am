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
          images \
          aurora \
          mesh \
          render \
          shader \
          $(EMPTY)

noinst_LTLIBRARIES = libgraphics.la

noinst_HEADERS = \
                 types.h \
                 util.h \
                 graphics.h \
                 fpscounter.h \
                 icon.h \
                 cursor.h \
                 queueman.h \
                 queueable.h \
                 glcontainer.h \
                 texture.h \
                 font.h \
                 camera.h \
                 renderable.h \
                 object.h \
                 guifrontelement.h \
                 guibackelement.h \
                 yuv_to_rgb.h \
                 ttf.h \
                 indexbuffer.h \
                 vertexbuffer.h \
                 $(EMPTY)

libgraphics_la_SOURCES = \
                         graphics.cpp \
                         fpscounter.cpp \
                         icon.cpp \
                         cursor.cpp \
                         queueman.cpp \
                         queueable.cpp \
                         glcontainer.cpp \
                         texture.cpp \
                         font.cpp \
                         camera.cpp \
                         renderable.cpp \
                         yuv_to_rgb.cpp \
                         ttf.cpp \
                         indexbuffer.cpp \
                         vertexbuffer.cpp \
                         $(EMPTY)

libgraphics_la_LIBADD = \
                        images/libimages.la \
                        aurora/libaurora.la \
                        shader/libshader.la \
                        mesh/libmesh.la \
                        render/librender.la \
                        $(EMPTY)

if INTERNAL_GLEW
libgraphics_la_LIBADD += ../../glew/libglew.la
endif
