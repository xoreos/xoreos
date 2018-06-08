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
 *  Generic renderable walkmesh.
 */

#ifndef GRAPHICS_AURORA_WALKMESH_H
#define GRAPHICS_AURORA_WALKMESH_H

#include <vector>

#include "glm/vec3.hpp"

#include "src/common/types.h"
#include "src/common/scopedptr.h"

#include "src/graphics/renderable.h"

namespace Graphics {

namespace Aurora {

class Walkmesh : public Graphics::Renderable {
public:
	Walkmesh();

	/** Return elevation at given coordinates or FLT_MIN if can't walk
	 *  there.
	 *
	 *  @param faceIndex Index of the intersected walkmesh face
	 */
	float getElevationAt(float x, float y, uint32 &faceIndex) const;

	bool testCollision(const glm::vec3 &orig, const glm::vec3 &dest) const;

	// .--- Rendering

	/** Highlight face with specified index.
	 *
	 *  @param index Index of the face to highlight or -1 to disable
	 *               highlighting
	 */
	void highlightFace(uint32 index);

	void setInvisible(bool invisible);
	void calculateDistance();
	void render(Graphics::RenderPass pass);

	// '---
protected:
	std::vector<float> _vertices;
	std::vector<uint32> _indices;
	std::vector<bool> _faceWalkableMap;
	std::vector<uint32> _indicesWalkable;
	std::vector<uint32> _indicesNonWalkable;
	int _highlightFaceIndex;
	bool _invisible;

	void refreshIndexGroups();
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_WALKMESH_H
