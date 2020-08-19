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
 *  Base class for pathfinding
 */

#include <algorithm>

#include "external/glm/gtx/intersect.hpp"

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/geometry.h"
#include "src/common/aabbnode.h"

#include "src/graphics/graphics.h"
#include "src/graphics/aurora/walkmesh.h"
#include "src/graphics/aurora/line.h"

#include "src/engines/aurora/astar.h"
#include "src/engines/aurora/pathfinding.h"

namespace Engines {

Pathfinding::Pathfinding(std::vector<bool> walkableProperties, uint32_t polygonEdges) :
                         _polygonEdges(polygonEdges), _verticesCount(0), _facesCount(0),
                         _epsilon(0.f), _pathVisible(false), _walkmeshVisible(false),
                         _walkableProperties(walkableProperties), _aStarAlgorithm(0) {

	_pathDrawing = new Graphics::Aurora::Line();
	_walkmeshDrawing = new Graphics::Aurora::Walkmesh(this);
}

Pathfinding::~Pathfinding() {
	delete _pathDrawing;
	delete _walkmeshDrawing;
	for (auto it = _aabbTrees.begin(); it != _aabbTrees.end(); ++it)
		delete *it;

	delete _aStarAlgorithm;
}

void Pathfinding::showPath(bool visible) {
	_pathVisible = visible;

	if (visible) {
		_pathDrawing->show();
	} else {
		_pathDrawing->hide();
	}
}

void Pathfinding::showWalkmesh(bool visible) {
	_walkmeshVisible = visible;

	if (visible) {
		_walkmeshDrawing->show();
	} else {
		_walkmeshDrawing->hide();
	}
}

bool Pathfinding::findPath(float startX, float startY, float endX, float endY,
	                       std::vector<uint32_t> &facePath, float width, uint32_t nbrIt) {
	if (!_aStarAlgorithm)
		error("An AStar algorithm must be set");

	if (!walkable(glm::vec3(startX, startY, 0.f))) {
		warning("Start point of the path need to be walkable.");
		return false;
	}

	bool result = _aStarAlgorithm->findPath(startX, startY, endX, endY, facePath, width, nbrIt);
	_walkmeshDrawing->setFaces(facePath);
	return result;
}

bool Pathfinding::isToTheLeft(glm::vec3 startSegment, glm::vec3 endSegment, glm::vec3 point) const {
	return glm::cross((endSegment - startSegment), point - startSegment)[2] > 0;
}

glm::vec3 Pathfinding::getOrthonormalVec(glm::vec3 segment, bool clockwise) const {
	glm::vec3 copy = segment;
	// Sinus of 90 degrees.
	float sinus = (clockwise ? -1 : 1);
	copy[0] = - segment[1] * sinus;
	copy[1] = segment[0] * sinus;
	return glm::normalize(copy);
}

void Pathfinding::smoothPath(float startX, float startY, float endX, float endY,
                             std::vector<uint32_t> &facePath, std::vector<glm::vec3> &path) {
	// Use Vector3 for simplicity and vectorial operations.
	glm::vec3 start(startX, startY, 0.f);
	glm::vec3 end(endX, endY, 0.f);

	// Vector that will store positions of each vertex of the path of faces.
	std::vector<glm::vec3> tunnel;
	// Indicate if the position is to the left or to the right.
	std::vector<bool> tunnelLeftRight;

	// Vector that will store the vertices that we will keep.
	std::vector<uint32_t> funnelIdx;

	tunnel.push_back(start);
	tunnelLeftRight.push_back(true);

	// Fill the tunnel vector and the related left/right vector.
	getVerticesTunnel(facePath, tunnel, tunnelLeftRight);
	// Add the ending point.
	tunnel.push_back(end);
	tunnel.push_back(end);
	// Consider the last point as a left position.
	tunnelLeftRight.push_back(true);
	tunnelLeftRight.push_back(false);

	uint32_t apex = 0;
	uint32_t feeler[2] = {0, 0};
	glm::vec3 feelerVector[2] = {glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)};

