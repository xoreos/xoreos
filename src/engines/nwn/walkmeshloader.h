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
 *  Class that loads a walkmesh file.
 */

#ifndef ENGINES_NWN_WALKMESHLOADER_H
#define ENGINES_NWN_WALKMESHLOADER_H

#include "external/glm/mat4x4.hpp"

#include "src/aurora/types.h"
#include "src/common/ustring.h"

namespace Common {
class AABBNode;
class SeekableReadStream;
class StreamTokenizer;
}

namespace Engines {

namespace NWN {

class WalkmeshLoader {
public:
	WalkmeshLoader();
	~WalkmeshLoader();

	void load(Aurora::FileType fileType, const Common::UString &name, float orientation[4], float position[3],
	          std::vector<float> &vertices, std::vector<uint32_t> &faces, std::vector<uint32_t> &facesProperty,
	          const Common::UString &filterNode = "");

	Common::AABBNode *getAABB();

private:
	/** Read the vertices positions from an ASCII stream. */
	void readVerts(size_t n, float *position, Common::SeekableReadStream *stream,
	               Common::StreamTokenizer *tokenize, glm::mat4x4 &rotation, std::vector<float> &vertices);
	/** Read the faces vertices from an ASCII stream. */
	void readFaces(size_t n, Common::SeekableReadStream *stream, Common::StreamTokenizer *tokenize,
	               std::vector<uint32_t> &faces, std::vector<uint32_t> &facesProperty, uint32_t startVertex);
	/** Read floats number from lines of Common::UString. */
	void readFloats(const std::vector<Common::UString> &strings,
	                float *floats, uint32_t n, uint32_t start);
	/** Read and construct an AABB tree/node from an ASCII stream. */
	Common::AABBNode *readAABB(float *position, uint8_t orientation,
	                           Common::SeekableReadStream *stream,
	                           Common::StreamTokenizer *tokenize);
	Common::AABBNode *buildAABBTree(std::vector<float> &vertices, std::vector<uint32_t> &faces);
	Common::AABBNode *createAABB(uint32_t face, std::vector<float> &vertices,
	                             std::vector<uint32_t> &faces) const;
	float AABBCompatibility(Common::AABBNode *nodeA, Common::AABBNode *nodeB,
	                        float min[], float max[]) const;
	/** Change the axis orientation of coordonates. */
	void changeOrientation(uint8_t orientation, float *position) const;

	Common::AABBNode *_node;
	/** A map between vertices with same coordinates. */
	std::map<size_t, size_t> _sameVertex;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_WALKMESHLOADER_H
