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
 *  Base class for local pathfinding.
 */

#include "external/glm/gtx/intersect.hpp"
#include "external/glm/gtx/transform.hpp"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/boundingbox.h"
#include "src/common/aabbnode.h"
#include "src/common/geometry.h"

#include "src/graphics/aurora/walkmesh.h"
#include "src/graphics/aurora/line.h"

#include "src/engines/aurora/astar.h"
#include "src/engines/aurora/objectwalkmesh.h"
#include "src/engines/aurora/localpathfinding.h"


namespace Engines {

LocalAStar::LocalAStar(LocalPathfinding *localPathfinding) : AStar(localPathfinding) {
}

LocalAStar::~LocalAStar() {
}

float LocalAStar::getGValue(Node &previousNode, uint32_t face, float &x, float &y) const {
	const LocalPathfinding *localPathfinding = dynamic_cast<const LocalPathfinding *>(_pathfinding);
	if (!localPathfinding)
		throw Common::Exception("LocalAStar::getGValue(): No local pathfinding object");

	localPathfinding->getFacePosition(face, x, y);
	return getEuclideanDistance(previousNode.x, previousNode.y, x, y);
}

LocalPathfinding::LocalPathfinding(Pathfinding *globalPathfinding) : Pathfinding(std::vector<bool>(), 4),
    _globalPathfinding(globalPathfinding) {
	// Set property 0 to unwalkable.
	_walkableProperties.push_back(false);
	// Set property 1 to walkable.
	_walkableProperties.push_back(true);

	_gridHeight = 45;
	_gridWidth = 45;
	_cellSize = 0.1;

	_xCenter = 0.f;
	_yCenter = 0.f;

	_xMin = 0.f;
	_yMin = 0.f;

	_xEnd = 0.f;
	_yEnd = 0.f;

	_angle = 0.f;

	_endFace = UINT32_MAX;

	float unwalkableColor[4] = {0.6, 0.4, 0.6, 0.4};
	float walkableColor[4]   = {0.3, 0.7, 0.5, 0.4};
	_walkmeshDrawing->setUnwalkableColor(unwalkableColor);
	_walkmeshDrawing->setWalkableColor(walkableColor);

	float pathColor[4] = {1.f, 1.f, 1.f, 0.8};
	_pathDrawing->setColor(pathColor);

	LocalAStar *localAStar = new LocalAStar(this);
	setAStarAlgorithm(localAStar);
}

LocalPathfinding::~LocalPathfinding() {
}

bool LocalPathfinding::walkable(glm::vec3 point) {
	return Pathfinding::walkable(toVirtualPlan(point));
}

bool LocalPathfinding::findIntersection(const glm::vec3 &start, const glm::vec3 &end,
                                        glm::vec3 &intersect) const {
	glm::vec3 objIntersect;
	float distance = FLT_MAX;
	for (auto o = _staticObjects.begin(); o != _staticObjects.end(); ++o) {
		if (!(*o)->findIntersection(start, end, objIntersect))
			continue;

		if (glm::distance(start, objIntersect) >= distance)
			continue;

		distance = glm::distance(start, objIntersect);
		intersect = objIntersect;
	}

	if (distance < FLT_MAX)
		return true;

	return _globalPathfinding->findIntersection(start.x, start.y, start.z, end.x, end.y, end.z,
	                                            intersect);
}

bool LocalPathfinding::buildWalkmeshAround(std::vector<glm::vec3> &path, float halfWidth) {
	if (path.size() < 2)
		return false;

	// Prevent rendering.
	bool walkmeshVisible = _walkmeshVisible;
	if (walkmeshVisible) showWalkmesh(false);

	_cellSize = halfWidth;
	// First, find the intersection between the grid and the path.
	_xCenter = path[0][0];
	_yCenter = path[0][1];
	const uint32_t xCenterCell = _gridWidth / 2;
	const uint32_t yCenterCell = _gridHeight / 2;

	glm::vec2 lastPoint = path.back();
	glm::vec2 anglePoint = path[1];

	// Compute the angle with the vertical .
	glm::vec2 relatAnglePoint = anglePoint - glm::vec2(path[0]);
	_angle = acos(relatAnglePoint[1] / glm::length(relatAnglePoint));
	if (relatAnglePoint[0] > 0.f) _angle = M_PI - _angle;

	// Work in the virtual plan centered in (_xCenter, _yCenter) and rotated by an angle _angle
	// in the XY plan.
	_xMin = - (xCenterCell * _cellSize) - _cellSize / 2;
	_yMin = - (yCenterCell * _cellSize) - _cellSize / 2;

	bool hasToShift = true;
	const float shift = 1.f;
	const glm::vec2 bottomLeft(_xMin - shift, _yMin - shift);
	const glm::vec2 topRight(_xMin + _cellSize * _gridWidth + shift,
	                         _yMin + _cellSize * _gridHeight + shift);
	bool bIntersec = false, lIntersec = false, tIntersec = false, rIntersec = false;
	// Check if we are inside the big box.
	if (Common::intersectBoxPoint2D(bottomLeft, topRight, toVirtualPlan(lastPoint))) {
		glm::vec3 end = toVirtualPlan(path.back());
		_xEnd = end[0];
		_yEnd = end[1];
		// Check if we are between the big and the normal box otherwise we need to shift.
		if (glm::length(lastPoint - glm::vec2(_xCenter, _yCenter)) < -_xMin) {
			hasToShift = false;
		} else {
			hasToShift = true;
			if (relatAnglePoint[0] > 0.f) {
				bIntersec = true;
			} else {
				tIntersec = true;
			}
		}
	} else {
		// We need to compute the intersection between the path and the big box.
		glm::vec2 intersection, iB;
		const glm::vec2 bottomRight(topRight[0], bottomLeft[1]);
		const glm::vec2 topLeft(bottomLeft[0], topRight[1]);
		for (size_t p = 1; p < path.size(); ++p) {
			const glm::vec2 start(toVirtualPlan(path[p - 1]));
			const glm::vec2 end(toVirtualPlan(path[p]));
			bIntersec = Common::intersectSegments2D(bottomLeft, bottomRight, start, end, intersection, iB);
			lIntersec = Common::intersectSegments2D(bottomLeft, topLeft, start, end, intersection, iB);
			tIntersec = Common::intersectSegments2D(topRight, topLeft, start, end, intersection, iB);
			rIntersec = Common::intersectSegments2D(topRight, bottomRight, start, end, intersection, iB);
			if (!bIntersec && !lIntersec && !tIntersec && !rIntersec)
				continue;

			_xEnd = intersection[0];
			_yEnd = intersection[1];
			break;
		}
	}

	if (hasToShift) {
		glm::vec3 diff(0.f, 0.f, 0.f);
		if (lIntersec || rIntersec) {
			diff[0] = lIntersec ? shift : -shift;
			_xMin += diff[0];
		}

		if (tIntersec || bIntersec) {
			diff[1] = tIntersec ? shift : -shift;
			_yMin += diff[1];
		}

		glm::vec3 newCenter = fromVirtualPlan(diff);
		_xCenter = newCenter[0];
		_yCenter = newCenter[1];

		// If the end point is in the box, recompute the end in the virtual plan.
		glm::vec2 end = toVirtualPlan(lastPoint);
		if (Common::intersectBoxPoint2D(glm::vec2(_xMin, _yMin),
		                                glm::vec2(_xMin +_cellSize * _gridWidth, _yMin + _cellSize * _gridHeight),
		                                end)) {
			_xEnd = end[0];
			_yEnd = end[1];
		}
	}

	// Build the local mesh.
	_facesCount = _gridHeight * _gridWidth;
	_faceProperty.clear();
	_faceProperty.resize(_facesCount, 1);

	// Adjust drawing height.
	const float walkmeshHeight = _globalPathfinding->getHeight(_xCenter, _yCenter, true) + 0.05;
	_walkmeshDrawing->setAdjustedHeight(0.15);

	// Rasterize unwalkable faces from the walkmesh into the grid.
	std::vector<Common::AABBNode *> nodes;
	glm::vec2 minInReal = fromVirtualPlan(glm::vec2(_xMin, _yMin));
	glm::vec2 maxInReal = fromVirtualPlan(glm::vec2(_xMin + _cellSize * _gridWidth,
	                                                _yMin + _cellSize * _gridHeight));
	glm::vec2 lTInReal = fromVirtualPlan(glm::vec2(_xMin, _yMin + _cellSize * _gridHeight));
	glm::vec2 rBInReal = fromVirtualPlan(glm::vec2(_xMin + _cellSize * _gridWidth, _yMin));
	_trueMin = glm::vec3(MIN(MIN(MIN(minInReal[0], maxInReal[0]), lTInReal[0]), rBInReal[0]),
	        MIN(MIN(MIN(minInReal[1], maxInReal[1]), lTInReal[1]), rBInReal[1]),
	        walkmeshHeight - 0.8f);
	_trueMax = glm::vec3(MAX(MAX(MAX(minInReal[0], maxInReal[0]), lTInReal[0]), rBInReal[0]),
	        MAX(MAX(MAX(minInReal[1], maxInReal[1]), lTInReal[1]), rBInReal[1]),
	        walkmeshHeight + 0.8f);
	for (uint8_t c = 0; c < 2; ++c) {
		_trueMin[c] -= halfWidth;
		_trueMax[c] += halfWidth;
	}

	for (size_t n = 0; n < _globalPathfinding->_aabbTrees.size(); ++n) {
		if (_globalPathfinding->_aabbTrees[n])
			_globalPathfinding->_aabbTrees[n]->getNodesInAABox(_trueMin, _trueMax, nodes);
	}

	std::vector<glm::vec3> vertices;
	for (auto n = nodes.begin(); n != nodes.end(); ++n) {
		const uint32_t face = static_cast<uint32_t>((*n)->getProperty());
		if (_globalPathfinding->faceWalkable(face))
			continue;

		_globalPathfinding->getVertices(face, vertices, false);
		if (!Common::intersectBoxTriangle3D(_trueMin, _trueMax,
		                                    vertices[0], vertices[1], vertices[2]))
			continue;

		// Translate to virtual plan.
		for (auto &v : vertices) {
			v = toVirtualPlan(v);
		}
		rasterizeTriangle(vertices, halfWidth);
	}

	_verticesCount = (_gridWidth + 1) * (_gridHeight + 1);
	_vertices.resize(_verticesCount * 3);
	for (uint32_t yVert = 0; yVert < _gridHeight + 1; ++yVert) {
		for (uint32_t xVert = 0; xVert < _gridWidth + 1; ++xVert) {
			const uint32_t vertexID = xVert + yVert * (_gridWidth + 1);
			const glm::vec3 vertex = fromVirtualPlan(glm::vec3(_xMin + _cellSize * xVert,
			                                                   _yMin + _cellSize * yVert,
			                                                   0.f));
			// x position of the vertex.
			_vertices[3 * vertexID] = vertex[0];
			// y position of the vertex.
			_vertices[3 * vertexID + 1] = vertex[1];
			// z position of the vertex.
			_vertices[3 * vertexID + 2] = walkmeshHeight;
		}
	}

	// Set faces.
	_faces.resize(_facesCount * 4);
	for (uint32_t yCell = 0; yCell < _gridHeight; ++yCell) {
		for (uint32_t xCell = 0; xCell < _gridWidth; ++xCell) {
			const uint32_t faceID = xCell + yCell * _gridWidth;
			_faces[4 * faceID] = xCell + (yCell * (_gridWidth + 1));
			_faces[4 * faceID + 1] = xCell + 1 + (yCell * (_gridWidth + 1));
			_faces[4 * faceID + 2] = xCell + 1 + ((1 + yCell) * (_gridWidth + 1));
			_faces[4 * faceID + 3] = xCell + ((1 + yCell) * (_gridWidth + 1));
		}
	}

	_adjFaces.resize(_facesCount * 4);
	// Set adjacencies.
	for (uint32_t yCell = 0; yCell < _gridHeight; ++yCell) {
		for (uint32_t xCell = 0; xCell < _gridWidth; ++xCell) {
			const uint32_t faceID = xCell + yCell * _gridWidth;
			// Bottom face.
			_adjFaces[faceID * 4] = yCell != 0 ? faceID - _gridWidth : UINT32_MAX;
			// Right face.
			_adjFaces[faceID * 4 + 1] = xCell != _gridWidth - 1 ? faceID + 1 : UINT32_MAX;
			// Top face.
			_adjFaces[faceID * 4 + 2] = yCell != _gridHeight - 1 ? faceID + _gridWidth : UINT32_MAX;
			// Left face.
			_adjFaces[faceID * 4 + 3] = xCell != 0 ? faceID - 1 : UINT32_MAX;
		}
	}

	// Add relevant static objects.
	for (auto o = _staticObjects.begin(); o != _staticObjects.end(); ++o) {
		if (!(*o)->in(_trueMin, _trueMax))
			continue;

		addObjects((*o)->getVertices(), (*o)->getFaces(), halfWidth);
	}

	if (walkmeshVisible) showWalkmesh(true);

	return true;
}

uint32_t LocalPathfinding::findFace(float x, float y, bool onlyWalkable) {
	float xRel = fabs(x - _xMin);
	float yRel = fabs(y - _yMin);

	uint32_t xIndex = MIN(static_cast<uint32_t>(floor(xRel / _cellSize)), _gridWidth - 1);
	uint32_t yIndex = MIN(static_cast<uint32_t>(floor(yRel / _cellSize)), _gridHeight - 1);

	uint32_t face = xIndex + (yIndex * _gridWidth);
	if (onlyWalkable && !faceWalkable(face))
		return UINT32_MAX;

	return face;
}

uint32_t LocalPathfinding::closestWalkableFace(uint32_t face, uint32_t &second) const {
	// Look around the face if there is something interesting.
	std::vector<uint32_t> adjFaces;
	getAdjacentFaces(face, UINT32_MAX, adjFaces);
	if (!adjFaces.empty())
		return adjFaces.front();

	// Check if the face is on the border (a path bypassing an object/wall).
	std::list<uint32_t> candidates;
	// On the bottom.
	if (face < _gridWidth) {
		for (auto f = face + 1; f < _gridWidth; ++f) {
			if (!faceWalkable(f))
				continue;

			candidates.push_back(f);
			break;
		}
		for (auto f = face - 1; f != UINT32_MAX; --f) {
			if (!faceWalkable(f))
				continue;

			candidates.push_back(f);
			break;
		}
	}

	// On the top.
	const uint32_t topLeftFace = (_gridWidth - 1) * _gridHeight;
	if (face >= topLeftFace) {
		for (auto f = face + 1; f < topLeftFace + _gridWidth; ++f) {
			if (!faceWalkable(f))
				continue;

			candidates.push_back(f);
			break;
		}
		for (auto f = face - 1; f < topLeftFace; --f) {
			if (!faceWalkable(f))
				continue;

			candidates.push_back(f);
			break;
		}
	}

	// One the left.
	if (face % _gridWidth == 0) {
		for (auto f = face + _gridWidth; f <= topLeftFace; f += _gridWidth) {
			if (!faceWalkable(f))
				continue;

			candidates.push_back(f);
			break;
		}
		if (face != 0) {
			for (auto f = face - _gridWidth; f != UINT32_MAX; f -= _gridWidth) {
				if (!faceWalkable(f))
					continue;

				candidates.push_back(f);
				break;
			}
		}
	}

	// One the right.
	if (face % _gridWidth == _gridWidth - 1) {
		for (auto f = face +_gridWidth; f < _gridWidth * _gridHeight; f += _gridWidth) {
			if (!faceWalkable(f))
				continue;

			candidates.push_back(f);
			break;
		}
		if (face > _gridWidth) {
			for (auto f = face -_gridWidth; f >= _gridWidth - 1; f -= _gridWidth) {
				if (!faceWalkable(f))
					continue;

				candidates.push_back(f);
				break;
			}
		}
	}

	uint32_t closest = UINT32_MAX;
	second = UINT32_MAX;
	float x, y, cX, cY;
	getFacePosition(face, x, y);
	float distance = FLT_MAX;
	float distanceSecond = FLT_MAX;
	for (auto c = candidates.begin(); c != candidates.end(); ++c) {
		getFacePosition(*c, cX, cY);
		const float length = glm::length(glm::vec2(x - cX, y- cY));
		if (distance < length) {
			if (distanceSecond >= length)
				second = *c;
			continue;
		}

		closest = *c;
	}

	return closest;
}

void LocalPathfinding::getAdjacentFaces(uint32_t face, uint32_t parent, std::vector<uint32_t> &adjFaces,
                                        bool onlyWalkable) const {
	// Use Jump Point Search algorithm to ignore symmetric paths unless there is no end face.
	adjFaces.clear();

	if (parent == UINT32_MAX || _endFace == UINT32_MAX) {
		// Make forward and backward directions as priorities.
		std::list<uint32_t> directions = {2, 0, 1, 3};
		for (auto d = directions.begin(); d != directions.end(); ++d) {
			const uint32_t orthoFace = _adjFaces[face * 4 + *d];
			const uint32_t diagFace = getDiagonalFace(face, *d);
			if (onlyWalkable) {
				if (faceWalkable(orthoFace))
					adjFaces.push_back(orthoFace);
				if (faceWalkable(diagFace))
					adjFaces.push_back(diagFace);
			} else {
				adjFaces.push_back(orthoFace);
				adjFaces.push_back(diagFace);
			}
		}
		return;
	}

	// Get direction.
	uint32_t dir;
	bool orthoMove = true;
	bool diagMove = true;
	const uint32_t row1 = parent / _gridWidth;
	const uint32_t row2 = face / _gridWidth;
	const uint32_t column1 = parent % _gridWidth;
	const uint32_t column2 = face % _gridWidth;
	const int32_t vertiDiff = row2 - row1;
	const int32_t horiDiff = column2 - column1;

	if (vertiDiff == 0 || horiDiff == 0) {
		diagMove = false;
		// Orthogonal move.
		if (horiDiff > 0) {
			dir = 1;
		} else if (horiDiff < 0) {
			dir = 3;
		} else if (vertiDiff > 0) {
			dir = 2;
		} else {
			dir = 0;
		}
	} else {
		orthoMove = false;
		if (horiDiff == vertiDiff) {
			if (vertiDiff > 0) {
				dir = 1;
			} else {
				dir = 3;
			}
		} else {
			if (vertiDiff > 0) {
				dir = 2;
			} else {
				dir = 0;
			}
		}
	}

	// Follow the initial adjacency order (bottom, right, top, left).
	if (orthoMove) {
		if (face == _endFace) {
			adjFaces.push_back(_endFace);
			return;
		}

		// Add axis face.
		if (onlyWalkable) {
			if (!faceWalkable(face))
				return;

			// Before jumping, look at forced neighbors.
			const uint32_t neighborA = _adjFaces[face * 4 + (dir + 1) % 4];
			if (!faceWalkable(neighborA) && neighborA != UINT32_MAX) {
				const uint32_t forcedNeighbor = _adjFaces[neighborA * 4 + dir];
				if (faceWalkable(forcedNeighbor)) {
					adjFaces.push_back(forcedNeighbor);
				}
			}
			const uint32_t neighborB = _adjFaces[face * 4 + (dir + 3) % 4];
			if (!faceWalkable(neighborB) && neighborB != UINT32_MAX) {
				const uint32_t forcedNeighbor = _adjFaces[neighborB * 4 + dir];
				if (faceWalkable(forcedNeighbor)) {
					adjFaces.push_back(forcedNeighbor);
				}
			}
			// Jump until an unwalkable face or the end of the grid or a forced neighbor.
			const uint32_t jumpFace = orthogonalJump(face, dir);
			if (jumpFace != UINT32_MAX) {
				adjFaces.push_back(jumpFace);
			}
		} else {
			adjFaces.push_back(face);
		}
	}

	// Add diagonal face.
	if (diagMove) {
		if (onlyWalkable) {
			if (!faceWalkable(face))
				return;

			// Before jumping, look at forced neighbors.
			const uint32_t neighborA = _adjFaces[face * 4 + (dir + 2) % 4];
			if (!faceWalkable(neighborA) && neighborA != UINT32_MAX) {
				const uint32_t forcedNeighbor = _adjFaces[neighborA * 4 + (dir + 1) % 4];
				if (faceWalkable(forcedNeighbor)) {
					adjFaces.push_back(forcedNeighbor);
				}
			}
			const uint32_t neighborB = _adjFaces[face * 4 + (dir + 3) % 4];
			if (!faceWalkable(neighborB) && neighborB != UINT32_MAX) {
				const uint32_t forcedNeighbor = _adjFaces[neighborA * 4 + dir];
				if (faceWalkable(forcedNeighbor)) {
					adjFaces.push_back(forcedNeighbor);
				}
			}

			// There are three kind of jumps, horizontally, vertically and diagonaly.
			// First try horizontal and vertical jumps.
			const uint32_t jumpFaceA = orthogonalJump(face, dir);
			if (jumpFaceA != UINT32_MAX)
				adjFaces.push_back(jumpFaceA);
			const uint32_t jumpFaceB = orthogonalJump(face, (dir + 1) % 4);
			if (jumpFaceB != UINT32_MAX)
				adjFaces.push_back(jumpFaceB);

			const uint32_t jumpFace = diagonalJump(face, dir);
			if (jumpFace != UINT32_MAX) {
				adjFaces.push_back(jumpFace);
			}
		} else {
			adjFaces.push_back(face);
		}
	}
}

uint32_t LocalPathfinding::diagonalJump(uint32_t startFace, uint32_t jumpDir) const {
	const uint32_t diagFace = getDiagonalFace(startFace, jumpDir);
	if (!faceWalkable(diagFace))
		return UINT32_MAX;

	if (diagFace == _endFace)
		return diagFace;

	// Before jumping, look at forced neighbors.
	const uint32_t neighborA = _adjFaces[diagFace * 4 + (jumpDir + 2) % 4];
	const uint32_t neighborB = _adjFaces[diagFace * 4 + (jumpDir + 3) % 4];
	if ((!faceWalkable(neighborA) && neighborA != UINT32_MAX)
	    || (!faceWalkable(neighborB) && neighborB != UINT32_MAX))
		return diagFace;

	// There are three kind of jumps, horizontally, vertically and diagonaly.
	// First try horizontal and vertical jumps.
	const uint32_t jumpFaceA = orthogonalJump(diagFace, jumpDir);
	const uint32_t jumpFaceB = orthogonalJump(diagFace, (jumpDir + 1) % 4);
	if (jumpFaceA != UINT32_MAX || jumpFaceB != UINT32_MAX)
		return diagFace;

	return diagonalJump(diagFace, jumpDir);
}

uint32_t LocalPathfinding::orthogonalJump(uint32_t startFace, uint32_t jumpDir) const {
	const uint32_t nextFace = _adjFaces[startFace * 4 + jumpDir];
	if (nextFace == _endFace)
		return nextFace;

	if (!faceWalkable(nextFace))
		return UINT32_MAX;

	// Check about forced neighbor(s) (orthogonal unwalkable faces).
	const uint32_t orthognalFaceA = _adjFaces[nextFace * 4 + (jumpDir + 1) % 4];
	const uint32_t orthognalFaceB = _adjFaces[nextFace * 4 + (jumpDir + 3) % 4];
	if (!faceWalkable(orthognalFaceA) && orthognalFaceA != UINT32_MAX) {
		const uint32_t forcedNeigbor = _adjFaces[orthognalFaceA * 4 + jumpDir];
		if (faceWalkable(forcedNeigbor))
			return nextFace;
	}

	if (!faceWalkable(orthognalFaceB) && orthognalFaceB != UINT32_MAX) {
		const uint32_t forcedNeigbor = _adjFaces[orthognalFaceB * 4 + jumpDir];
		if (faceWalkable(forcedNeigbor))
			return nextFace;
	}

	return orthogonalJump(nextFace, jumpDir);
}

uint32_t LocalPathfinding::getDiagonalFace(uint32_t face, uint32_t direction) const {
	const uint32_t sideA = _adjFaces[face * 4 + direction];
	const uint32_t sideB = _adjFaces[face * 4 + (direction + 1) % 4];
	// Check if we are on a border of the grid.
	if (sideA == UINT32_MAX || sideB == UINT32_MAX)
		return UINT32_MAX;

	return (direction % 2 == 0 ? sideA : sideB) + (direction > 1 ? -1 : 1);
}

void LocalPathfinding::getVerticesTunnel(std::vector<uint32_t> &facePath, std::vector<glm::vec3> &tunnel,
                                         std::vector<bool> &tunnelLeftRight) {
	std::vector<glm::vec3> sharedVertices;
	std::vector<bool> leftRight;

	for (size_t f = 0; f < facePath.size() - 1; ++f) {
		getSharedVertices(facePath[f], facePath[f + 1], sharedVertices, leftRight);
		tunnel.insert(tunnel.end(), sharedVertices.begin(), sharedVertices.end());
		tunnelLeftRight.insert(tunnelLeftRight.end(), leftRight.begin(), leftRight.end());
	}
}

bool LocalPathfinding::getSharedVertices(uint32_t face1, uint32_t face2,
                                         std::vector<glm::vec3> &verts, std::vector<bool> &leftRight) const {
	verts.clear();
	leftRight.clear();
	uint32_t vert1, vert2;

	const uint32_t row1 = face1 / _gridWidth;
	const uint32_t row2 = face2 / _gridWidth;
	const uint32_t column1 = face1 % _gridWidth;
	const uint32_t column2 = face2 % _gridWidth;
	const int32_t vertiDiff = row2 < row1 ? -1 : (row2 > row1 ? 1 : 0);
	const int32_t horiDiff = column2 < column1 ? -1 : (column2 > column1 ? 1 : 0);

	if (vertiDiff > 0) {
		 // Left.
		vert1 = face2 + row2 - horiDiff;
		 // Right.
		vert2 = face2 + row2 - horiDiff + 1;
	} else {
		 // Left.
		vert1 = face2 + _gridWidth + row2 + 2;
		 // Right.
		vert2 = face2 + _gridWidth + row2 + 1;
	}

	// Check if it is a diagonal move.
	if (vertiDiff != 0 && horiDiff != 0) {
		glm::vec3 vertex;
		bool isLeft;
		if (vertiDiff + horiDiff == 0) {
			getVertex(vert1, vertex);
			isLeft = true;
		} else {
			getVertex(vert2, vertex);
			isLeft = false;
		}

		verts.push_back(vertex);
		leftRight.push_back(isLeft);
		verts.push_back((vertex));
		leftRight.push_back(!isLeft);
		return true;
	}

	// Horizontal move.
	if (horiDiff > 0) {
		vert1 = face2 + _gridWidth + row2; // Left;
		vert2 = face2 + row2; // Right.
	} else if (horiDiff < 0){
		vert1 = face2 + row2 + 1; // Left.
		vert2 = face2 + _gridWidth + row2 + 1; // Right;
	}

	glm::vec3 vertex1, vertex2;
	getVertex(vert1, vertex1);
	getVertex(vert2, vertex2);
	verts.push_back(vertex1);
	leftRight.push_back(true);
	verts.push_back(vertex2);
	leftRight.push_back(false);

	return true;
}

void LocalPathfinding::getFacePosition(uint32_t face, float &x, float &y) const {
	const uint32_t xIndex = face % _gridWidth;
	const uint32_t yIndex = (face - xIndex) / _gridWidth;

	x = _xMin + xIndex * _cellSize + _cellSize * 0.5;
	y = _yMin + yIndex * _cellSize + _cellSize * 0.5;
}

bool LocalPathfinding::findPathTo(std::vector<glm::vec3> &path) {
	std::vector<uint32_t> facePath;

	const glm::vec3 virtStart = toVirtualPlan(path[0]);

	_endFace = findFace(_xEnd, _yEnd, false);
	// Check if path is walkable.
	uint32_t secondEndFace = UINT32_MAX;
	if (!faceWalkable(_endFace)) {
		_endFace = closestWalkableFace(_endFace, secondEndFace);
		getFacePosition(_endFace, _xEnd, _yEnd);
	}
	bool pathFound = findPath(virtStart[0], virtStart[1], _xEnd, _yEnd, facePath, 0.f, 1000);
	if (!pathFound && secondEndFace != UINT32_MAX) {
		// Try with the other end face.
		facePath.clear();
		getFacePosition(secondEndFace, _xEnd, _yEnd);
		pathFound = findPath(virtStart[0], virtStart[1], _xEnd, _yEnd, facePath, 0.f, 1000);
	}
	if (facePath.empty())
		return pathFound;

	const glm::vec3 start = path[0];
	const glm::vec3 end = fromVirtualPlan(glm::vec3(_xEnd, _yEnd, 0.f));
	path.clear();
	smoothPath(start[0], start[1], end[0], end[1], facePath, path);
	for (auto &v : path)
		v = toVirtualPlan(v);

	return pathFound;
}

void LocalPathfinding::rasterizeTriangle(std::vector<glm::vec3> &vertices, float halfWidth) {
	// Set positions relative.
	float y0 = vertices[0][1];
	float y1 = vertices[1][1];
	float y2 = vertices[2][1];

	float x0 = vertices[0][0];
	float x1 = vertices[1][0];
	float x2 = vertices[2][0];

	glm::vec3 shift0(0.f, 0.f, 0.f);
	glm::vec3 shift1(0.f, 0.f, 0.f);
	glm::vec3 shift2(0.f, 0.f, 0.f);
	if (halfWidth > 0.f) {
		shift0 = getOrthonormalVec(glm::vec3(x1 - x0, y1 - y0, 0.f), false) * halfWidth;
		shift1 = getOrthonormalVec(glm::vec3(x2 - x1, y2 - y1, 0.f), false) * halfWidth;
		shift2 = getOrthonormalVec(glm::vec3(x0 - x2, y0 - y2, 0.f), false) * halfWidth;
	}

	y0 -= _yMin;
	y1 -= _yMin;
	y2 -= _yMin;

	x0 -= _xMin;
	x1 -= _xMin;
	x2 -= _xMin;

	// Bounding rectangle
	const float minX = MIN(x1, MIN(x2, x0)) - halfWidth;
	const float maxX = MAX(MAX(x1, MAX(x2, x0)) + halfWidth, 0.f);
	const float minY = MIN(y1, MIN(y2, y0)) - halfWidth;
	const float maxY = MAX(MAX(y1, MAX(y2, y0)) + halfWidth, 0.f);

	const uint32_t startY = static_cast<uint32_t>(MAX(floorf(minY / _cellSize), 0.f));
	const uint32_t startX = static_cast<uint32_t>(MAX(floorf(minX / _cellSize), 0.f));
	const uint32_t endY = static_cast<uint32_t>(MIN(ceilf(maxY / _cellSize), static_cast<float>(_gridHeight)));
	const uint32_t endX = static_cast<uint32_t>(MIN(ceilf(maxX / _cellSize), static_cast<float>(_gridWidth)));

	const float dy10 = y1 - y0;
	const float dy21 = y2 - y1;
	const float dy02 = y0 - y2;

	const float dx10 = x1 - x0;
	const float dx21 = x2 - x1;
	const float dx02 = x0 - x2;

	float yStep = _cellSize * startY + _cellSize / 2;
	float xStep = _cellSize * startX + _cellSize / 2;
	const float firstXstep = xStep;
	// Scan through bounding rectangle.
	for (uint32_t y = startY; y < endY; ++y) {
		const float yStep0 = yStep - y0 + shift0[1];
		const float yStep1 = yStep - y1 + shift1[1];
		const float yStep2 = yStep - y2 + shift2[1];

		xStep = firstXstep;
		for (uint32_t x = startX; x < endX; ++x) {
			const float test01 = (xStep - x0 + shift0[0]) * dy10 - yStep0 * dx10;
			const float test12 = (xStep - x1 + shift1[0]) * dy21 - yStep1 * dx21;
			const float test20 = (xStep - x2 + shift2[0]) * dy02 - yStep2 * dx02;
			if (test01 < 0 && test12 < 0 && test20 < 0) {
				_faceProperty[x + y * _gridWidth] = 0;
			}

			xStep += _cellSize;
		}
		yStep += _cellSize;
	}
}

void LocalPathfinding::addObjects(const std::vector<float> &vertices, const std::vector<uint32_t> &faces,
                                  float halfWidth) {
	// Local Objects.
	const uint32_t facesCount = faces.size() / 3;
	if (facesCount != 0) {
		for (uint32_t f = 0; f < facesCount; ++f) {
			std::vector<glm::vec3> vert;
			for (uint8_t v = 0; v < 3; ++v) {
				const float xO = vertices[faces[f * 3 + v] * 3];
				const float yO = vertices[faces[f * 3 + v] * 3 + 1];
				vert.push_back(glm::vec3(xO, yO, 0.f));
			}

			for (auto &v : vert)
				v = toVirtualPlan(v);

			const glm::vec2 vertA(vert[0][0], vert[0][1]);
			const glm::vec2 vertB(vert[1][0], vert[1][1]);
			const glm::vec2 vertC(vert[2][0], vert[2][1]);
			rasterizeTriangle(vert, halfWidth);
		}
	}
}

void LocalPathfinding::addStaticObjects(ObjectWalkmesh *objectWalkmesh) {
	_staticObjects.push_back(objectWalkmesh);
}

glm::vec3 LocalPathfinding::toVirtualPlan(const glm::vec3 &vector) const {
	glm::vec3 virt;
	glm::vec3 transla = vector - glm::vec3(_xCenter, _yCenter, 0.f);
	virt[0] = cos(-_angle) * transla[0] - sin(-_angle) * transla[1];
	virt[1] = sin(-_angle) * transla[0] + cos(-_angle) * transla[1];
	return virt;
}

glm::vec2 LocalPathfinding::toVirtualPlan(const glm::vec2 &vector) const {
	glm::vec2 transla =  vector - glm::vec2(_xCenter, _yCenter);
	glm::vec2 virt;
	virt[0] = cos(-_angle) * transla[0] - sin(-_angle) * transla[1];
	virt[1] = sin(-_angle) * transla[0] + cos(-_angle) * transla[1];
	return virt;
}

glm::vec3 LocalPathfinding::fromVirtualPlan(const glm::vec3 &vector) const {
	glm::vec3 virt;
	virt[0] = cos(_angle) * vector[0] - sin(_angle) * vector[1];
	virt[1] = sin(_angle) * vector[0] + cos(_angle) * vector[1];
	return virt + glm::vec3(_xCenter, _yCenter, 0.f);
}

glm::vec2 LocalPathfinding::fromVirtualPlan(const glm::vec2 &vector) const {
	glm::vec2 virt;
	virt[0] = cos(_angle) * vector[0] - sin(_angle) * vector[1];
	virt[1] = sin(_angle) * vector[0] + cos(_angle) * vector[1];
	return virt + glm::vec2(_xCenter, _yCenter);
}

} // End of namespace Engines