	for (size_t c = 1; c < tunnel.size(); ++c ) {
		// Get the vector between the apex and the next vertex on the path of faces.
		glm::vec3 v = tunnel[c] - tunnel[apex];

		// Are we outside of the feeler ? Or have we just move the apex to a new vertex?
		// The cross operation can state if the vector is on the left or right relatively.
		if (apex == feeler[tunnelLeftRight[c]]
		    || (glm::cross(v, feelerVector[tunnelLeftRight[c]])[2] < 0.f) != tunnelLeftRight[c]) {

			feeler[tunnelLeftRight[c]] = c;
			feelerVector[tunnelLeftRight[c]] = v;

			// Ensure that the opposite feeler is not the apex. Are we crossing the opposite feeler or
			// is it the ending point?
			if (apex != feeler[!tunnelLeftRight[c]] && (close(tunnel[c], tunnel[feeler[!tunnelLeftRight[c]]])
			    || (glm::cross(v, feelerVector[!tunnelLeftRight[c]])[2] < 0.f) != tunnelLeftRight[c])) {
				funnelIdx.push_back(apex);

				// Move the apex to the opposite feeler.
				apex = feeler[!tunnelLeftRight[c]];

				// Sometimes the current vertex can be closer than the one on the opposite feeler.
				// In that case, swap them.
				if (glm::length(v) < glm::length(feelerVector[!tunnelLeftRight[c]])) {
					glm::vec3 tmpV = tunnel[c];
					tunnel[c] = tunnel[feeler[!tunnelLeftRight[c]]];
					tunnel[feeler[!tunnelLeftRight[c]]] = tmpV;
					bool tmpB = tunnelLeftRight[c];
					tunnelLeftRight[c] = tunnelLeftRight[feeler[!tunnelLeftRight[c]]];
					tunnelLeftRight[feeler[!tunnelLeftRight[c]]] = tmpB;
				}

				// Adjust the current vertex and the feelers.
				c = apex;
				feeler[0] = apex;
				feeler[1] = apex;
			}
		}
	}

	path.push_back(start);
	for (size_t point = 0; point < funnelIdx.size(); ++point)
		path.push_back(tunnel[funnelIdx[point]]);

	// Assume end path is walkable.
	path.push_back(end);

	// Drawing part.
	std::vector<glm::vec3> pathToDraw;
	for (std::vector<glm::vec3>::iterator f = path.begin(); f != path.end(); ++f)
		pathToDraw.push_back(*f);

	for (std::vector<glm::vec3>::iterator it = pathToDraw.begin(); it != pathToDraw.end(); ++it) {
		(*it)[2] = getHeight((*it)[0], (*it)[1], true);
	}
	_pathDrawing->setVertices(pathToDraw);
}

void Pathfinding::getVerticesTunnel(std::vector<uint32_t> &facePath, std::vector<glm::vec3> &tunnel,
                                    std::vector<bool> &tunnelLeftRight) {
	if (facePath.size() < 2)
		return;

	std::vector<glm::vec3> cVerts, pVerts;
	int32_t returnPoint = -1;

	for (size_t face = 1; face < facePath.size(); ++face) {
		getVertices(facePath[face], cVerts, false);
		getVertices(facePath[face - 1], pVerts, false);

		if (face == 1) {
			// Find the first left and right by comparing to the next face.
			uint8_t startVert = _polygonEdges;
			for (uint8_t pV = 0; pV < _polygonEdges; ++pV) {
				bool equal = false;
				for (uint8_t cV = 0; cV < _polygonEdges; ++cV) {
					if (close(pVerts[pV], cVerts[cV])) {
						equal = true;
						break;
					}
				}
				if (equal)
					continue;

				startVert = pV;
				break;
			}

			if (startVert == _polygonEdges)
				error("No different vertices found");

			tunnel.push_back(pVerts[(startVert + 1) % _polygonEdges]);
			tunnel.push_back(pVerts[(startVert + 2) % _polygonEdges]);

			// Check if it is clockwise (to the left).
			bool orderedVert = isToTheLeft(pVerts[(startVert + 1) % _polygonEdges], pVerts[(startVert + 2) % _polygonEdges], pVerts[startVert]);
			// We are looking backward.
			tunnelLeftRight.push_back(!orderedVert);
			tunnelLeftRight.push_back(orderedVert);
		} else {
			// The following expects that faces share exactly two vertices i.e. one to
			// the left and one to the right. Otherwise tunnel and tunnelLeftRight
			// sizes will mismatch.
			bool otherVertSide = true;
			for (uint8_t cV = 0; cV < _polygonEdges; ++cV) {
				bool equal = false;
				for (uint8_t pV = 0; pV < _polygonEdges; ++pV) {
					if (close(pVerts[pV], cVerts[cV])) {
						equal = true;
						break;
					}
				}
				if (equal) {
					// Check if it is a new vertex or an already added vertex.
					bool alreadyThere = false;
					for (int32_t t = tunnel.size() - 1; t > returnPoint;--t) {
						if (close(cVerts[cV], tunnel[t])) {
							// The new vertex has to be the vertex from the previous face which we hadn't add last time.
							// So if we know on which side the vertex we previously added, we can deduce the side of the new added vertex.
							otherVertSide = tunnelLeftRight[t];
							alreadyThere = true;
							break;
						}
					}

					if (!alreadyThere) {
						tunnel.push_back(cVerts[cV]);
					}
				}
			}
			if (tunnelLeftRight.back() == otherVertSide)
				returnPoint = tunnel.size() - 3;
			tunnelLeftRight.push_back(!otherVertSide);
		}
	}
	if (tunnel.size() != tunnelLeftRight.size()) {
		warning("Possible error in tunnel parity (base tunnel size %u and left/right tunnel size %u",
		        (uint) tunnel.size(), (uint) tunnelLeftRight.size());
	}
}

