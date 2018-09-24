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
	
	glm::vec3 orig(x, y, 1000);
	glm::vec3 intersection;

	for (size_t i = 2; i < vertexCount; ++i) {
		if (glm::intersectRayTriangle(orig,
					glm::vec3(0, 0, -1),
					_geometry[0],
					_geometry[i - 1],
					_geometry[i],
					intersection) == 1) {
			return true;
		}
	}

	return false;
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
 * Run this after the trigger geometry data is loaded.
 */
void Trigger::prepare() {
	// Flag as processed
	_prepared = true;
}

} // End of namespace Engines
