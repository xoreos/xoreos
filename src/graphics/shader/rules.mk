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

# GLSL shaders.

noinst_LTLIBRARIES += src/graphics/shader/libshader.la
src_graphics_shader_libshader_la_SOURCES =

src_graphics_shader_libshader_la_SOURCES += \
    src/graphics/shader/shader.h \
    src/graphics/shader/shadercode.h \
    src/graphics/shader/shaderinstance.h \
    src/graphics/shader/shadermaterial.h \
    src/graphics/shader/shaderrenderable.h \
    src/graphics/shader/shadersurface.h \
    src/graphics/shader/materialman.h \
    src/graphics/shader/surfaceman.h \
    $(EMPTY)

src_graphics_shader_libshader_la_SOURCES += \
    src/graphics/shader/shader.cpp \
    src/graphics/shader/shadercode.cpp \
    src/graphics/shader/shaderinstance.cpp \
    src/graphics/shader/shadermaterial.cpp \
    src/graphics/shader/shaderrenderable.cpp \
    src/graphics/shader/shadersurface.cpp \
    src/graphics/shader/materialman.cpp \
    src/graphics/shader/surfaceman.cpp \
    $(EMPTY)
