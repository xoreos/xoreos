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

#include <algorithm>

#include "external/glm/gtc/type_ptr.hpp"

#include "src/common/maths.h"

#include "src/graphics/aurora/subscenequad.h"

#include "src/events/events.h"

namespace Graphics {

namespace Aurora {

SubSceneQuad::SubSceneQuad() :
		GUIElement(GUIElement::kGUIElementFront), _lastSampled(0),
		_x(0), _y(0), _width(0), _height(0), _clearEnabled(true) {
	/*
	 * This distance value should ensure, that the subscene stays before panels and background
	 * but behind anything else.
	 */
	_distance = -100;
}

void SubSceneQuad::calculateDistance() {
}

void SubSceneQuad::render(RenderPass pass) {
	// Save the viewport information for later restoration.
	glPushAttrib(GL_VIEWPORT_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Set the new viewport.
	glViewport(_x, _y, _width, _height);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(glm::value_ptr(_projection));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(glm::value_ptr(_transformation));

	glEnable(GL_SCISSOR_TEST);
	glScissor(_x, _y, _width, _height);

	if (_clearEnabled)
		glClear(GL_COLOR_BUFFER_BIT);

	float elapsedTime = 0;
	const uint32_t now = EventMan.getTimestamp();

	if (_lastSampled != 0)
		elapsedTime = (now - _lastSampled) / 1000.0f;

	_lastSampled = now;

	for (size_t i = 0; i < _renderables.size(); ++i) {
		_renderables[i]->advanceTime(elapsedTime);
		_renderables[i]->render(pass);
	}

	glDisable(GL_SCISSOR_TEST);

	// Restore the original projection matrix.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

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

void SubSceneQuad::setDistance(float distance) {
	_distance = distance;
}

void SubSceneQuad::setProjectionMatrix(const glm::mat4 &projection) {
	_projection = projection;
}

void SubSceneQuad::setGlobalTransformationMatrix(const glm::mat4 &transformation) {
	_transformation = transformation;
}

void SubSceneQuad::setClearEnabled(bool clearEnabled) {
	_clearEnabled = clearEnabled;
}

void SubSceneQuad::add(Renderable *renderable) {
	_renderables.push_back(renderable);
}

void SubSceneQuad::remove(Renderable *renderable) {
	std::vector<Renderable *>::iterator iter = std::find(_renderables.begin(), _renderables.end(), renderable);
	if (iter != _renderables.end()) {
		_renderables.erase(iter);
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
