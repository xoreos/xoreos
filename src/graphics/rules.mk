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

# Graphics (OpenGL, 2D/3D drawing, images, textures, ...) subsystem.

noinst_LTLIBRARIES += src/graphics/libgraphics.la
src_graphics_libgraphics_la_SOURCES =

src_graphics_libgraphics_la_SOURCES += \
    src/graphics/types.h \
    src/graphics/windowman.h \
    src/graphics/graphics.h \
    src/graphics/fpscounter.h \
    src/graphics/icon.h \
    src/graphics/cursor.h \
    src/graphics/queueman.h \
    src/graphics/queueable.h \
    src/graphics/glcontainer.h \
    src/graphics/texture.h \
    src/graphics/font.h \
    src/graphics/camera.h \
    src/graphics/renderable.h \
    src/graphics/resolution.h \
    src/graphics/object.h \
    src/graphics/guielement.h \
    src/graphics/yuv_to_rgb.h \
    src/graphics/ttf.h \
    src/graphics/indexbuffer.h \
    src/graphics/vertexbuffer.h \
    $(EMPTY)

src_graphics_libgraphics_la_SOURCES += \
    src/graphics/windowman.cpp \
    src/graphics/graphics.cpp \
    src/graphics/fpscounter.cpp \
    src/graphics/icon.cpp \
    src/graphics/cursor.cpp \
    src/graphics/queueman.cpp \
    src/graphics/queueable.cpp \
    src/graphics/glcontainer.cpp \
    src/graphics/texture.cpp \
    src/graphics/font.cpp \
    src/graphics/camera.cpp \
    src/graphics/renderable.cpp \
    src/graphics/yuv_to_rgb.cpp \
    src/graphics/ttf.cpp \
    src/graphics/indexbuffer.cpp \
    src/graphics/vertexbuffer.cpp \
    $(EMPTY)

src_graphics_libgraphics_la_LIBADD = \
    src/graphics/images/libimages.la \
    src/graphics/aurora/libaurora.la \
    src/graphics/shader/libshader.la \
    src/graphics/mesh/libmesh.la \
    src/graphics/render/librender.la \
    $(EMPTY)

if INTERNAL_GLEW
src_graphics_libgraphics_la_LIBADD += glew/libglew.la
endif

# Subdirectories

include src/graphics/images/rules.mk
include src/graphics/aurora/rules.mk
include src/graphics/mesh/rules.mk
include src/graphics/render/rules.mk
include src/graphics/shader/rules.mk
