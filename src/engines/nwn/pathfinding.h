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

#ifndef ENGINES_NWN_PATHFINDING_H
#define ENGINES_NWN_PATHFINDING_H

#include "src/engines/aurora/pathfinding.h"

namespace Common {
class AABBNode;
class SeekableReadStream;
class StreamTokenizer;
class Matrix4x4;
}

namespace Engines {

namespace NWN {

class WalkmeshLoader;

class Pathfinding : public Engines::Pathfinding {
public:
	/** Construct a pathfinding object for NWN. */
	Pathfinding(std::vector<bool> walkableProperties);
	~Pathfinding();

	/** Add wok tile data. */
	void addTile(const Common::UString &wokFile, float *orientation, float *position);
	/** Connect all tiles together. Should be called before any path request. */
	void finalize();
	/** Is the the walkmesh already loaded and ready to be used? */
	bool loaded() const;

private:
	/** Informations about adjacencies and the position along the border of the tile. */
	struct Face {
		uint32 faceId;               ///< Id of the face in the current tile.
		uint32 adjacentTile;         ///< Id of the adjacent tile.
		uint32 adjacentFace;         ///< Id of the adjacent face in the adjacent tile.
		glm::vec3 vert[3];           ///< Vertices of the face.
		bool yAxis;                  ///< Is the border of the related tile along the y axis?
		float axisPosition;          ///< The position of the border.
		float epsilon;               ///< The epsilon value to state if two vertices are close.
		std::vector<uint8> axisVert; ///< The vertices along the border axis.
		uint8 oppositeVert;          ///< The vertex at the other end of the border.
		uint8 minVert;               ///< The lower vertex on the border.
		uint8 maxVert;               ///< The higher vertex on the border.
		float min;                   ///< The minimal position on the border.
		float max;                   ///< The maximal position on the border.

		Face();
		void computeMinOnAxis();
		bool operator<(const Face &face) const;
	};

	/** Structure used to connect the tiles together. */
	struct Tile {
		uint32 tileId;
		uint32 xPosition;
		uint32 yPosition;
		std::vector<uint32> faces;
		std::vector<uint32> adjFaces;
		std::vector<uint32> facesProperty;
		std::vector<Face> borderBottom;
		std::vector<Face> borderRight;
		std::vector<Face> borderLeft;
		std::vector<Face> borderTop;
	};

	/** Find face adjacencies between two tiles and make all border faces match an other face. */
	void connectTiles(uint32 tileA, uint32 tileB, bool yAxis, float axisPosition);
	/** Find face adjacencies within a tile. */
	void connectInnerFaces(uint32 tile);
	/** Get the position in the adjacent face vector from the vertices positions. */
	uint32 getAdjPosition(uint32 vertA, uint32 vertB) const;
	/** Find the faces on the border of a tile. */
	void getBorderface(std::vector<Face> &border, uint32 tile, bool yAxis, float axisPosition, float epsilon) const;
	/** Find out the order of the vertices of a face along the border of a tile. */
	void getMinMaxFromFace(Face &face, float min[3], float max[3]) const;
	/** Is a face, in a given tile, walkable? */
	bool faceInTileWalkable(uint32 tile, uint32 face) const;
	/** Cut a face in two. The face to be cut is shrink and the other part is returned. */
	Face cutFaceAt(bool isAtGoodMax, Tile &tileToCut, Tile &tileGood, Face &faceToCut, Face &faceGood);

	bool _loaded;                     ///< State if the walkmesh is finalized.
	std::vector<uint32> _startVertex; ///< Starting index of the vertex for each tiles.
	std::vector<Tile> _tiles;         ///< Tiles of the area.

	WalkmeshLoader *_walkmeshLoader;  ///< Walkmesh loader.
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_PATHFINDING_H
