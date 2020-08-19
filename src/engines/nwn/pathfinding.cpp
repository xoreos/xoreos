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
 *
 */

/** @file
 *  Pathfinding for NWN.
 */

#include <cassert>

#include <algorithm>

#include "src/common/ustring.h"
#include "src/common/streamtokenizer.h"
#include "src/common/strutil.h"
#include "src/common/aabbnode.h"

#include "src/aurora/resman.h"

#include "src/engines/aurora/astar.h"
#include "src/engines/nwn/walkmeshloader.h"
#include "src/engines/nwn/pathfinding.h"

namespace Engines {

namespace NWN {

Pathfinding::Pathfinding::Face::Face(): adjacentTile(UINT32_MAX), adjacentFace(UINT32_MAX) {
}

Pathfinding::Pathfinding(std::vector<bool> walkableProperties)
    : Engines::Pathfinding(walkableProperties), _loaded(false) {
	_epsilon = 0.06f;

	AStar * aStarAlgorithm = new AStar(this);
	setAStarAlgorithm(aStarAlgorithm);

	_walkmeshLoader = new WalkmeshLoader();
}

Pathfinding::~Pathfinding() {
	delete _walkmeshLoader;
}

void Pathfinding::addTile(const Common::UString &wokFile, float *orientation, float *position) {
	Tile tile = Tile();
	tile.tileId = _tiles.size();

	_walkmeshLoader->load(::Aurora::kFileTypeWOK, wokFile, orientation, position, _vertices,
	                      tile.faces, tile.facesProperty);
	_facesCount += tile.faces.size() / 3;
	_verticesCount = _vertices.size() / 3;

	tile.adjFaces.resize(tile.faces.size(), UINT32_MAX);
	_tiles.push_back(tile);

	_aabbTrees.push_back(_walkmeshLoader->getAABB());

	// Find Adjacent tiles.
	glm::vec3 leftMax(position[0] - 5.f, position[1] + 5.f, 0.f);
	glm::vec3 bottomMax(position[0] + 5.f, position[1] - 5.f, 0.f);

	for (size_t n = 0; n < _aabbTrees.size(); ++n) {
		float x, y, z;
		_aabbTrees[n]->getMax(x, y, z);
		if (fabs(x - leftMax[0]) < 4.f && fabs(y - leftMax[1]) < 4.f) {
			connectTiles(n, _aabbTrees.size() - 1, false, x);
			continue;
		}
		if (fabs(x - bottomMax[0]) < 4.f && fabs(y - bottomMax[1]) < 4.f) {
			connectTiles(n, _aabbTrees.size() - 1, true, y);
			continue;
		}
	}
}

void Pathfinding::finalize() {
	// Merge all faces, adjacency and property included.
	_faces.clear();
	_adjFaces.clear();
	std::vector<uint32_t> startIndex;

	// Connect faces inside the tile.
	for (uint32_t t = 0; t < _tiles.size(); ++t)
		connectInnerFaces(t);

	// Adjust face indices.
	for (uint32_t t = 0; t < _tiles.size(); ++t) {
		startIndex.push_back(_faceProperty.size());
		uint32_t prevFacesCount = startIndex.back();

		// Append faces from the tiles.
		_faces.insert(_faces.end(), _tiles[t].faces.begin(), _tiles[t].faces.end());

		// Append adjacency. Adjust face index.
		_adjFaces.resize(_faces.size(), UINT32_MAX);
		for (size_t aF = 0; aF < _tiles[t].adjFaces.size(); ++aF) {
			uint32_t adjFaceTile = _tiles[t].adjFaces[aF];
			if (adjFaceTile != UINT32_MAX)
				_adjFaces[prevFacesCount * 3 + aF] = prevFacesCount + adjFaceTile;
		}

		// Append face property.
		_faceProperty.insert(_faceProperty.end(), _tiles[t].facesProperty.begin(), _tiles[t].facesProperty.end());

		// Adjust AABB.
		_aabbTrees[t]->adjustChildrenProperty(prevFacesCount);
	}

	// Set adjacencies between tiles.
	for (size_t t = 0; t < _tiles.size(); ++t) {
		for (size_t f = 0; f < _tiles[t].borderBottom.size(); ++f) {
			// Check if there is an adjacent face.
			if (_tiles[t].borderBottom[f].adjacentFace == UINT32_MAX)
				continue;

			// Set adjacent face.
			Face &face = _tiles[t].borderBottom[f];
			uint32_t faceID = face.faceId + startIndex[t];
			uint32_t adjacentFaceID = face.adjacentFace + startIndex[face.adjacentTile];
			_adjFaces[faceID * 3 + getAdjPosition(face.minVert, face.maxVert)] = adjacentFaceID;

			// Do the same for the opposite face.
			auto &borderTop = _tiles[face.adjacentTile].borderTop;
			for (auto oF = borderTop.begin(); oF != borderTop.end(); ++oF) {
				if (face.adjacentFace != oF->faceId)
					continue;

				Face &oppositeFace = *oF;
				_adjFaces[adjacentFaceID * 3 + getAdjPosition(oppositeFace.minVert, oppositeFace.maxVert)] = faceID;
				break;
			}
		}

		for (size_t f = 0; f < _tiles[t].borderLeft.size(); ++f) {
			// Check if there is an adjacent face.
			if (_tiles[t].borderLeft[f].adjacentFace == UINT32_MAX)
				continue;

			// Set adjacent face.
			Face &face = _tiles[t].borderLeft[f];
			uint32_t faceID = face.faceId + startIndex[t];
			uint32_t adjacentFaceID = face.adjacentFace + startIndex[face.adjacentTile];
			_adjFaces[faceID * 3 + getAdjPosition(face.minVert, face.maxVert)] = adjacentFaceID;

			// Do the same for the opposite face.
			auto &borderRight = _tiles[face.adjacentTile].borderRight;
			for (auto oF = borderRight.begin(); oF != borderRight.end(); ++oF) {
				if (face.adjacentFace != oF->faceId)
					continue;

				Face &oppositeFace = *oF;
				_adjFaces[adjacentFaceID * 3 + getAdjPosition(oppositeFace.minVert, oppositeFace.maxVert)] = faceID;
				break;
			}
		}
	}

	_loaded = true;
}

bool Pathfinding::loaded() const {
	return _loaded;
}

void Pathfinding::connectInnerFaces(uint32_t tile) {
	for (size_t f = 0; f < _tiles[tile].faces.size() / 3; ++f) {
		for (size_t i = 0; i < _tiles[tile].faces.size() / 3; ++i) {
			if (f == i)
				continue;

			// Avoid unwalkable face
			if (!faceInTileWalkable(tile, f) || !faceInTileWalkable(tile, i))
				continue;

			uint8_t count = 0;
			std::vector<uint8_t> edges;
			for (uint32_t c = 0; c < 3; ++c) {
				for (uint32_t e = 0; e < 3; ++e) {
					if (_tiles[tile].faces[f * 3 + e] == _tiles[tile].faces[i * 3 + c]) {
						++count;
						edges.push_back(e);
						break;
					}
				}
			}

			if (count > 1) {
				if (edges[0] + edges[1] == 1) {
					_tiles[tile].adjFaces[f * 3] = i;
				} else if (edges[0] + edges[1] == 3) {
					_tiles[tile].adjFaces[f * 3 + 1] = i;
				} else if (edges[0] + edges[1] == 2) {
					_tiles[tile].adjFaces[f * 3 + 2] = i;
				}
			}

		}
	}
}

void Pathfinding::connectTiles(uint32_t tileA, uint32_t tileB, bool yAxis, float axisPosition) {
	// First, collect faces on the border of the new tile (tileB) and the other tile (tileA).
	// Second, match them to the (almost) exact faces of the other tile (which should already have collect border faces).
	// Finally, split faces as needed.

	// Collect faces on the border.

	// How close two vertices should stated as the same.
	const float epsilon = 0.08;
	// If yAxis is true the border is bottom/top (tileB/tileA), left/right (tileB/tileA) otherwise.
	std::vector<Face> &borderB = yAxis ? _tiles[tileB].borderBottom : _tiles[tileB].borderLeft;
	std::vector<Face> &borderA = yAxis ? _tiles[tileA].borderTop : _tiles[tileA].borderRight;

	getBorderface(borderA, tileA, yAxis, axisPosition, epsilon);
	getBorderface(borderB, tileB, yAxis, axisPosition, epsilon);

	uint32_t posA = 0;
	uint32_t posB = 0;

	while (posA < borderA.size() && posB < borderB.size()) {
		Face &faceA = borderA[posA];
		Face &faceB = borderB[posB];
		if (glm::distance(faceA.vert[faceA.minVert], faceB.vert[faceB.minVert]) > epsilon) {
			// The min vertices don't match.Either we have a lonely face or one face is shifted and
			// we need to cut the other.

			// Check between min and max.
			const bool faceAMinInB = faceA.min + epsilon <= faceB.max && faceA.min - epsilon >= faceB.min;
			const bool faceBMinInA = faceB.min + epsilon <= faceA.max && faceB.min - epsilon >= faceA.min;
			// Check along the Z axis.
			const float minAZ = faceA.vert[faceA.minVert][2];
			const float minBZ = faceB.vert[faceB.minVert][2];
			const float maxAZ = faceA.vert[faceA.maxVert][2];
			const float maxBZ = faceB.vert[faceB.maxVert][2];
			const bool faceAZInB = (minAZ <= minBZ + epsilon && minAZ >= maxBZ - epsilon)
			                       || (minAZ <= maxBZ + epsilon && minAZ >= minBZ - epsilon);
			const bool faceBZInA = (minBZ <= minAZ + epsilon && minBZ >= maxAZ - epsilon)
			                       || (minBZ <= maxAZ + epsilon && minBZ >= minAZ - epsilon);

			if ((faceAMinInB && faceAZInB) || (faceBMinInA && faceBZInA)) {
				// There is a shifted face, cut accordingly.
				if (faceAMinInB) {
					Face newFaceInB = cutFaceAt(false, _tiles[tileB], _tiles[tileA], faceB, faceA);
					borderB.insert(borderB.begin() + posB, newFaceInB);
					++posB;
				} else {
					Face newFaceInA = cutFaceAt(false, _tiles[tileA], _tiles[tileB], faceA, faceB);
					borderA.insert(borderA.begin() + posA, newFaceInA);
					++posA;
				}
				++_facesCount;
			} else {
				// This is a lonely face, skip it.
				if (faceA.min < faceB.min) {
					borderA.erase(borderA.begin() + posA);
				} else {
					borderB.erase(borderB.begin() + posB);
				}
			}
			continue;
		} else {
			// Adjust the vertex.
			uint32_t vertexToMove = _tiles[tileB].faces[faceB.faceId * 3 + faceB.minVert];
			for (uint32_t v = 0; v < 3; ++v)
				_vertices[vertexToMove * 3 + v] = faceA.vert[faceA.minVert][v];
		}

		// Check if the faces fit.
		if (fabs(faceA.max - faceB.max) < epsilon
			&& glm::distance(faceA.vert[faceA.maxVert], faceB.vert[faceB.maxVert]) < epsilon) {
			// Set adjacency properties.
			borderA[posA].adjacentTile = tileB;
			borderA[posA].adjacentFace = faceB.faceId;
			borderB[posB].adjacentTile = tileA;
			borderB[posB].adjacentFace = faceA.faceId;

			// Adjust vertex.
			uint32_t vertexToMove = _tiles[tileB].faces[faceB.faceId * 3 + faceB.maxVert];
			for (uint32_t v = 0; v < 3; ++v)
				_vertices[vertexToMove * 3 + v] = faceA.vert[faceA.maxVert][v];

			++posA; ++posB;
		} else {
			// Add a new face.
			bool isBToCut = faceA.max < faceB.max;
			Face &faceToCut = isBToCut ? faceB : faceA;
			Tile &tileToCut = isBToCut ? _tiles[tileB] : _tiles[tileA];
			Face &faceGood = isBToCut ? faceA : faceB;
			Tile &tileGood = isBToCut ? _tiles[tileA] : _tiles[tileB];

			Face newFace = cutFaceAt(true, tileToCut, tileGood, faceToCut, faceGood);
			// Add the new face to the border after the current one.
			if (isBToCut) {
				borderB.insert(borderB.begin() + posB + 1, newFace);
			} else {
				borderA.insert(borderA.begin() + posA + 1, newFace);
			}

			++posA; ++posB;
			_facesCount++;
		}
	}

	// Remove lonely face.
	if (borderA.size() != posA) {
		borderA.resize(posA + 1);
	}
	if (borderB.size() != posB) {
		borderB.resize(posB + 1);
	}
}

void Pathfinding::Face::computeMinOnAxis() {
	bool onAxis = false;
	axisVert.clear();
	for (uint32_t v = 0; v < 3; ++v) {
		if (fabs(vert[v][(uint32_t) yAxis] - axisPosition) < epsilon) {
			axisVert.push_back(v);
			onAxis = true;
		} else {
			oppositeVert = v;
		}
	}

	if (!onAxis) {
		error("No vertex on the axis");
	}

	if (axisVert.size() > 1) {
		if (vert[axisVert[0]][(uint32_t) !yAxis] < vert[axisVert[1]][(uint32_t) !yAxis]) {
			minVert = axisVert[0];
			maxVert = axisVert[1];
		} else {
			minVert = axisVert[1];
			maxVert = axisVert[0];
		}
	} else {
		error("At least two vertices should be on the axis");
	}

	min = vert[minVert][(uint32_t) !yAxis];
	max = vert[maxVert][(uint32_t) !yAxis];
}

bool Pathfinding::Face::operator<(const Face &face) const {
	return this->min < face.min;
}

uint32_t Pathfinding::getAdjPosition(uint32_t vertA, uint32_t vertB) const {
	uint32_t pos = vertA + vertB;
	if (pos == 3) {
		return 1;
	} else if (pos == 1) {
		return 0;
	}
	return pos;
}

void Pathfinding::getBorderface(std::vector<Face> &border, uint32_t tile, bool yAxis, float axisPosition, float epsilon) const {
	glm::vec3 vert[3];
	for (size_t f = 0; f < _tiles[tile].facesProperty.size(); ++f) {
		// Avoid unwalkable face.
		if (!faceInTileWalkable(tile, f))
			continue;

		getVertex(_tiles[tile].faces[f * 3], vert[0], false);
		getVertex(_tiles[tile].faces[f * 3 + 1], vert[1], false);
		getVertex(_tiles[tile].faces[f * 3 + 2], vert[2], false);

		// Discard degenerate face.
		if (vert[0] == vert[1] || vert[0] == vert[2] || vert[1] == vert[2]) {
			continue;
		}
		for (uint32_t v = 0; v < 3; ++v) {
			if (fabs(vert[v][(int32_t) yAxis] - axisPosition) < epsilon
			    && fabs(vert[(v + 1) % 3][(int32_t) yAxis] - axisPosition) < epsilon) {
				Face face = Face();
				face.faceId = f;
				face.axisPosition = axisPosition;
				face.yAxis = yAxis;
				face.vert[0] = vert[0]; face.vert[1] = vert[1]; face.vert[2] = vert[2];
				face.epsilon = epsilon;
				face.computeMinOnAxis();

				border.push_back(face);

				break;
			}
		}
	}

	// Sort faces along the axis.
	std::sort(border.begin(), border.end());
}

void Pathfinding::getMinMaxFromFace(Face &face, float min[3], float max[3]) const {
	min[0] = face.vert[0][0]; min[1] = face.vert[0][1]; min[2] = face.vert[0][2];
	max[0] = face.vert[0][0]; max[1] = face.vert[0][1]; max[2] = face.vert[0][2];

	for (uint32_t i = 1; i < 3; ++i) {
		for (uint32_t j = 0; j < 2; ++j) {
			if (face.vert[i][j] < min[j])
				min[j] = face.vert[i][j];

			if (face.vert[i][j] > max[j])
				max[j] = face.vert[i][j];
		}
	}
}

bool Pathfinding::faceInTileWalkable(uint32_t tile, uint32_t face) const {
	const uint32_t &property = _tiles[tile].facesProperty[face];
	return surfaceWalkable(property);
}

Pathfinding::Face Pathfinding::cutFaceAt(bool isAtGoodMax, Tile &tileToCut, Tile &tileGood,
                                         Face &faceToCut, Face &faceGood) {
	// Get center of the face to be cut for AABB.
	glm::vec2 center(0.f, 0.f);
	std::vector<glm::vec3> vertices;
	for (uint32_t v = 0; v < 3; ++v) {
		vertices.push_back(faceToCut.vert[v]);
	}
	findCenter(vertices, center[0], center[1]);

	// Check if there is the same face on other borders.
	Face *usedFace = 0;
	Face *adjUsedFace = 0;
	// Check orthogonal borders.
	if (faceToCut.yAxis) {
		std::vector<Face> *oBorder = &tileToCut.borderLeft;
		for (std::vector<Face>::iterator it = oBorder->begin(); it != oBorder->end(); ++it) {
			if (it->faceId != faceToCut.faceId)
				continue;

			usedFace = &(*it);
			std::vector<Face> &adjBorder = _tiles[usedFace->adjacentTile].borderRight;
			for (std::vector<Face>::iterator adjF = adjBorder.begin(); adjF != adjBorder.end(); ++adjF) {
				if (adjF->adjacentFace != usedFace->faceId)
					continue;

				adjUsedFace = &(*adjF);
				break;
			}
			break;
		}

		if (!usedFace) {
			oBorder = &tileToCut.borderRight;
			for (std::vector<Face>::iterator it = oBorder->begin(); it != oBorder->end(); ++it) {
				if (it->faceId != faceToCut.faceId)
					continue;

				usedFace = &(*it);
				std::vector<Face> &adjBorder = _tiles[usedFace->adjacentTile].borderLeft;
				for (std::vector<Face>::iterator adjF = adjBorder.begin(); adjF != adjBorder.end(); ++adjF) {
					if (adjF->adjacentFace != usedFace->faceId)
						continue;

					adjUsedFace = &(*adjF);
					break;
				}
				break;
			}
		}
	} else {
		std::vector<Face> *oBorder = &tileToCut.borderTop;
		for (std::vector<Face>::iterator it = oBorder->begin(); it != oBorder->end(); ++it) {
			if (it->faceId != faceToCut.faceId)
				continue;

			usedFace = &(*it);
			std::vector<Face> &adjBorder = _tiles[usedFace->adjacentTile].borderBottom;
			for (std::vector<Face>::iterator adjF = adjBorder.begin(); adjF != adjBorder.end(); ++adjF) {
				if (adjF->adjacentFace != usedFace->faceId)
					continue;

				adjUsedFace = &(*adjF);
				break;
			}
			break;
		}

		if (!usedFace) {
			oBorder = &tileToCut.borderBottom;
			for (std::vector<Face>::iterator it = oBorder->begin(); it != oBorder->end(); ++it) {
				if (it->faceId != faceToCut.faceId)
					continue;

				usedFace = &(*it);
				std::vector<Face> &adjBorder = _tiles[usedFace->adjacentTile].borderTop;
				for (std::vector<Face>::iterator adjF = adjBorder.begin(); adjF != adjBorder.end(); ++adjF) {
					if (adjF->adjacentFace != usedFace->faceId)
						continue;

					adjUsedFace = &(*adjF);
					break;
				}
				break;
			}
		}
	}

	// Create a new face.
	Face newFace = faceToCut;
	newFace.faceId = tileToCut.facesProperty.size();
	if (isAtGoodMax) {
		newFace.min = faceGood.max;
		newFace.max = faceToCut.max;
		newFace.vert[faceToCut.minVert] = faceGood.vert[faceGood.maxVert];
	} else {
		newFace.max = faceGood.min;
		newFace.min = faceToCut.min;
		newFace.vert[faceToCut.maxVert] = faceGood.vert[faceGood.minVert];
	}

	// Add vertex indices from the cut face.
	for (uint32_t v = 0; v < 3; ++v) {
		tileToCut.faces.push_back(tileToCut.faces[faceToCut.faceId * 3 + v]);
		tileToCut.adjFaces.push_back(UINT32_MAX);
	}

	// Use the max/min vertex from the good face as the min/max vertex for the new face.
	if (isAtGoodMax) {
		tileToCut.faces[newFace.faceId * 3 + newFace.minVert] = tileGood.faces[faceGood.faceId * 3 + faceGood.maxVert];
	} else {
		tileToCut.faces[newFace.faceId * 3 + newFace.maxVert] = tileGood.faces[faceGood.faceId * 3 + faceGood.minVert];
	}
	// Set face property.
	tileToCut.facesProperty.push_back(tileToCut.facesProperty[faceToCut.faceId]);

	// Change max/min vertex for the cut face.
	if (isAtGoodMax) {
		faceToCut.vert[faceToCut.maxVert] = faceGood.vert[faceGood.maxVert];
		faceToCut.max = faceGood.max;
		tileToCut.faces[faceToCut.faceId * 3 + faceToCut.maxVert] = tileGood.faces[faceGood.faceId * 3 + faceGood.maxVert];
	} else {
		faceToCut.vert[faceToCut.minVert] = faceGood.vert[faceGood.minVert];
		faceToCut.min = faceGood.min;
		tileToCut.faces[faceToCut.faceId * 3 + faceToCut.minVert] = tileGood.faces[faceGood.faceId * 3 + faceGood.minVert];
	}

	// Set adjacency properties.
	faceGood.adjacentTile = tileToCut.tileId;
	faceGood.adjacentFace = faceToCut.faceId;
	faceToCut.adjacentTile = tileGood.tileId;
	faceToCut.adjacentFace = faceGood.faceId;

	// Switch id for face on other border if needed.
	if (usedFace) {
		// If the faceToCut and the other face share at least two vertices,
		// the faceToCut is still on the other border. Change id otherwise.
		uint8_t sameVertCount = 0;

		for (size_t v = 0; v < 3; ++v) {
			for (size_t u = 0; u < 3; ++u) {
				if (glm::distance(faceToCut.vert[v], adjUsedFace->vert[u]) < adjUsedFace->epsilon)
					++sameVertCount;
			}
		}
		if (sameVertCount < 2) {
			usedFace->faceId = newFace.faceId;
			for (uint32_t v = 0; v < 3; ++v) {
				usedFace->vert[v] = newFace.vert[v];
			}
			// Look for the adjacent face.
			adjUsedFace->adjacentFace = newFace.faceId;
		}
	}

	// Create two new AABB nodes.
	float min[3], max[3];
	getMinMaxFromFace(faceToCut, min, max);
	Common::AABBNode *cutNode = new Common::AABBNode(min, max, faceToCut.faceId);
	getMinMaxFromFace(newFace, min, max);
	Common::AABBNode *newNode = new Common::AABBNode(min, max, newFace.faceId);

	// Get AABB from cut face.
	std::vector<Common::AABBNode *> nodes;
	_aabbTrees[tileToCut.tileId]->getNodesInAABox(center - 0.2f, center + 0.2f, nodes);
	Common::AABBNode *parentNode = 0;
	for (std::vector<Common::AABBNode *>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ((uint32_t) (*it)->getProperty() == faceToCut.faceId) {
			parentNode = *it;
			break;
		}
	}

	// AABB node not found.
	assert(parentNode != 0);

	// Set property to -1 i.e. has children.
	parentNode->adjustChildrenProperty(-1 - (int32_t) faceToCut.faceId);
	parentNode->setChildren(cutNode, newNode);

	return newFace;
}

} // End of namespace NWN

} // End of namespace Engines
