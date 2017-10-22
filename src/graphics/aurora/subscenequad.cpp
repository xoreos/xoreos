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
 *  The quad for displaying a subscene.
 */

#include "src/common/maths.h"

#include "src/graphics/aurora/subscenequad.h"

namespace Graphics {

namespace Aurora {

SubSceneQuad::SubSceneQuad() :
		GUIElement(GUIElement::kGUIElementFront),
		_x(0), _y(0), _width(0), _height(0) {
	_distance = -FLT_MAX;
}

void SubSceneQuad::calculateDistance() {
}

void SubSceneQuad::render(RenderPass pass) {
	// Save the viewport information for later restoration.
	glPushAttrib(GL_VIEWPORT_BIT);

	// Set the new viewport.
	glViewport(_x, _y, _width, _height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(_x, _y, _width, _height);

	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_SCISSOR_TEST);

	// Restore the original viewport.
	glPopAttrib();
}

void SubSceneQuad::setPosition(int x, int y) {
	_x = x;
	_y = y;
}

void SubSceneQuad::setSize(int width, int height) {
	_width = width;
	_height = height;
}

} // End of namespace Aurora

} // End of namespace Graphics
