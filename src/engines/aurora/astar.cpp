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

#include <algorithm>

#include "src/common/util.h"

#include "src/engines/aurora/pathfinding.h"
#include "src/engines/aurora/astar.h"

namespace Engines {

AStar::AStar(Engines::Pathfinding* pathfinding) : _pathfinding(pathfinding) {
}

AStar::~AStar() {
}

AStar::Node::Node(): face(UINT32_MAX), x(0.f), y(0.f), parent(UINT32_MAX), G(0.f), H(0.f) {
}

AStar::Node::Node(uint32_t faceID, float pX, float pY, uint32_t parentNode):
face(faceID), x(pX), y(pY), parent(parentNode), G(0.f), H(0.f) {
}

bool AStar::Node::operator<(const Node &node) const {
	return G + H < node.G + node.H;
}

bool AStar::findPath(float startX, float startY, float endX, float endY,
                     std::vector<uint32_t> &facePath, float width, uint32_t maxIteration) {

	// Cleaning the futur path.
	facePath.clear();

	// Find faces start and end points belong.
	uint32_t startFace = _pathfinding->findFace(startX, startY, false);
	uint32_t endFace = _pathfinding->findFace(endX, endY, false);

	// Check if start and end points are in the walkmesh.
	if (startFace == UINT32_MAX || endFace == UINT32_MAX)
		return false;

	// Check if start and end points are in the same face.
	if (startFace == endFace) {
		facePath.push_back(startFace);
		return true;
	}

	// Init nodes and lists.
	Node startNode = Node(startFace, startX, startY);
	Node endNode   = Node(endFace, endX, endY);

	startNode.G = 0.f;
	startNode.H = getHeuristic(startNode, endNode);
	// Get track of the closest node near the end in case of the unavailable path.
	Node closestToEnd = startNode;

	std::vector<Node> openList;
	std::vector<Node> closedList;
	openList.push_back(startNode);

	// Searching...
	for (uint32_t it = 0; it < maxIteration; ++it) {
		if (openList.empty())
			break;

		Node current = openList.front();

		if (current.face == endNode.face) {
			reconstructPath(current, closedList, facePath);
			return true;
		}

		openList.erase(openList.begin());
		closedList.push_back(current);

		std::vector<uint32_t> adjFaces;
		_pathfinding->getAdjacentFaces(current.face, current.parent, adjFaces);
		for (std::vector<uint32_t>::iterator a = adjFaces.begin(); a != adjFaces.end(); ++a) {
			// Check if it has been already evaluated.
			if (hasNode(*a, closedList))
				continue;

			// Check if the creature can go through to the adjacent face.
			if (width > 0.f && !_pathfinding->goThrough(current.face, *a, width))
				continue;

			// Distance from start point to this node.
			float x, y;
			float gScore = current.G + getGValue(current, *a, x, y);

			// Check if it is a new node.
			Node *adjNode = getNode(*a, openList);
			bool isThere = adjNode != 0;
			if (!isThere) {
				adjNode = new Node(*a, x, y);
				adjNode->parent = current.face;
			} else if (gScore >= adjNode->G) {
				continue;
			}

			// adjNode is the best node up to now, update/add.
			adjNode->parent = current.face;
			adjNode->G = gScore;
			adjNode->H = getHeuristic(*adjNode, endNode);
			if (adjNode->H < closestToEnd.H)
				closestToEnd = *adjNode;
			if (!isThere) {
				openList.push_back(*adjNode);
				delete adjNode;
			}

			std::sort(openList.begin(), openList.end());
		}
	}

	reconstructPath(closestToEnd, closedList, facePath);
	return false;
}

float AStar::getGValue(Node &previousNode, uint32_t face, float &x, float &y) const {
	_pathfinding->getAdjacencyCenter(previousNode.face, face, x, y);
	return getEuclideanDistance(previousNode.x,previousNode.y, x, y);
}

float AStar::getHeuristic(Node &node, Node &endNode) const {
	// Naive estimation.
	return getEuclideanDistance(node.x,node.y, endNode.x,endNode.y);
}

AStar::Node *AStar::getNode(uint32_t face, std::vector<Node> &nodes) const {
	for (std::vector<Node>::iterator n = nodes.begin(); n != nodes.end(); ++n) {
		if ((*n).face == face) {
			return &(*n);
		}
	}

	return 0;
}

bool AStar::hasNode(uint32_t face, std::vector<Node> &nodes) const {
	for (std::vector<Node>::iterator n = nodes.begin(); n != nodes.end(); ++n) {
		if ((*n).face == face) {
			return true;
		}
	}

	return false;
}

float AStar::getEuclideanDistance(float xA, float yA, float xB, float yB) const {
	return sqrt(pow(xA - xB, 2.f) + pow(yA - yB, 2.f));
}

void AStar::reconstructPath(Node &endNode, std::vector<Node> &closedList, std::vector<uint32_t> &path) {
	Node &cNode = endNode;
	path.push_back(endNode.face);
	path.push_back(endNode.parent);

	while (cNode.parent != UINT32_MAX) {
		for (std::vector<Node>::iterator n = closedList.begin(); n != closedList.end(); ++n) {
			if (cNode.parent != (*n).face)
				continue;

			cNode = (*n);
			if (cNode.parent != UINT32_MAX)
				path.push_back(cNode.parent);

			break;
		}
	}
	std::reverse(path.begin(), path.end());
}

} // End of namespace Engines
