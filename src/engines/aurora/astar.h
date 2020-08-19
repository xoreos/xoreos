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
 *  A* algorithm is used to find paths as fast as possible and as short as possible.
 */

#ifndef ENGINES_ASTAR_H
#define ENGINES_ASTAR_H

#include <vector>

#include "src/common/types.h"

namespace Engines {

class Pathfinding;

class AStar {
public:
	AStar(Pathfinding *pathfinding);
	virtual ~AStar();
	/** Find a path of faces between two points.
	 *
	 * If the width is set, it will discard paths that are not large enough. If the path is not
	 * found, return a path which is the closest to the end point.
	 *
	 *  @param startX       The x component of the starting point.
	 *  @param startY       The y component of the starting point.
	 *  @param endX         The x component of the ending point.
	 *  @param endY         The y component of the ending point.
	 *  @param facePath     The vector where the path will be stored.
	 *  @param width        The creature's width. Default is no width.
	 *  @param maxIteration The maximum number of iteration before the algorithm stops searching.
	 *  @return             Return true if a path is found. False otherwise.
	 */
	bool findPath(float startX, float startY, float endX, float endY,
	              std::vector<uint32_t> &facePath, float width = 0.f, uint32_t maxIteration = 10000);

protected:
	/** A node in the walkmesh network and its relationship within the structure and the algorithm.
	 *
	 * Here the node is the face. It could have been a vertex or a side of a face.
	 * The edges of the network is, here, the adjacency between faces. How they are weighted depends
	 * on the G cost is computed (see getGValue() ).
	 */
	struct Node {
		uint32_t face; ///< Actually the real node.
		float x; ///< The x position of the node.
		float y; ///< The y position of the node.
		uint32_t parent; ///< Parent node, UINT32_MAX for no parent.

		float G; //< Cost value (from the starting node to this node).
		float H; ///< Heuristic value (estimation cost from this node to the ending point).

		Node();
		Node(uint32_t faceID, float pX, float pY, uint32_t parentNode = UINT32_MAX);
		/** Compare the distance between two nodes. */
		bool operator<(const Node &node) const;
	};

	/** Get G cost. Basically the length from the starting point to another point.
	 *
	 *  There are many ways to compute the length between two faces. In this base
	 *  class, the length is computed from adjacency edge center to adjacency edge
	 *  center. But it could have been from centroid to centroid, it really depends
	 *  on the shape of the polygons that make the walkmesh.
	 */
	virtual float getGValue(Node &previousNode, uint32_t face, float &x, float &y) const;
	/** Get a heuristic (an estimation) of the distance between two nodes.
	 *
	 *  As this could be higly related on how the walkmesh is done. This base class
	 *  give a simple and naive implementation, that is the euclidean distance.
	 */
	virtual float getHeuristic(Node &node, Node &endNode) const;
	/** Compute the euclidean distance (usual distance) between two points in th XY plan. */
	float getEuclideanDistance(float xA, float yA, float xB, float yB) const;

	/** Find a node in a vector of nodes. */
	Node *getNode(uint32_t face, std::vector<Node> &nodes) const;
	/** Check if a vector of nodes contains a specific node. */
	bool hasNode(uint32_t face, std::vector<Node> &nodes) const;

	Pathfinding *_pathfinding; ///< Pathfinding object that contains the walkmesh.

private:
	/** Reconstruct the path of faces from the closed list and the end node. */
	void reconstructPath(Node &endNode, std::vector<Node> &closedList, std::vector<uint32_t> &path);
};

} // End of namespace Engines

#endif // ENGINES_ASTAR_H
