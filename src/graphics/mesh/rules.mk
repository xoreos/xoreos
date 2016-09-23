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

# Meshes holding 3D geometry.

noinst_LTLIBRARIES += src/graphics/mesh/libmesh.la
src_graphics_mesh_libmesh_la_SOURCES =

src_graphics_mesh_libmesh_la_SOURCES += \
    src/graphics/mesh/mesh.h \
    src/graphics/mesh/meshman.h \
    src/graphics/mesh/meshwirebox.h \
    $(EMPTY)

src_graphics_mesh_libmesh_la_SOURCES += \
    src/graphics/mesh/mesh.cpp \
    src/graphics/mesh/meshman.cpp \
    src/graphics/mesh/meshwirebox.cpp \
    $(EMPTY)
