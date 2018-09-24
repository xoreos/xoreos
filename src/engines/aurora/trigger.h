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
 *  Generic trigger.
 */

#ifndef ENGINES_AURORA_TRIGGER_H
#define ENGINES_AURORA_TRIGGER_H

#include <vector>

#include "glm/vec3.hpp"

#include "src/graphics/renderable.h"
#include "src/common/boundingbox.h"

namespace Engines {

class Trigger : public Graphics::Renderable {
public:
	Trigger();

	void setVisible(bool visible);
	bool contains(float x, float y);

	// .--- Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);
	// '---
protected:
	std::vector<glm::vec3> _geometry;
	bool _visible;

	void prepare();
private:
	bool _prepared;
	Common::BoundingBox _boundingbox;

	bool isRayIntersect(float  x, float  y,
	                    float x1, float y1,
			    float x2, float y2) const;
};

} // End of namespace Engines

#endif // ENGINES_AURORA_TRIGGER_H
