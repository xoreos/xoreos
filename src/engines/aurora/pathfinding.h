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
 *  Base class for pathfinding.
 */

#ifndef ENGINES_PATHFINDING_H
#define ENGINES_PATHFINDING_H

#include "external/glm/vec3.hpp"

#include "src/common/ustring.h"

#include "src/graphics/renderable.h"

namespace Graphics {

namespace Aurora {
class Walkmesh;
class Line;
}

}

namespace Common {
class AABBNode;
}

namespace Engines {

class LocalPathfinding;
class AStar;

class Pathfinding {
public:
	Pathfinding(std::vector<bool> walkableProperties, uint32_t polygonEdges = 3);
	virtual ~Pathfinding();

	/** Find a path of face between two points.
	 *
	 *  It will use an A* algorithm to find as fast and as best as possible a path.
	 *  The algorithm used can be tuned from the bare class Engines::AStar and must be
	 *  set in the constructor thanks to setAStarAlgorithm() method.
	 */
	bool findPath(float startX, float startY, float endX, float endY,
	              std::vector<uint32_t> &facePath, float width = 0.f, uint32_t nbrIt = 10000);
	/** Compute a smooth line from a path of face.
	 *
	 *  @param startX       The x component of the starting point.
	 *  @param startY       The y component of the starting point.
	 *  @param endX         The x component of the ending point.
	 *  @param endY         The y component of the ending point.
	 *  @param facePath     The path of face.
	 */
	void smoothPath(float startX, float startY, float endX, float endY,
	                std::vector<uint32_t> &facePath, std::vector<glm::vec3> &path);

	/** Get the intersection between a line and the walkmesh.
	 *
	 *  @param x1            The x component of the first point in the line.
	 *  @param y1            The y component of the first point in the line.
	 *  @param z1            The x component of the first point in the line.
	 *  @param x2            The x component of the second point in the line.
	 *  @param y2            The y component of the second point in the line.
	 *  @param z2            The x component of the second point in the line.
	 *  @param intersect     The returned intersection.
	 *  @param onlyWalkable  Specify if the intersection must be with a walkable face.
	 */
	bool findIntersection(float x1, float y1, float z1, float x2, float y2, float z2,
	                      glm::vec3 &intersect, bool onlyWalkable = false) const;
	/** Set the A* algorithm object. */
	void setAStarAlgorithm(AStar *aStarAlgorithm);

	/** Is the face from walkmesh walkable? */
	virtual bool faceWalkable(uint32_t faceID) const;
	/** Get the height at a specific point (in the XY plane) in the walkmesh. */
	float getHeight(float x, float y, bool onlyWalkable = false) const;

	/** Show the computed path. */
	void showPath(bool visible = true);
	/** Show the walkmesh. */
	void showWalkmesh(bool visible = true);

protected:
	/** State if the point, in the XY plane, is walkable. */
	bool walkable(glm::vec3 point);
	/** Find a face according to a point in the XY plane. */
	virtual uint32_t findFace(float x, float y, bool onlyWalkable = true);
	/** Is the surface walkable? */
	virtual bool surfaceWalkable(uint32_t surfaceID) const;
	/** Get the adjacent faces of a specific face. */
	virtual void getAdjacentFaces(uint32_t face, uint32_t parent, std::vector<uint32_t> &adjFaces,
	                              bool onlyWalkable = true) const;
	/** Get the vertices of a face. */
	virtual void getVertices(uint32_t faceID, std::vector<glm::vec3> &vertices, bool xyPlane = true) const;
	/** The vertex position from the vertex id. */
	virtual void getVertex(uint32_t vertexID, glm::vec3 &vertex, bool xyPlane = true) const;
	/** Get the vertices along a path of faces. */
	virtual void getVerticesTunnel(std::vector<uint32_t> &facePath, std::vector<glm::vec3> &tunnel,
	                       std::vector<bool> &tunnelLeftRight);
	/** Check if a given align-axis square is walkable. */
	virtual bool walkableAASquare(glm::vec3 center, float halfWidth);
	/** Check if a given segment is walkable. */
	virtual bool walkableSegment(glm::vec3 start, glm::vec3 end);
	/** Find the rough center of the polygon (simple mean). */
	virtual void findCenter(std::vector<glm::vec3> &vertices, float &centerX, float &centerY) const;
	/** Are two points close? Use the _epsilon value to evaluate the proximity.*/
	bool close(glm::vec3 &pointA, glm::vec3 &pointB) const;

	uint32_t _polygonEdges;  ///< The number of edge a walkmesh face has.
	uint32_t _verticesCount; ///< The total number of vertices in the walkmesh.
	uint32_t _facesCount;    ///< The total number of faces in the walkmesh.

	float  _epsilon; ///< The minimal distance to consider two vertices equal.

	std::vector<float>  _vertices;     ///< The vertices of the walkmesh. Each vertex has 3 components.
	std::vector<uint32_t> _faces;        ///< The faces of the walkmesh.
	std::vector<uint32_t> _adjFaces;     ///< The adjacent faces in the walkmesh.
	std::vector<uint32_t> _faceProperty; ///< The property of each faces. Usually used to state the walkability.

	std::vector<Common::AABBNode *> _aabbTrees; ///< The set of AABB trees in the walkmesh.
	bool _pathVisible;
	bool _walkmeshVisible;

private:
	/** Is a point in a specific face? */
	bool inFace(uint32_t faceID, glm::vec3 point) const;
	/** Is a line in a specific face? */
	bool inFace(uint32_t faceID, glm::vec3 lineStart, glm::vec3 lineEnd,
	            glm::vec3 &intersect) const;
	/** Get the vertices shared by two faces. */
	bool getSharedVertices(uint32_t face1, uint32_t face2,
	                       glm::vec3 &vert1, glm::vec3 &vert2) const;
	/** Can a creature go from one face to an other one? */
	bool goThrough(uint32_t fromFace, uint32_t toFace, float width);
	/** Get the orthonornmal vector of a segment. */
	glm::vec3 getOrthonormalVec(glm::vec3 segment, bool clockwise = true) const;
	/** Is a point to the left from a given segment? */
	bool isToTheLeft(glm::vec3 startSegment, glm::vec3 endSegment, glm::vec3 Point) const;
	/** Get the center of the adjacency edge from two faces. */
	void getAdjacencyCenter(uint32_t faceA, uint32_t faceB, float &x, float &y) const;

	Graphics::Aurora::Line *_pathDrawing;
	Graphics::Aurora::Walkmesh *_walkmeshDrawing;

	std::vector<bool> _walkableProperties; ///< Mapping between surface property and walkability.
	AStar *_aStarAlgorithm; ///< A* algorithm used.

friend class AStar;
friend class Graphics::Aurora::Walkmesh;
friend class LocalPathfinding;
};

} // End of namespace Engines

#endif // ENGINES_PATHFINDING_H
