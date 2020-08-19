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
 *  Base class for walkmesh from a static object.
 */

#ifndef ENGINES_OBJECTWALKMESH_H
#define ENGINES_OBJECTWALKMESH_H

#include <vector>

#include "external/glm/vec2.hpp"

#include "src/common/ustring.h"

namespace Engines {

class ObjectWalkmesh {
public:
	virtual ~ObjectWalkmesh() {}

	virtual void load(const Common::UString &resRef,
	                  float orientation[4], float position[3]) = 0;

	virtual bool in(const glm::vec2 &minBox, const glm::vec2 &maxBox) const = 0;
	virtual bool in(const glm::vec2 &point) const = 0;
	virtual bool findIntersection(const glm::vec3 &start, const glm::vec3 &end,
	                              glm::vec3 &intersect) const = 0;
	virtual const std::vector<float> &getVertices() const = 0;
	virtual const std::vector<uint32_t> &getFaces() const = 0;
};

} // End of namespace Engines

#endif // ENGINES_OBJECTWALKMESH_H
