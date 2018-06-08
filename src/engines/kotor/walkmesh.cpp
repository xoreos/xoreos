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
 *  KotOR walkmesh (BWM).
 */

#include "src/common/scopedptr.h"

#include "src/aurora/resman.h"

#include "src/engines/kotor/walkmesh.h"

namespace Engines {

namespace KotOR {

void Walkmesh::load(const Common::UString &resRef, ::Aurora::FileType type, const glm::mat4 &transform) {
	_vertices.clear();
	_indices.clear();
	_faceWalkableMap.clear();

	Common::ScopedPtr<Common::SeekableReadStream> bwm(ResMan.getResource(resRef, type));

	if (!bwm) {
		warning("Walkmesh file not found: %s", resRef.c_str());
		return;
	}

	try {
		appendFromStream(*bwm, transform);
		refreshIndexGroups();
	} catch (Common::Exception &e) {
		warning("Walkmesh load failed: %s %s", resRef.c_str(), e.what());
	}
}

void Walkmesh::appendFromStream(Common::SeekableReadStream &stream,
                                const glm::mat4 &transform) {
	stream.seek(0);

	/** Header format:
	 *
	 *  uint8[8]  - BWM version
	 *  uint32    - walkmesh type
	 *  uint8[48] - reserved
	 *  float[3]  - position
	 */

	if (stream.readUint32BE() != MKTAG('B', 'W', 'M', ' ') ||
			stream.readUint32BE() != MKTAG('V', '1', '.', '0'))
		throw Common::Exception("Invalid BWM version");

	stream.skip(64);

	uint32 vertexCount = stream.readUint32LE();
	uint32 vertexOffset = stream.readUint32LE();
	uint32 faceCount = stream.readUint32LE();
	uint32 faceOffset = stream.readUint32LE();
	uint32 faceTypeOffset = stream.readUint32LE();

	//-

	appendFaceTypes(stream, faceCount, faceTypeOffset);
	appendIndices(stream, faceCount, faceOffset);
	appendVertices(stream, vertexCount, vertexOffset, transform);
}

void Walkmesh::appendFaceTypes(Common::SeekableReadStream &stream,
                               uint32 faceCount,
                               uint32 faceTypeOffset) {
	stream.seek(faceTypeOffset);

	size_t prevFaceCount = _faceWalkableMap.size();
	_faceWalkableMap.resize(prevFaceCount + faceCount);
	uint32 index = prevFaceCount;

	for (uint32 i = 0; i < faceCount; ++i) {
		uint32 faceType = stream.readUint32LE();
		switch (faceType) {
			case 0:
			case 2:
			case 7:
			case 8:
				_faceWalkableMap[index++] = false;
				break;
			default:
				_faceWalkableMap[index++] = true;
				break;
		}
	}
}

void Walkmesh::appendIndices(Common::SeekableReadStream &stream,
                             uint32 faceCount,
                             uint32 faceOffset) {
	stream.seek(faceOffset);

	size_t prevIndexCount = _indices.size();
	uint32 indexCount = 3 * faceCount;
	_indices.resize(prevIndexCount + indexCount);
	uint32 index = prevIndexCount;
	size_t prevVertexCount = _vertices.size() / 3;

	for (uint32 i = 0; i < indexCount; ++i) {
		_indices[index++] = prevVertexCount + stream.readUint32LE();
	}
}

static void multiply(const float *v, const glm::mat4 &m, float *rv) {
	rv[0]   = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + m[3][0];
	rv[1]   = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + m[3][1];
	rv[2]   = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + m[3][2];
	float w = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + m[3][3];
	rv[0] /= w;
	rv[1] /= w;
	rv[2] /= w;
}

void Walkmesh::appendVertices(Common::SeekableReadStream &stream,
                              uint32 vertexCount,
                              uint32 vertexOffset,
                              const glm::mat4 &transform) {
	stream.seek(vertexOffset);

	size_t prevVerticesSize = _vertices.size();
	uint32 verticesSize = 3 * vertexCount;
	_vertices.resize(prevVerticesSize + verticesSize);
	uint32 index = prevVerticesSize;

	for (uint32 i = 0; i < vertexCount; ++i) {
		float v[3];
		v[0] = stream.readIEEEFloatLE();
		v[1] = stream.readIEEEFloatLE();
		v[2] = stream.readIEEEFloatLE();

		float rv[3];
		multiply(v, transform, rv);

		_vertices[index + 0] = rv[0];
		_vertices[index + 1] = rv[1];
		_vertices[index + 2] = rv[2];

		index += 3;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
