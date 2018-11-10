/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Dedicated mesh used for dynamic font rendering.
 */

#ifndef GRAPHICS_MESH_MESHFONT_H
#define GRAPHICS_MESH_MESHFONT_H

#include "src/graphics/mesh/mesh.h"

namespace Graphics {

namespace Mesh {

class MeshFont : public Mesh {
public:
	MeshFont();

	/** Dynamic data prior to render call. */
	void render(float *pos, float *uv, float *rgba);
};

} // End of namespace Mesh

} // End of namespace Graphics

#endif // GRAPHICS_MESH_MESHFONT_H
