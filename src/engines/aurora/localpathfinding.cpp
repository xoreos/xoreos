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

#include "glm/gtx/intersect.hpp"
#include "glm/gtx/transform.hpp"

#include "src/common/util.h"
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

float LocalAStar::getGValue(Node &previousNode, uint32 face, float &x, float &y) const {
	static_cast<LocalPathfinding *>(_pathfinding)->getFacePosition(face, x, y);
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

bool LocalPathfinding::buildWalkmeshAround(std::vector<glm::vec3> &path, float halfWidth) {
	// Prevent rendering.
	bool walkmeshVisible = _walkmeshVisible;
	if (walkmeshVisible) showWalkmesh(false);

	_cellSize = halfWidth;
	// First, find the intersection between the grid and the path.
	_xCenter = path[0][0];
	_yCenter = path[0][1];
	const uint32 xCenterCell = _gridWidth / 2;
	const uint32 yCenterCell = _gridHeight / 2;

	glm::vec2 lastPoint(path.back()[0], path.back()[1]);
	glm::vec2 anglePoint;
	if (glm::distance(glm::vec2(_xCenter, _yCenter), lastPoint) < _cellSize * MAX(xCenterCell, yCenterCell)) {
		// Last point is close, take it to compute the angle.
		anglePoint = lastPoint;
	} else {
		// Find intersection between the path and a circle around the center.
		for (size_t p = 1; p < path.size(); ++p) {
			glm::vec3 start(path[p - 1][0], path[p - 1][1], path[p - 1][2]);
			glm::vec3 end(path[p][0], path[p][1], path[p][2]);
			glm::vec3 intersectA, normalA, intersectB, normalB;
			if (glm::intersectLineSphere(start, end, start, 4.f,
			                             intersectA, normalA,
			                             intersectB, normalB)) {
				anglePoint = glm::vec2(intersectA);
			} else {
				warning("Path has no intersection inside local circle");
				anglePoint = lastPoint;
			}
		}
	}

	// Compute the angle with the vertical .
	glm::vec2 relatAnglePoint = anglePoint - glm::vec2(path[0]);
	_angle = acos(relatAnglePoint[1] / glm::length(relatAnglePoint));
	if (relatAnglePoint[0] > 0.f) _angle = M_PI - _angle;

	// Work in the virtual plan centered in (_xCenter, _yCenter) and rotated by an angle _angle
	// in the XY plan.
	_xMin = - (xCenterCell * _cellSize) - _cellSize / 2;
	_yMin = - (yCenterCell * _cellSize) - _cellSize / 2;

	bool hasToShift = true;
	const float shift = 1.5;
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
	const float height = _globalPathfinding->getHeight(_xCenter, _yCenter, true) + 0.05;
	_walkmeshDrawing->setAdjustedHeight(0.15);

	// Rasterize unwalkable faces from the walkmesh into the grid.
	std::vector<Common::AABBNode *> nodes;
	glm::vec2 min(_xMin, _yMin);
	glm::vec2 max(_xMin + _cellSize * _gridWidth, _yMin + _cellSize * _gridHeight);
	glm::vec2 leftTop(_xMin, _yMin + _cellSize * _gridHeight);
	glm::vec2 rightBottom(_xMin + _cellSize * _gridWidth, _yMin);
	glm::vec2 minInReal = fromVirtualPlan(min);
	glm::vec2 maxInReal = fromVirtualPlan(max);
	glm::vec2 lTInReal = fromVirtualPlan(leftTop);
	glm::vec2 rBInReal = fromVirtualPlan(rightBottom);
	_trueMin = glm::vec2(MIN(MIN(MIN(minInReal[0], maxInReal[0]), lTInReal[0]), rBInReal[0]),
	        MIN(MIN(MIN(minInReal[1], maxInReal[1]), lTInReal[1]), rBInReal[1]));
	_trueMax = glm::vec2(MAX(MAX(MAX(minInReal[0], maxInReal[0]), lTInReal[0]), rBInReal[0]),
	        MAX(MAX(MAX(minInReal[1], maxInReal[1]), lTInReal[1]), rBInReal[1]));
	for (uint8 c = 0; c < 2; ++c) {
		_trueMin[c] -= halfWidth;
		_trueMax[c] += halfWidth;
	}

	for (size_t n = 0; n < _globalPathfinding->_AABBTrees.size(); ++n) {
		if (_globalPathfinding->_AABBTrees[n])
			_globalPathfinding->_AABBTrees[n]->getNodesInAABox2D(_trueMin, _trueMax, nodes);
	}

	std::vector<glm::vec3> vertices;
	for (std::vector<Common::AABBNode *>::iterator n = nodes.begin(); n != nodes.end(); ++n) {
		const uint32 face = static_cast<uint32>((*n)->getProperty());
		if (_globalPathfinding->faceWalkable(face))
			continue;

		_globalPathfinding->getVertices(face, vertices);
		// Translate to virtual plan.
		for (auto &v : vertices) {
			v = toVirtualPlan(v);
		}
		rasterizeTriangle(vertices, halfWidth);
	}

	_verticesCount = (_gridWidth + 1) * (_gridHeight + 1);
	_vertices.resize(_verticesCount * 3);
	for (uint32 yVert = 0; yVert < _gridHeight + 1; ++yVert) {
		for (uint32 xVert = 0; xVert < _gridWidth + 1; ++xVert) {
			const uint32 vertexID = xVert + yVert * (_gridWidth + 1);
			glm::vec3 virtVert(_xMin + _cellSize * xVert, _yMin + _cellSize * yVert, height);
			glm::vec3 vertex = fromVirtualPlan(virtVert);
			// x position of the vertex.
			_vertices[3 * vertexID] = vertex[0];
			// y position of the vertex.
			_vertices[3 * vertexID + 1] = vertex[1];
			// z position of the vertex.
			_vertices[3 * vertexID + 2] = vertex[2];
		}
	}

	// Set faces.
	_faces.resize(_facesCount * 4);
	for (uint32 yCell = 0; yCell < _gridHeight; ++yCell) {
		for (uint32 xCell = 0; xCell < _gridWidth; ++xCell) {
			const uint32 faceID = xCell + yCell * _gridWidth;
			_faces[4 * faceID] = xCell + (yCell * (_gridWidth + 1));
			_faces[4 * faceID + 1] = xCell + 1 + (yCell * (_gridWidth + 1));
			_faces[4 * faceID + 2] = xCell + 1 + ((1 + yCell) * (_gridWidth + 1));
			_faces[4 * faceID + 3] = xCell + ((1 + yCell) * (_gridWidth + 1));
		}
	}

	_adjFaces.resize(_facesCount * 4);
	// Set adjacencies.
	for (uint32 yCell = 0; yCell < _gridHeight; ++yCell) {
		for (uint32 xCell = 0; xCell < _gridWidth; ++xCell) {
			const uint32 faceID = xCell + yCell * _gridWidth;
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

uint32 LocalPathfinding::findFace(float x, float y, bool onlyWalkable) {
	float xRel = fabs(x - _xMin);
	float yRel = fabs(y - _yMin);

	uint32 xIndex = MIN(static_cast<uint32>(floor(xRel / _cellSize)), _gridWidth - 1);
	uint32 yIndex = MIN(static_cast<uint32>(floor(yRel / _cellSize)), _gridHeight - 1);

	uint32 face = xIndex + (yIndex * _gridWidth);
	if (onlyWalkable && !faceWalkable(face))
		return UINT32_MAX;

	return face;
}

void LocalPathfinding::getAdjacentFaces(uint32 face, std::vector<uint32> &adjFaces, bool onlyWalkable) const {
	adjFaces.clear();

	// Follow the initial adjacency order (bottom, right, top, left).
	int32 shift = 1;
	for (uint8 f = 0; f < 4; ++f) {
		const uint32 sideA = _adjFaces[face * 4 + f];
		if (sideA == UINT32_MAX)
			continue;

		// Add axis face.
		if (onlyWalkable) {
			if (faceWalkable(sideA))
				adjFaces.push_back(sideA);
		} else {
			adjFaces.push_back(sideA);
		}

		// Add diagonal face.
		const uint32 sideB = _adjFaces[face * 4 + (f + 1) % 4];
		if (sideB == UINT32_MAX)
			continue;
		const uint32 &sideHeight = f % 2 == 0 ? sideA : sideB;

		if (f == 2)
			shift = -1;
		const uint32 diagFace = sideHeight + shift;

		if (onlyWalkable && !faceWalkable(diagFace))
			continue;

		adjFaces.push_back(diagFace);
	}
}

void LocalPathfinding::getVerticesTunnel(std::vector<uint32> &facePath, std::vector<glm::vec3> &tunnel,
                                         std::vector<bool> &tunnelLeftRight) {
	std::vector<glm::vec3> sharedVertices;
	std::vector<bool> leftRight;

	for (size_t f = 0; f < facePath.size() - 1; ++f) {
		getSharedVertices(facePath[f], facePath[f + 1], sharedVertices, leftRight);
		tunnel.insert(tunnel.end(), sharedVertices.begin(), sharedVertices.end());
		tunnelLeftRight.insert(tunnelLeftRight.end(), leftRight.begin(), leftRight.end());
	}
}

bool LocalPathfinding::getSharedVertices(uint32 face1, uint32 face2,
                                         std::vector<glm::vec3> &verts, std::vector<bool> &leftRight) const {
	verts.clear();
	leftRight.clear();
	uint32 vert1, vert2;

	const uint32 row1 = face1 / _gridWidth;
	const uint32 row2 = face2 / _gridWidth;
	const uint32 column1 = face1 % _gridWidth;
	const uint32 column2 = face2 % _gridWidth;
	const int32 vertiDiff = row2 - row1;
	const int32 horiDiff = column2 - column1;

	if (fabs(vertiDiff) > 1 || fabs(horiDiff) > 1) {
		error("Faces are not side by side, diff: %i, %i", vertiDiff, horiDiff);
		return false;
	}

	if (vertiDiff > 0) {
		 // Left.
		vert1 = face2 + row2 - horiDiff;
		 // Right.
		vert2 = face2 + row2 - horiDiff + 1;
	} else {
		 // Left.
		vert1 = face1 + row1 + 1;
		 // Right.
		vert2 = face1 + row1;
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
		vert1 = face1 + row2; // Left.
		vert2 = face1 + _gridWidth + row2; // Right;
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

void LocalPathfinding::getFacePosition(uint32 face, float &x, float &y) const {
	const uint32 xIndex = face % _gridWidth;
	const uint32 yIndex = (face - xIndex) / _gridWidth;

	x = _xMin + xIndex * _cellSize + _cellSize * 0.5;
	y = _yMin + yIndex * _cellSize + _cellSize * 0.5;
}

bool LocalPathfinding::findPathTo(std::vector<glm::vec3> &path) {
	std::vector<uint32> facePath;

	glm::vec3 virtStart = toVirtualPlan(path[0]);

	bool pathFound = findPath(virtStart[0], virtStart[1], _xEnd, _yEnd, facePath, 0.f, 1000);
	if (facePath.empty())
		return pathFound;

	glm::vec3 start = path[0];
	glm::vec3 end(_xEnd, _yEnd, 0.f);
	end = fromVirtualPlan(end);
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

	const uint32 startY = static_cast<uint32>(MAX(floor(minY / _cellSize), 0.f));
	const uint32 startX = static_cast<uint32>(MAX(floor(minX / _cellSize), 0.f));
	const uint32 endY = static_cast<uint32>(MIN(ceil(maxY / _cellSize), static_cast<float>(_gridHeight)));
	const uint32 endX = static_cast<uint32>(MIN(ceil(maxX / _cellSize), static_cast<float>(_gridWidth)));

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
	for (uint32 y = startY; y < endY; ++y) {
		const float yStep0 = yStep - y0 + shift0[1];
		const float yStep1 = yStep - y1 + shift1[1];
		const float yStep2 = yStep - y2 + shift2[1];

		xStep = firstXstep;
		for (uint32 x = startX; x < endX; ++x) {
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

void LocalPathfinding::addObjects(const std::vector<float> &vertices, const std::vector<uint32> &faces,
                                  float halfWidth) {
	// Local Objects.
	const uint32 facesCount = faces.size() / 3;
	if (facesCount != 0) {
		for (uint32 f = 0; f < facesCount; ++f) {
			std::vector<glm::vec3> vert;
			for (uint8 v = 0; v < 3; ++v) {
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

glm::vec3 LocalPathfinding::toVirtualPlan(glm::vec3 &vector) const {
	glm::vec3 virt;
	glm::vec3 transla = vector - glm::vec3(_xCenter, _yCenter, 0.f);
	virt[0] = cos(-_angle) * transla[0] - sin(-_angle) * transla[1];
	virt[1] = sin(-_angle) * transla[0] + cos(-_angle) * transla[1];
	return virt;
}

glm::vec2 LocalPathfinding::toVirtualPlan(glm::vec2 &vector) const {
	glm::vec2 transla =  vector - glm::vec2(_xCenter, _yCenter);
	glm::vec2 virt;
	virt[0] = cos(-_angle) * transla[0] - sin(-_angle) * transla[1];
	virt[1] = sin(-_angle) * transla[0] + cos(-_angle) * transla[1];
	return virt;
}

glm::vec3 LocalPathfinding::fromVirtualPlan(glm::vec3 &vector) const {
	glm::vec3 virt;
	virt[0] = cos(_angle) * vector[0] - sin(_angle) * vector[1];
	virt[1] = sin(_angle) * vector[0] + cos(_angle) * vector[1];
	return virt + glm::vec3(_xCenter, _yCenter, 0.f);
}

glm::vec2 LocalPathfinding::fromVirtualPlan(glm::vec2 &vector) const {
	glm::vec2 virt;
	virt[0] = cos(_angle) * vector[0] - sin(_angle) * vector[1];
	virt[1] = sin(_angle) * vector[0] + cos(_angle) * vector[1];
	return virt + glm::vec2(_xCenter, _yCenter);
}

} // End of namespace Engines
