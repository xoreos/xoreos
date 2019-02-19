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
 *  Walkmesh loader class for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_PATH_WALKMESHLOADER_H
#define ENGINES_KOTORBASE_PATH_WALKMESHLOADER_H

#include <map>

#include "glm/mat4x4.hpp"

#include "src/aurora/types.h"

namespace Common {
	class UString;
	class AABBNode;
	class SeekableReadStream;
	class StreamTokenizer;
}

namespace Engines {

namespace KotOR {

class Pathfinding;

class WalkmeshLoader {
public:
	WalkmeshLoader();

	Common::AABBNode *getAABB();

	void load(Aurora::FileType fileType,
	          const Common::UString &resRef,
	          const glm::mat4 &transform,
	          std::vector<float> &vertices,
	          std::vector<uint32> &faces,
	          std::vector<uint32> &faceTypes,
	          std::vector<uint32> &adjFaces,
	          std::map<uint32, uint32> &adjRooms,
	          Pathfinding *pathfinding = 0);

private:
	void multiply(const float *v, const glm::mat4 &m, float *rv) const;

	void appendFaceTypes(Common::SeekableReadStream &stream,
	                     std::vector<uint32> &faceTypes,
	                     uint32 faceCount,
	                     uint32 faceTypeOffset);

	void appendFaces(Common::SeekableReadStream &stream,
	                 std::vector<uint32> &faces, size_t prevVertexCount,
	                 uint32 faceCount,
	                 uint32 faceOffset);

	void appendVertices(Common::SeekableReadStream &stream,
	                    std::vector<float> &vertices,
	                    uint32 vertexCount,
	                    uint32 vertexOffset,
	                    const glm::mat4 &transform);

	void appendAdjFaces(Common::SeekableReadStream &stream,
	                    std::vector<uint32> &adjFaces, size_t prevFaceCount,
	                    uint32 faceAdjCount,
	                    uint32 faceAdjOffset);

	void appendPerimEdges(Common::SeekableReadStream &stream,
	                      std::map<uint32, uint32> &adjRooms,
	                      uint32 perimEdgeCount,
	                      uint32 perimEdgeOffset);

	Common::AABBNode *getAABB(Common::SeekableReadStream &stream,
	                          uint32 nodeOffset, uint32 AABBsOffset,
	                          size_t prevFaceCount);

	std::vector<uint32> _walkableFaces;
	Common::AABBNode *_node;
	Pathfinding *_pathfinding;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_PATH_WALKMESHLOADER_H
