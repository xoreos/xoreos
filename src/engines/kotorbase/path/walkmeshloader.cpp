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

#include <memory>

#include "src/common/endianness.h"
#include "src/common/aabbnode.h"

#include "src/aurora/resman.h"

#include "src/engines/kotorbase/path/pathfinding.h"
#include "src/engines/kotorbase/path/walkmeshloader.h"

namespace Engines {

namespace KotORBase {

WalkmeshLoader::WalkmeshLoader() : _node(0), _pathfinding(0) {
}

void WalkmeshLoader::load(Aurora::FileType fileType,
                          const Common::UString &resRef,
                          const glm::mat4 &transform,
                          std::vector<float> &vertices,
                          std::vector<uint32_t> &faces,
                          std::vector<uint32_t> &faceTypes,
                          std::vector<uint32_t> &adjFaces,
                          std::map<uint32_t, uint32_t> &adjRooms,
                          Pathfinding *pathfinding) {

	_pathfinding = pathfinding;
	_node = 0;
	_walkableFaces.clear();
	std::unique_ptr<Common::SeekableReadStream> stream(ResMan.getResource(resRef, fileType));

	if (!stream) {
		warning("Walkmesh file not found: %s", resRef.c_str());
		return;
	}

	try {
		stream->seek(0);

		/** Header format:
		 *
		 *  uint8_t[8]  - BWM version
		 *  uint32_t    - walkmesh type
		 *  uint8_t[48] - reserved
		 *  float[3]    - position
		 */

		if (stream->readUint32BE() != MKTAG('B', 'W', 'M', ' ') ||
				stream->readUint32BE() != MKTAG('V', '1', '.', '0'))
			throw Common::Exception("Invalid BWM version");

		stream->skip(64);

		const uint32_t vertexCount = stream->readUint32LE();
		if (vertexCount == 0)
			return;

		const uint32_t vertexOffset   = stream->readUint32LE();
		const uint32_t faceCount      = stream->readUint32LE();
		const uint32_t faceOffset     = stream->readUint32LE();
		const uint32_t faceTypeOffset = stream->readUint32LE();
		stream->skip(8);
		const uint32_t AABBCount   = stream->readUint32LE();
		const uint32_t AABBsOffset = stream->readUint32LE();
		stream->skip(4);
		const uint32_t faceAdjCount     = stream->readUint32LE();
		const uint32_t faceAdjOffset    = stream->readUint32LE();
		const uint32_t perimEdgesCount  = stream->readUint32LE();
		const uint32_t perimEdgesOffset = stream->readUint32LE();

		const size_t prevVertexCount = vertices.size() / 3;
		const size_t prevFaceCount = faces.size() / 3;

		appendFaceTypes(*stream, faceTypes, faceCount, faceTypeOffset);
		appendFaces(*stream, faces, prevVertexCount, faceCount, faceOffset);
		adjFaces.resize(faces.size(), UINT32_MAX);
		appendAdjFaces(*stream, adjFaces, prevFaceCount, faceAdjCount, faceAdjOffset);
		appendVertices(*stream, vertices, vertexCount, vertexOffset, transform);

		if (perimEdgesCount > 0)
			appendPerimEdges(*stream, adjRooms, perimEdgesCount, perimEdgesOffset);

		if (AABBCount > 0)
			_node = getAABB(*stream, AABBsOffset, AABBsOffset, prevFaceCount);
	} catch (Common::Exception &e) {
		warning("Walkmesh load failed: %s %s", resRef.c_str(), e.what());
	}
}

Common::AABBNode *WalkmeshLoader::getAABB() {
	return _node;
}

void WalkmeshLoader::appendFaceTypes(Common::SeekableReadStream &stream,
                                     std::vector<uint32_t> &faceTypes,
                                     uint32_t faceCount,
                                     uint32_t faceTypeOffset) {
	stream.seek(faceTypeOffset);

	const size_t prevFaceCount = faceTypes.size();
	faceTypes.resize(prevFaceCount + faceCount);

	for (size_t f = prevFaceCount; f < prevFaceCount + faceCount; ++f) {
		faceTypes[f] = stream.readUint32LE();

		// Store walkable faces. It is used for the face adjacencies.
		if (_pathfinding && _pathfinding->faceWalkable(f))
			_walkableFaces.push_back(f);
	}
}

void WalkmeshLoader::appendFaces(Common::SeekableReadStream &stream,
                                 std::vector<uint32_t> &faces,
                                 size_t prevVertexCount,
                                 uint32_t faceCount,
                                 uint32_t faceOffset) {
	stream.seek(faceOffset);

	const size_t prevFaceCount = faces.size() / 3;
	faces.resize(faces.size() + 3 * faceCount);
	const size_t totalFaceCount = prevFaceCount + faceCount;

	for (size_t f = prevFaceCount; f < totalFaceCount; ++f) {
		faces[f * 3 + 0] = prevVertexCount + stream.readUint32LE();
		faces[f * 3 + 1] = prevVertexCount + stream.readUint32LE();
		faces[f * 3 + 2] = prevVertexCount + stream.readUint32LE();
	}
}

void WalkmeshLoader::appendAdjFaces(Common::SeekableReadStream &stream,
                                    std::vector<uint32_t> &adjFaces,
                                    size_t prevFaceCount,
                                    uint32_t faceAdjCount,
                                    uint32_t faceAdjOffset) {
	stream.seek(faceAdjOffset);

	for (uint32_t a = 0; a < faceAdjCount; ++a) {
		for (uint32_t i = 0; i < 3; ++i) {
			const uint32_t edge = stream.readSint32LE();
			// Map edge to face.
			if (edge < UINT32_MAX)
				adjFaces[_walkableFaces[a] * 3 + i] = (edge + (2 - edge % 3)) / 3 + prevFaceCount;
		}
	}
}

void WalkmeshLoader::multiply(const float *v, const glm::mat4 &m, float *rv) const {
	rv[0]   = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + m[3][0];
	rv[1]   = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + m[3][1];
	rv[2]   = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + m[3][2];
	float w = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + m[3][3];
	rv[0] /= w;
	rv[1] /= w;
	rv[2] /= w;
}

void WalkmeshLoader::appendVertices(Common::SeekableReadStream &stream,
                                    std::vector<float> &vertices,
                                    uint32_t vertexCount,
                                    uint32_t vertexOffset,
                                    const glm::mat4 &transform) {
	stream.seek(vertexOffset);

	const size_t prevVertexCount = vertices.size() / 3;
	const size_t totalVertexCount = prevVertexCount + vertexCount;
	vertices.resize(vertices.size() + 3 * vertexCount);

	for (uint32_t i = prevVertexCount; i < totalVertexCount; ++i) {
		float v[3];
		v[0] = stream.readIEEEFloatLE();
		v[1] = stream.readIEEEFloatLE();
		v[2] = stream.readIEEEFloatLE();

		float rv[3];
		multiply(v, transform, rv);

		vertices[i * 3 + 0] = rv[0];
		vertices[i * 3 + 1] = rv[1];
		vertices[i * 3 + 2] = rv[2];
	}
}

void WalkmeshLoader::appendPerimEdges(Common::SeekableReadStream &stream,
                                      std::map<uint32_t, uint32_t> &adjRooms,
                                      uint32_t perimEdgeCount, uint32_t perimEdgeOffset) {
	stream.seek(perimEdgeOffset);
	for (uint32_t pe = 0; pe < perimEdgeCount; ++pe) {
		uint32_t perimetricEdge = stream.readUint32LE();
		adjRooms[perimetricEdge] = stream.readUint32LE();
	}
}

Common::AABBNode *WalkmeshLoader::getAABB(Common::SeekableReadStream &stream,
                                          uint32_t nodeOffset, uint32_t AABBsOffset,
                                          size_t prevFaceCount) {
	stream.seek(nodeOffset);

	float min[3], max[3];
	for (uint8_t m = 0; m < 3; ++m)
		min[m] = stream.readIEEEFloatLE();
	for (uint8_t m = 0; m < 3; ++m)
		max[m] = stream.readIEEEFloatLE();

	const int32_t relatedFace = stream.readSint32LE();
	stream.skip(4); // Unknown
	stream.readUint32LE(); // Plane
	const uint32_t leftOffset = stream.readUint32LE();
	const uint32_t rightOffset = stream.readUint32LE();

	// Children always come as pair.
	if (relatedFace >= 0)
		return new Common::AABBNode(min, max, relatedFace + prevFaceCount);

	// 44 is the size of an AABBNode.
	const uint32_t AABBNodeSize = 44;
	Common::AABBNode *leftNode = getAABB(stream, leftOffset * AABBNodeSize + AABBsOffset,
	                                     AABBsOffset, prevFaceCount);
	Common::AABBNode *rightNode = getAABB(stream, rightOffset * AABBNodeSize + AABBsOffset,
	                                      AABBsOffset, prevFaceCount);
	Common::AABBNode *aabb = new Common::AABBNode(min, max);
	aabb->setChildren(leftNode, rightNode);

	return aabb;
}

} // End of namespace KotORBase

} // End of namespace Engines