void Pathfinding::getVertices(uint32_t faceID, std::vector<glm::vec3> &vertices, bool xyPlane) const {
	vertices.clear();
	vertices.resize(_polygonEdges);

	for (uint32_t v = 0; v < _polygonEdges; ++v) {
		getVertex(_faces[faceID * _polygonEdges + v], vertices[v], xyPlane);
	}
}


void Pathfinding::getVertex(uint32_t vertexID, glm::vec3 &vertex, bool xyPlane) const {
	// Don't take the z component into account.
	vertex = glm::vec3(_vertices[vertexID * 3],
	                         _vertices[vertexID * 3 + 1],
	                         xyPlane ? 0.f : _vertices[vertexID * 3 + 2]);
}

bool Pathfinding::walkableAASquare(glm::vec3 center, float halfWidth) {
	glm::vec2 min(center[0] - halfWidth, center[1] - halfWidth);
	glm::vec2 max(center[0] + halfWidth, center[1] + halfWidth);

	std::vector<Common::AABBNode *> nodesIn;
	for (std::vector<Common::AABBNode *>::iterator n = _aabbTrees.begin(); n != _aabbTrees.end(); ++n) {
		if (*n)
			(*n)->getNodesInAABox(min, max, nodesIn);
	}

	std::vector<glm::vec3> vertices;
	for (std::vector<Common::AABBNode *>::iterator n = nodesIn.begin(); n != nodesIn.end(); ++n) {
		uint32_t face = (*n)->getProperty();
		getVertices(face, vertices);

		if (_polygonEdges == 3) {
			if (!Common::intersectBoxTriangle2D(min, max, vertices[0], vertices[1], vertices[2]))
				continue;
		} else if (_polygonEdges == 4) {
			glm::vec2 vertMin(vertices[0][0], vertices[0][1]);
			glm::vec2 vertMax(vertices[1][0], vertices[1][1]);
			if (!Common::intersectBoxes3D(min, max, vertices[0], vertices[1]))
				continue;
		}

		if (!faceWalkable(face))
			return false;
	}
	return true;
}

bool Pathfinding::walkableSegment(glm::vec3 start, glm::vec3 end) {
	std::vector<Common::AABBNode *> nodesIn;

	for (std::vector<Common::AABBNode *>::iterator n = _aabbTrees.begin(); n != _aabbTrees.end(); ++n) {
		if (*n)
			(*n)->getNodesInSegment(start, end, nodesIn);
	}

	std::vector<glm::vec3> vertFace;
	for (std::vector<Common::AABBNode *>::iterator n = nodesIn.begin(); n != nodesIn.end(); ++n) {
		uint32_t face = (*n)->getProperty();
		getVertices(face, vertFace);

		if (_polygonEdges == 3) {
			if (!Common::intersectTriangleSegment2D(vertFace[0], vertFace[1], vertFace[2],
			                                        start, end))
				continue;
		} else if (_polygonEdges == 4) {
			if (!Common::intersectBoxSegment2D(vertFace[0], vertFace[2], start, end))
				continue;
		}

		if (!faceWalkable(face))
			return false;
	}
	return true;
}

