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

# GLM (OpenGL Mathematics) 0.9.8.5 (<https://glm.g-truc.net/>)

noinst_HEADERS += \
    external/glm/common.hpp \
    external/glm/exponential.hpp \
    external/glm/ext.hpp \
    external/glm/fwd.hpp \
    external/glm/geometric.hpp \
    external/glm/glm.hpp \
    external/glm/integer.hpp \
    external/glm/mat2x2.hpp \
    external/glm/mat2x3.hpp \
    external/glm/mat2x4.hpp \
    external/glm/mat3x2.hpp \
    external/glm/mat3x3.hpp \
    external/glm/mat3x4.hpp \
    external/glm/mat4x2.hpp \
    external/glm/mat4x3.hpp \
    external/glm/mat4x4.hpp \
    external/glm/matrix.hpp \
    external/glm/packing.hpp \
    external/glm/trigonometric.hpp \
    external/glm/vec2.hpp \
    external/glm/vec3.hpp \
    external/glm/vec4.hpp \
    external/glm/vector_relational.hpp \
    $(EMPTY)

EXTRA_DIST += \
    external/glm/README.xoreos \
    external/glm/copying.txt \
    $(EMPTY)

# Subdirectories

include external/glm/detail/rules.mk
include external/glm/gtc/rules.mk
include external/glm/gtx/rules.mk
include external/glm/simd/rules.mk
