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
 *  Generic renderable for walkmesh.
 */

#ifndef GRAPHICS_AURORA_WALKMESH_H
#define GRAPHICS_AURORA_WALKMESH_H

#include <vector>

#include "src/graphics/renderable.h"

namespace Engines {
class Pathfinding;
}

namespace Graphics {

namespace Aurora {

class Walkmesh : Graphics::Renderable {
public:
	Walkmesh(Engines::Pathfinding *pathfinding);
	~Walkmesh();

	void setFaces(std::vector<uint32_t> &faces);

	void show();
	void hide();

	void setAdjustedHeight(float adjustedHeight);
	void setWalkableColor(float color[4]);
	void setUnwalkableColor(float color[4]);

	// Renderable.
	void calculateDistance();
	void render(Graphics::RenderPass pass);

private:
	Engines::Pathfinding *_pathfinding;
	std::vector<uint32_t> _highlightedFaces;

	float _heightAdjust;
	float _unwalkableFaceColor[4];
	float _walkableFaceColor[4];
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_WALKMESH_H
