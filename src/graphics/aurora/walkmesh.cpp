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

#include "src/common/util.h"

#include "src/engines/aurora/pathfinding.h"

#include "src/graphics/aurora/walkmesh.h"

namespace Graphics {

namespace Aurora {

Walkmesh::Walkmesh(Engines::Pathfinding *pathfinding)
    : Graphics::Renderable(Graphics::kRenderableTypeObject),
    _pathfinding(pathfinding) {
	_heightAdjust = 0.05;
	_unwalkableFaceColor[0] = 0.1;
	_unwalkableFaceColor[1] = 0.5;
	_unwalkableFaceColor[2] = 0.5;
	_unwalkableFaceColor[3] = 0.4;

	_walkableFaceColor[0] = 0.5;
	_walkableFaceColor[1] = 0.f;
	_walkableFaceColor[2] = 0.5;
	_walkableFaceColor[3] = 0.4;
}

Walkmesh::~Walkmesh() {
}

void Walkmesh::setFaces(std::vector<uint32_t> &faces) {
	_highlightedFaces = faces;
}

void Walkmesh::show() {
	Renderable::show();
}

void Walkmesh::hide() {
	Renderable::hide();
}

void Walkmesh::setAdjustedHeight(float adjustedHeight) {
	_heightAdjust = adjustedHeight;
}

void Walkmesh::setWalkableColor(float color[4]) {
	_walkableFaceColor[0] = color[0];
	_walkableFaceColor[1] = color[1];
	_walkableFaceColor[2] = color[2];
	_walkableFaceColor[3] = color[3];
}

void Walkmesh::setUnwalkableColor(float color[4]) {
	_unwalkableFaceColor[0] = color[0];
	_unwalkableFaceColor[1] = color[1];
	_unwalkableFaceColor[2] = color[2];
	_unwalkableFaceColor[3] = color[3];
}

void Walkmesh::calculateDistance() {
}

void Walkmesh::render(Graphics::RenderPass pass) {
	if (pass == Graphics::kRenderPassOpaque)
		return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (uint32_t f = 0; f < _pathfinding->_facesCount; ++f) {
		glBegin(GL_POLYGON);
		if (_pathfinding->faceWalkable(f)) {
			glColor4f(1.f, 1.f, 1.f, 0.5);
		} else {
			glColor4f(1.f, 1.f, 1.f, 0.3);
		}

		std::vector<glm::vec3> vertices;
		_pathfinding->getVertices(f, vertices, false);
		for (std::vector<glm::vec3>::iterator v = vertices.begin(); v != vertices.end(); ++v) {
			glVertex3f((*v)[0], (*v)[1], (*v)[2] + _heightAdjust);
		}
		glEnd();
	}

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	for (uint32_t f = 0; f < _pathfinding->_facesCount; ++f) {
		glBegin(GL_POLYGON);
		if (_pathfinding->faceWalkable(f)) {
			glColor4f(_walkableFaceColor[0], _walkableFaceColor[1], _walkableFaceColor[2], _walkableFaceColor[3]);
		} else {
			glColor4f(_unwalkableFaceColor[0], _unwalkableFaceColor[1], _unwalkableFaceColor[2], _unwalkableFaceColor[3]);
		}

		std::vector<glm::vec3> vertices;
		_pathfinding->getVertices(f, vertices, false);
		for (std::vector<glm::vec3>::iterator v = vertices.begin(); v != vertices.end(); ++v) {
			glVertex3f((*v)[0], (*v)[1], (*v)[2] + _heightAdjust);
		}
		glEnd();
	}

	for (size_t hF = 0; hF < _highlightedFaces.size(); ++hF) {
		uint32_t f = _highlightedFaces[hF];
		glBegin(GL_POLYGON);
		if (_pathfinding->faceWalkable(f)) {
			glColor4f(MIN<float>(_walkableFaceColor[0] + 0.2, 1.f),
			          MIN<float>(_walkableFaceColor[1] + 0.2, 1.f),
			          MIN<float>(_walkableFaceColor[2] + 0.2, 1.f), 0.5);
		} else {
			glColor4f(_unwalkableFaceColor[0], _unwalkableFaceColor[1], _unwalkableFaceColor[2], 0.5);
		}

		std::vector<glm::vec3> vertices;
		_pathfinding->getVertices(f, vertices, false);
		for (std::vector<glm::vec3>::iterator v = vertices.begin(); v != vertices.end(); ++v)
			glVertex3f((*v)[0], (*v)[1], (*v)[2] + _heightAdjust);
		glEnd();
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

} // End of namespace Aurora

} // End of namespace Graphics
