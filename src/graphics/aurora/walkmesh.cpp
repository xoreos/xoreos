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
 *  Generic renderable walkmesh.
 */

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/intersect.hpp"

#include "src/common/maths.h"
#include "src/common/util.h"

#include "src/graphics/aurora/walkmesh.h"

namespace Graphics {

namespace Aurora {

Walkmesh::Walkmesh()
		: Renderable(Graphics::kRenderableTypeObject),
		  _highlightFaceIndex(-1),
		  _invisible(true) {
}

float Walkmesh::getElevationAt(float x, float y, uint32 &faceIndex) const {
	if (_indicesWalkable.empty())
		return FLT_MIN;

	size_t faceCount = _indicesWalkable.size() / 3;
	const float *vertices = _vertices.data();
	uint32 index = 0;

	glm::vec3 v0, v1, v2, intersection;
	glm::vec3 orig = glm::vec3(x, y, 1000.0f);

	for (size_t i = 0; i < faceCount; ++i) {
		v0 = glm::make_vec3(vertices + 3 * _indicesWalkable[index + 0]);
		v1 = glm::make_vec3(vertices + 3 * _indicesWalkable[index + 1]);
		v2 = glm::make_vec3(vertices + 3 * _indicesWalkable[index + 2]);
		index += 3;

		if (glm::intersectRayTriangle(orig, glm::vec3(0.0f, 0.0f, -1.0f), v0, v1, v2, intersection)) {
			faceIndex = i;
			return (v0 * (1.0f - intersection.x - intersection.y) +
			        v1 * intersection.x +
			        v2 * intersection.y).z;
		}
	}

	return FLT_MIN;
}

bool Walkmesh::testCollision(const glm::vec3 &orig, const glm::vec3 &dest) const {
	if (_indicesNonWalkable.empty())
		return false;

	size_t faceCount = _indicesNonWalkable.size() / 3;
	const float *vertices = _vertices.data();
	uint32 index = 0;

	glm::vec3 adjDest = glm::vec3(dest.x, dest.y, 1000.0f);
	glm::vec3 v0, v1, v2, intersection;
	glm::vec3 dir = glm::normalize(dest - orig);

	for (size_t i = 0; i < faceCount; ++i) {
		v0 = glm::make_vec3(vertices + 3 * _indicesNonWalkable[index + 0]);
		v1 = glm::make_vec3(vertices + 3 * _indicesNonWalkable[index + 1]);
		v2 = glm::make_vec3(vertices + 3 * _indicesNonWalkable[index + 2]);
		index += 3;

		// Intersection with horizontal objects
		if (glm::intersectRayTriangle(adjDest, glm::vec3(0.0f, 0.0f, -1.0f), v0, v1, v2, intersection))
			return true;

		// Intersection with vertical objects
		if (glm::intersectRayTriangle(orig, dir, v0, v1, v2, intersection)) {
			glm::vec3 absIntersection(v0 * (1.0f - intersection.x - intersection.y) +
			                          v1 * intersection.x +
			                          v2 * intersection.y);
			if (glm::distance(orig, absIntersection) <= glm::distance(orig, dest))
				return true;
		}
	}

	return false;
}

void Walkmesh::highlightFace(uint32 index) {
	_highlightFaceIndex = index;
}

void Walkmesh::setInvisible(bool invisible) {
	_invisible = invisible;
}

void Walkmesh::calculateDistance() {
}

void Walkmesh::render(Graphics::RenderPass pass) {
	if (_invisible || _vertices.empty() || pass != Graphics::kRenderPassTransparent)
		return;

	glVertexPointer(3, GL_FLOAT, 0, _vertices.data());
	glEnableClientState(GL_VERTEX_ARRAY);

	glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	glDrawElements(GL_TRIANGLES, _indicesNonWalkable.size(), GL_UNSIGNED_INT, _indicesNonWalkable.data());

	glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
	glDrawElements(GL_TRIANGLES, _indicesWalkable.size(), GL_UNSIGNED_INT, _indicesWalkable.data());

	glDisableClientState(GL_VERTEX_ARRAY);

	// Render highlighted face
	if (_highlightFaceIndex > -1) {
		glColor4f(0.0f, 1.0f, 1.0f, 0.5f);
		glBegin(GL_TRIANGLES);
			uint32 index = 3 * _highlightFaceIndex;
			glVertex3fv(&_vertices[3 * _indicesWalkable[index + 0]]);
			glVertex3fv(&_vertices[3 * _indicesWalkable[index + 1]]);
			glVertex3fv(&_vertices[3 * _indicesWalkable[index + 2]]);
		glEnd();
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void Walkmesh::refreshIndexGroups() {
	_indicesWalkable.clear();
	_indicesNonWalkable.clear();

	size_t faceCount = _indices.size() / 3;
	size_t index = 0;

	for (size_t i = 0; i < faceCount; ++i) {
		float i0 = _indices[index + 0];
		float i1 = _indices[index + 1];
		float i2 = _indices[index + 2];

		if (_faceWalkableMap[i]) {
			_indicesWalkable.push_back(i0);
			_indicesWalkable.push_back(i1);
			_indicesWalkable.push_back(i2);
		} else {
			_indicesNonWalkable.push_back(i0);
			_indicesNonWalkable.push_back(i1);
			_indicesNonWalkable.push_back(i2);
		}

		index += 3;
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
