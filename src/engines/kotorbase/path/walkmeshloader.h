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

#include "external/glm/mat4x4.hpp"

#include "src/aurora/types.h"

namespace Common {
	class UString;
	class AABBNode;
	class SeekableReadStream;
	class StreamTokenizer;
}

namespace Engines {

namespace KotORBase {

class Pathfinding;

class WalkmeshLoader {
public:
	WalkmeshLoader();

	Common::AABBNode *getAABB();

	void load(Aurora::FileType fileType,
	          const Common::UString &resRef,
	          const glm::mat4 &transform,
	          std::vector<float> &vertices,
	          std::vector<uint32_t> &faces,
	          std::vector<uint32_t> &faceTypes,
	          std::vector<uint32_t> &adjFaces,
	          std::map<uint32_t, uint32_t> &adjRooms,
	          Pathfinding *pathfinding = 0);

private:
	void multiply(const float *v, const glm::mat4 &m, float *rv) const;

	void appendFaceTypes(Common::SeekableReadStream &stream,
	                     std::vector<uint32_t> &faceTypes,
	                     uint32_t faceCount,
	                     uint32_t faceTypeOffset);

	void appendFaces(Common::SeekableReadStream &stream,
	                 std::vector<uint32_t> &faces, size_t prevVertexCount,
	                 uint32_t faceCount,
	                 uint32_t faceOffset);

	void appendVertices(Common::SeekableReadStream &stream,
	                    std::vector<float> &vertices,
	                    uint32_t vertexCount,
	                    uint32_t vertexOffset,
	                    const glm::mat4 &transform);

	void appendAdjFaces(Common::SeekableReadStream &stream,
	                    std::vector<uint32_t> &adjFaces, size_t prevFaceCount,
	                    uint32_t faceAdjCount,
	                    uint32_t faceAdjOffset);

	void appendPerimEdges(Common::SeekableReadStream &stream,
	                      std::map<uint32_t, uint32_t> &adjRooms,
	                      uint32_t perimEdgeCount,
	                      uint32_t perimEdgeOffset);

	Common::AABBNode *getAABB(Common::SeekableReadStream &stream,
	                          uint32_t nodeOffset, uint32_t AABBsOffset,
	                          size_t prevFaceCount);

	std::vector<uint32_t> _walkableFaces;
	Common::AABBNode *_node;
	Pathfinding *_pathfinding;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_PATH_WALKMESHLOADER_H
