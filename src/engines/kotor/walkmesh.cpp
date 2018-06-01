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
 *  KotOR walkmesh (.wok) file loader. Ported from Supermanu's pathfinding
 *  branch.
 */

#include "src/common/scopedptr.h"

#include "src/aurora/resman.h"

#include "src/engines/aurora/walkeleveval.h"

#include "src/engines/kotor/walkmesh.h"

namespace Engines {

namespace KotOR {

Walkmesh::Walkmesh()
		: Renderable(Graphics::kRenderableTypeObject),
		  _highlightFaceIndex(-1),
		  _invisible(true) {
}

void Walkmesh::load(const Common::UString &resRef) {
	clear();
	Common::ScopedPtr<Common::SeekableReadStream> wok(ResMan.getResource(resRef, ::Aurora::kFileTypeWOK));

	if (wok) {
		try {
			appendFromStream(*wok);
		} catch (Common::Exception &e) {
			warning("Walkmesh load failed: %s %s", resRef.c_str(), e.what());
		}
	} else
		warning("Walkmesh file not found: %s", resRef.c_str());

	refreshIndexGroups();
}

float Walkmesh::getElevationAt(float x, float y, uint32 &faceIndex) const {
	return WalkmeshElevationEvaluator::getElevationAt(*this, x, y, faceIndex);
}

void Walkmesh::highlightFace(uint32 index) {
	_highlightFaceIndex = index;
}

void Walkmesh::setInvisible(bool invisible) {
	_invisible = invisible;
}

void Walkmesh::appendFromStream(Common::SeekableReadStream &stream) {
	stream.seek(0);

	/** Header format:
	 *
	 *  uint8[8]  - WOK version
	 *  uint32    - walkmesh type
	 *  uint8[48] - reserved
	 *  float[3]  - position
	 */

	if (stream.readUint32BE() != MKTAG('B', 'W', 'M', ' ') ||
			stream.readUint32BE() != MKTAG('V', '1', '.', '0'))
		throw Common::Exception("Invalid WOK version");

	stream.skip(64);

	uint32 vertexCount = stream.readUint32LE();
	uint32 vertexOffset = stream.readUint32LE();
	uint32 faceCount = stream.readUint32LE();
	uint32 faceOffset = stream.readUint32LE();
	uint32 faceTypeOffset = stream.readUint32LE();

	//-

	appendFaceTypes(stream, faceCount, faceTypeOffset);
	appendIndices(stream, faceCount, faceOffset);
	appendVertices(stream, vertexCount, vertexOffset);
}

void Walkmesh::appendFaceTypes(Common::SeekableReadStream &stream, uint32 faceCount, uint32 faceTypeOffset) {
	stream.seek(faceTypeOffset);

	size_t prevFaceCount = _data.faceWalkableMap.size();
	_data.faceWalkableMap.resize(prevFaceCount + faceCount);
	uint32 index = prevFaceCount;

	for (uint32 i = 0; i < faceCount; ++i) {
		uint32 faceType = stream.readUint32LE();
		switch (faceType) {
			case 0:
			case 2:
			case 7:
			case 8:
				_data.faceWalkableMap[index++] = false;
				break;
			default:
				_data.faceWalkableMap[index++] = true;
				break;
		}
	}
}

void Walkmesh::appendIndices(Common::SeekableReadStream &stream, uint32 faceCount, uint32 faceOffset) {
	stream.seek(faceOffset);

	size_t prevIndexCount = _data.indices.size();
	uint32 indexCount = 3 * faceCount;
	_data.indices.resize(prevIndexCount + indexCount);
	uint32 index = prevIndexCount;
	size_t prevVertexCount = _data.vertices.size() / 3;

	for (uint32 i = 0; i < indexCount; ++i) {
		_data.indices[index++] = prevVertexCount + stream.readUint32LE();
	}
}

void Walkmesh::appendVertices(Common::SeekableReadStream &stream, uint32 vertexCount, uint32 vertexOffset) {
	stream.seek(vertexOffset);

	size_t prevVerticesSize = _data.vertices.size();
	uint32 verticesSize = 3 * vertexCount;
	_data.vertices.resize(prevVerticesSize + verticesSize);
	uint32 index = prevVerticesSize;

	for (uint32 i = 0; i < verticesSize; ++i) {
		_data.vertices[index++] = stream.readIEEEFloatLE();
	}
}

void Walkmesh::calculateDistance() {
}

void Walkmesh::render(Graphics::RenderPass pass) {
	if (pass == Graphics::kRenderPassTransparent && !_invisible) {
		glVertexPointer(3, GL_FLOAT, 0, _data.vertices.data());
		glEnableClientState(GL_VERTEX_ARRAY);

		glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
		glDrawElements(GL_TRIANGLES, _indicesNonWalkable.size(), GL_UNSIGNED_INT, _indicesNonWalkable.data());

		glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		glDrawElements(GL_TRIANGLES, _indicesWalkable.size(), GL_UNSIGNED_INT, _indicesWalkable.data());

		glDisableClientState(GL_VERTEX_ARRAY);

		// Render highlighted face
		if (_highlightFaceIndex > -1) {
			glColor4f(0.0f, 1.0f, 1.0f, 0.5f);
			glBegin(GL_TRIANGLES);
				uint32 index = 3 * _highlightFaceIndex;
				glVertex3fv(&_data.vertices[3 * _indicesWalkable[index + 0]]);
				glVertex3fv(&_data.vertices[3 * _indicesWalkable[index + 1]]);
				glVertex3fv(&_data.vertices[3 * _indicesWalkable[index + 2]]);
			glEnd();
		}

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

} // End of namespace KotOR

} // End of namespace Engines