bool Pathfinding::walkable(glm::vec3 point) {
	uint32_t face = findFace(point[0], point[1]);
	if (face == UINT32_MAX)
		return false;

	return faceWalkable(face);
}

void Pathfinding::findCenter(std::vector<glm::vec3> &vertices, float &centerX, float &centerY) const {
	centerX = 0.f;
	centerY = 0.f;
	for (std::vector<glm::vec3>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
		centerX += (*it)[0];
		centerY += (*it)[1];
	}

	centerX /= vertices.size();
	centerY /= vertices.size();
}

float Pathfinding::getHeight(float x, float y, bool onlyWalkable) const {
	glm::vec3 intersection;
	if (findIntersection(x, y, 100, x, y, -100, intersection, onlyWalkable))
		return intersection[2];

	return FLT_MIN;
}

uint32_t Pathfinding::findFace(float x, float y, bool onlyWalkable) {
	for (std::vector<Common::AABBNode *>::iterator it = _aabbTrees.begin(); it != _aabbTrees.end(); ++it) {
		if (*it == 0)
			continue;

		if (!(*it)->isIn(x, y))
			continue;

		std::vector<Common::AABBNode *> nodes;
		(*it)->getNodes(x, y, nodes);
		for (uint n = 0; n < nodes.size(); ++n) {
			uint32_t face = nodes[n]->getProperty();
			// Check walkability
			if (onlyWalkable && !faceWalkable(face))
				continue;

			if (!inFace(face, glm::vec3(x, y, 0.f)))
				continue;

			return face;
		}
	}

	return UINT32_MAX;
}

bool Pathfinding::findIntersection(float x1, float y1, float z1, float x2, float y2, float z2,
                                   glm::vec3 &intersect, bool onlyWalkable) const {
	for (size_t it = 0; it < _aabbTrees.size(); ++it) {
		if (_aabbTrees[it] == 0)
			continue;

		if (!_aabbTrees[it]->isIn(x1, y1, z1, x2, y2, z2))
			continue;

		std::vector<Common::AABBNode *> nodes;
		_aabbTrees[it]->getNodes(x1, y1, z1, x2, y2, z2, nodes);
		for (uint n = 0; n < nodes.size(); ++n) {
			uint32_t face = nodes[n]->getProperty();
			if (!inFace(face, glm::vec3(x1, y1, z1), glm::vec3(x2, y2, z2), intersect))
				continue;

			if (onlyWalkable && !faceWalkable(face))
				continue;

			return true;
		}
	}

	// Face not found
	return false;
}

bool Pathfinding::goThrough(uint32_t fromFace, uint32_t toFace, float width) {
	if (width <= 0.f)
		return true;

	glm::vec3 vec1, vec2;
	getSharedVertices(fromFace, toFace, vec1, vec2);
	// Check if the shared side is large enough
	if (glm::length(vec2 - vec1) > width)
		return true;

	// Test maximum three different cases:
	//  * A segment at the center of the shared side,
	//  * A segment that starts at the right vertex and go to the left,
	//  * A segment that starts at the left vertex and go to the right.
	// Obviously, it doesn't account all the possibilities, the test segment could be
	// a little bit to the left and also to the right at same time though it should
	// avoid all true negatives. Also it could be faster to test the first test at
	// the last position, it should be tested.

	glm::vec3 center, side1, side2;
	center = vec1 + ((vec2 - vec1) * 0.5f);
	side1 = center + (glm::normalize(vec1 - vec2) * (width / 2));
	side2 = center - (glm::normalize(vec1 - vec2) * (width / 2));
	bool test1 = walkableSegment(side1, side2);
	if (test1)
		return true;

	side1 = vec1;
	side2 = vec1 + (glm::normalize(vec2 - vec1) * width);
	bool test2 = walkableSegment(side1, side2);
	if (test2)
		return true;

	side1 = vec2;
	side2 = vec2 + (glm::normalize(vec1 - vec2) * width);
	return walkableSegment(side1, side2);
}

