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

#include "src/graphics/aurora/line.h"

namespace Graphics {

namespace Aurora {

Line::Line() : Renderable(kRenderableTypeObject) {
	_color[0] = 1.f;
	_color[1] = 0.f;
	_color[2] = 1.f;
	_color[3] = 0.8;
}

Line::~Line() {
}

void Line::setVertices(std::vector<glm::vec3> &points) {
	if (points.size() < 2)
		return;

	_points = points;
}

void Line::setColor(float color[]) {
	_color[0] = color[0];
	_color[1] = color[1];
	_color[2] = color[2];
	_color[3] = color[3];
}

void Line::show() {
	Renderable::show();
}

void Line::hide() {
	Renderable::hide();
}

void Line::calculateDistance() {
}

void Line::render(RenderPass pass) {
	if (pass == kRenderPassOpaque)
		return;

	if (_points.empty())
		return;

	glLineWidth(3.f);
	glBegin(GL_LINES);
	for (size_t p = 0; p < _points.size() - 1; ++p) {
		glColor4f(_color[0], _color[1], _color[2], _color[3]);
		glVertex3f(_points[p][0], _points[p][1], _points[p][2] + 0.1);
		glVertex3f(_points[p + 1][0], _points[p + 1][1], _points[p + 1][2] + 0.1);
	}
	glEnd();
	glLineWidth(1.f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	return;
}

} // End of namespace Aurora

} // End of namespace Graphics

