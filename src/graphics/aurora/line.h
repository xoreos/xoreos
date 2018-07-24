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
 *  Generic renderable line.
 */

#ifndef GRAPHICS_AURORA_LINE_H
#define GRAPHICS_AURORA_LINE_H

#include <vector>

#include "glm/vec3.hpp"

#include "src/graphics/renderable.h"

namespace Graphics {

namespace Aurora {

class Line : Renderable {
public:
	Line();
	~Line();

	void setVertices(std::vector<glm::vec3> &points);

	void show();
	void hide();

	void setColor(float color[4]);

	// Renderable.
	void calculateDistance();
	void render(RenderPass pass);

private:
	std::vector<glm::vec3> _points;
	float _color[4];
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_LINE_H