void Pathfinding::getAdjacentFaces(uint32_t face, uint32_t parent, std::vector<uint32_t> &adjFaces,
                                   bool onlyWalkable) const {
	adjFaces.clear();

	// Get adjacent faces
	for (uint8_t f = 0; f < _polygonEdges; ++f) {
		// Get adjacent face.
		uint32_t adjFace = _adjFaces[face * _polygonEdges + f];

		// Check if it is a border or the parent face.
		if (adjFace == UINT32_MAX || adjFace == parent)
			continue;

		// Check walkability.
		if (onlyWalkable && !faceWalkable(adjFace))
			continue;

		adjFaces.push_back(adjFace);
	}
}

void Pathfinding::getAdjacencyCenter(uint32_t faceA, uint32_t faceB, float &x, float &y) const {
	bool adjacent = false;
	// Get vertices from the closest edge to the face we are looking at.
	for (uint8_t f = 0; f < _polygonEdges; ++f) {
		if (_adjFaces[faceA * _polygonEdges + f] != faceB)
			continue;

		adjacent = true;
		uint32_t vert1 = _faces[faceA * _polygonEdges + f];
		uint32_t vert2 = _faces[faceA * _polygonEdges + (f + 1) % _polygonEdges];

		// Compute the center of the edge.
		x = (_vertices[vert1 * 3] + _vertices[vert2 * 3]) / 2;
		y = (_vertices[vert1 * 3 + 1] + _vertices[vert2 * 3 + 1]) / 2;

		break;
	}

	// Ensure the two faces are adjacent
	if(!adjacent)
		error("The two faces are not adjacent");
}

bool Pathfinding::close(glm::vec3 &pointA, glm::vec3 &pointB) const {
	if (_epsilon == 0.f)
		return pointA == pointB;

	return glm::distance(pointA, pointB) < _epsilon;
}

bool Pathfinding::inFace(uint32_t faceID, glm::vec3 point) const {
	// Ensure we are in the XY plane.
	point[2] = 0.f;

	std::vector<glm::vec3> vertices;
	getVertices(faceID, vertices);

	if (_polygonEdges == 3) {
		return Common::intersectTrianglePoint2D(point, vertices[0], vertices[1], vertices[2]);
	}

	if (_polygonEdges == 4) {
		// Assume AABB.
		return Common::intersectBoxPoint2D(vertices[0], vertices[2], point);
	}

	error("Number of edges not supported");
	return false;
}

bool Pathfinding::inFace(uint32_t faceID, glm::vec3 lineStart, glm::vec3 lineEnd, glm::vec3 &intersect) const {
	std::vector<glm::vec3> vertices;
	getVertices(faceID, vertices, false);

	glm::vec3 direction = glm::normalize(lineEnd - lineStart);
	glm::vec2 baryPosition;
	float distance;
	if (glm::intersectRayTriangle(lineStart, direction,
	                              vertices[0], vertices[1], vertices[2],
	                              baryPosition, distance)) {
		// Barycentric coordonate to cartesian.
		intersect = lineStart + direction * distance;
		return true;
	}

	return false;
}

bool Pathfinding::getSharedVertices(uint32_t face1, uint32_t face2, glm::vec3 &vert1, glm::vec3 &vert2) const {
	for (uint8_t i = 0; i < _polygonEdges; ++i) {
		if (_adjFaces[face1 * _polygonEdges + i] == face2) {
			std::vector<glm::vec3> vertices;
			getVertices(face1, vertices);
			vert1 = vertices[i];
			vert2 = vertices[(i + 1) % _polygonEdges];

			return true;
		}
	}

	// The faces are not adjacent.
	return false;
}

void Pathfinding::setAStarAlgorithm(AStar *aStarAlgorithm) {
	_aStarAlgorithm = aStarAlgorithm;
}

bool Pathfinding::faceWalkable(uint32_t faceID) const {
	if (faceID >= _faceProperty.size())
		return false;

	return surfaceWalkable(_faceProperty[faceID]);
}

bool Pathfinding::surfaceWalkable(uint32_t surfaceID) const {
	if (surfaceID >= _walkableProperties.size())
		return false;

	return _walkableProperties[surfaceID];
}

} // End of namespace Engines
