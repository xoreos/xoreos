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

#ifndef ENGINES_LOCALPATHFINDING_H
#define ENGINES_LOCALPATHFINDING_H

#include "src/common/ptrlist.h"

#include "src/engines/aurora/astar.h"
#include "src/engines/aurora/pathfinding.h"

namespace Common {
class BoundingBox;
}

namespace Engines {

class LocalPathfinding;
class ObjectWalkmesh;

class LocalAStar : public AStar {
public:
	LocalAStar(LocalPathfinding *localPathfinding);
	~LocalAStar();

	float getGValue(Node &previousNode, uint32_t face, float &x, float &y) const;
};

class LocalPathfinding : public Pathfinding {
public:
	LocalPathfinding(Pathfinding *globalPathfinding);
	~LocalPathfinding();

	/** State if the point, in the XY plane, is walkable. */
	bool walkable(glm::vec3 point);

	bool findIntersection(const glm::vec3 &start, const glm::vec3 &end, glm::vec3 &intersect) const;
	bool buildWalkmeshAround(std::vector<glm::vec3> &path, float halfWidth = 0.f);
	bool findPathTo(std::vector<glm::vec3> &path);
	void addStaticObjects(ObjectWalkmesh *objectWalkmesh);

protected:
	uint32_t findFace(float x, float y, bool onlyWalkable);
	/** Get the 6 walkable adjacent faces. */
	void getAdjacentFaces(uint32_t face, uint32_t parent, std::vector<uint32_t> &adjFaces,
	                      bool onlyWalkable = true) const;
	void getFacePosition(uint32_t face, float &x, float &y) const;
	void rasterizeTriangle(std::vector<glm::vec3> &vertices, float halfWidth = 0.f);
	void addObjects(const std::vector<float> &vertices, const std::vector<uint32_t> &faces,
	                float halfWidth = 0.f);
	/** Get the vertices along a path of faces. */
	virtual void getVerticesTunnel(std::vector<uint32_t> &facePath, std::vector<glm::vec3> &tunnel,
	                               std::vector<bool> &tunnelLeftRight);

private:
	bool getSharedVertices(uint32_t face1, uint32_t face2, std::vector<glm::vec3> &verts,
	                       std::vector<bool> &leftRight) const;
	glm::vec3 toVirtualPlan(const glm::vec3 &vector) const;
	glm::vec2 toVirtualPlan(const glm::vec2 &vector) const;
	glm::vec3 fromVirtualPlan(const glm::vec3 &vector) const;
	glm::vec2 fromVirtualPlan(const glm::vec2 &vector) const;

	uint32_t closestWalkableFace(uint32_t face, uint32_t &second) const;
	uint32_t getDiagonalFace(uint32_t face, uint32_t direction) const;
	uint32_t orthogonalJump(uint32_t startFace, uint32_t jumpDir) const;
	uint32_t diagonalJump(uint32_t startFace, uint32_t jumpDir) const;
	Pathfinding *_globalPathfinding;

	Common::PtrList<ObjectWalkmesh> _staticObjects;

	glm::vec3 _trueMin;
	glm::vec3 _trueMax;
	uint32_t _gridWidth;
	uint32_t _gridHeight;
	uint32_t _endFace;
	float  _cellSize;
	float  _xCenter;
	float  _yCenter;
	float  _xEnd;
	float  _yEnd;
	float  _xMin;
	float  _yMin;
	float _angle;


friend class LocalAStar;
};

} // End of namespace Engines

#endif // ENGINES_LOCALPATHFINDING_H
