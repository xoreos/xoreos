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

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/intersect.hpp"

#include "src/aurora/gff3file.h"
#include "src/aurora/resman.h"

#include "src/engines/aurora/trigger.h"

namespace Engines {

Trigger::Trigger()
		: Renderable(Graphics::kRenderableTypeObject),
		  _visible(false), _prepared(false) {

}

void Trigger::setVisible(bool visible) {
	_visible = visible;
}

/*
 * Return true if the (x, y) coordinates are located within
 * the horizontal projection of this closed polygon.
 */
bool Trigger::contains(float x, float y) const {
	// Must have a surface defined
	size_t vertexCount = _geometry.size();
	if (vertexCount < 3)
		return false;

	// Check the container data
	assert(_prepared);

	// Check if point is in bounding box
	if (!_boundingbox.isIn(x, y))
		return false;

	// Initialization
	std::vector<glm::vec3>::const_iterator it1, it2;
	int count = 0;

	// Set first iterator to the last vertex
	it1 = _geometry.end();
	it1--;

	// Cycle through the vertices, counting the number of ray intersections
	for (it2 = _geometry.begin(); it2 != _geometry.end(); ++it2) {
		// Check for a ray intersection
		if (isRayIntersect(x, y, it1->x, it1->y, it2->x, it2->y))
			count++;

		// Store for the next loop
		it1 = it2;
	}

	// Ray-casting algorithm
	return (count % 2) ? true : false;
}

void Trigger::calculateDistance() {

}

void Trigger::render(Graphics::RenderPass pass) {
	if (!_visible || pass != Graphics::kRenderPassTransparent)
		return;

	size_t vertexCount = _geometry.size();
	if (vertexCount < 3)
		return;

	// Semi-transparent blue hue
	glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
	glBegin(GL_TRIANGLES);

	for (size_t i = 2; i < vertexCount; ++i) {
		glVertex3fv(glm::value_ptr(_geometry[0]));
		glVertex3fv(glm::value_ptr(_geometry[i - 1]));
		glVertex3fv(glm::value_ptr(_geometry[i]));
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

/*
 * Prepare internal data for the contains(x, y) call.
 * Run this after the _geometry data is loaded.
 */
void Trigger::prepare() {
	std::vector<glm::vec3>::iterator it;

	// Add vertices to the bounding box
	for (it = _geometry.begin(); it != _geometry.end(); ++it)
		_boundingbox.add(it->x, it->y, it->z);

	// Flag as processed
	_prepared = true;
}

/*
 * Return true if a ray extending from (x, y) to (x, infinity)
 * intersects the line segment from (x1, y1) to (x2, y2).
 */
bool Trigger::isRayIntersect(float x,  float y,
                             float x1, float y1,
                             float x2, float y2) const {

	// Check if y is below max of y1 and y2
	if ((y > y1) && (y > y2))
		return false;

	// Check if x is between x1 and x2
	if (x < x1 ) {
		if (x < x2)
			return false;
	} else {
		if (x > x2)
			return false;
	}

	return true; // TODO
}

} // End of namespace Engines
