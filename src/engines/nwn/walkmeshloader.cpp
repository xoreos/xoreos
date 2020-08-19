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
 *  Class that reads a WalkmeshLoader file.
 */

#include <memory>

#include "external/glm/mat4x4.hpp"
#include "external/glm/gtc/matrix_transform.hpp"

#include "src/common/ustring.h"
#include "src/common/streamtokenizer.h"
#include "src/common/strutil.h"
#include "src/common/aabbnode.h"
#include "src/common/maths.h"

#include "src/aurora/resman.h"

#include "src/engines/nwn/walkmeshloader.h"

namespace Engines {

namespace NWN {

WalkmeshLoader::WalkmeshLoader() : _node(0) {
}

WalkmeshLoader::~WalkmeshLoader() {
}


void WalkmeshLoader::load(::Aurora::FileType fileType, const Common::UString &name,
                          float orientation[4], float position[3],
                          std::vector<float> &vertices, std::vector<uint32_t> &faces,
                          std::vector<uint32_t> &facesProperty, const Common::UString &filterNode) {
	std::unique_ptr<Common::SeekableReadStream> stream(ResMan.getResource(name, fileType));
	if (!stream)
		return;

	stream->seek(0);

	std::unique_ptr<Common::StreamTokenizer> tokenize = std::make_unique<Common::StreamTokenizer>(Common::StreamTokenizer::kRuleIgnoreAll);
	tokenize->addSeparator(' ');
	tokenize->addChunkEnd('\n');
	tokenize->addIgnore('\r');

	if (fileType == Aurora::kFileTypePWK)
		orientation[3] = Common::deg2rad(orientation[3]);

	uint8_t axisAlignedOrientation = static_cast<uint8_t>(orientation[3] / M_PI * 2);
	glm::mat4 rotation(1.f);
	rotation = glm::rotate(rotation,
	                       orientation[3],
	                       glm::vec3(orientation[0], orientation[1], orientation[2]));

	_sameVertex.clear();
	bool axisRotation = true;
	float localPosition[3] = { 0.0f, 0.0f, 0.0f };
	uint32_t startVertex = vertices.size() / 3;
	Common::UString currentNode = "";
	bool goodNode = filterNode.empty();

	while (!stream->eos()) {
		std::vector<Common::UString> line;
		size_t count = tokenize->getTokens(*stream, line, 3);
		tokenize->nextChunk(*stream);
		// Ignore empty lines and comments.
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].makeLower();

		if (line[0] == "node" && !filterNode.empty()) {
			currentNode = line[2];
			goodNode = currentNode.contains(filterNode);
		} else if (!goodNode) {
			continue;
		} else if (line[0] == "position") {
			readFloats(line, localPosition, 3, 1);
		} else if (line[0] == "orientation") {
			float ori[4];
			readFloats(line, ori, 4, 1);
			if (std::abs(ori[3]) > 0.01) {
				rotation = glm::rotate(rotation, ori[3],
				                       glm::vec3(ori[0], ori[1], ori[2]));
			}

			// Check if the orientation is not axis aligned, a factor of PI/2. The use of M_PI_2
			// with fmod lead to an wrong result hence the hardcoded PI/2: 1.57.
			if (fmod(fabs(ori[3]), 1.57f) > 0.05) {
				axisRotation = false;
			}

			axisAlignedOrientation += (int8_t) roundf(ori[2] * ori[3] / (3.14 / 2));
			axisAlignedOrientation %= 4;

			// Update position and orientation.
			glm::vec4 shift = rotation * glm::vec4(localPosition[0], localPosition[1], localPosition[2], 1.f);

			for (uint8_t i = 0; i < 3; ++i)
				position[i] += shift[i];

		} else if (line[0] == "verts") {
			size_t vertsCount;
			Common::parseString(line[1], vertsCount);
			readVerts(vertsCount, position, stream.get(), tokenize.get(), rotation, vertices);
		} else if (line[0] == "faces") {
			size_t facesCount;
			Common::parseString(line[1], facesCount);
			readFaces(facesCount, stream.get(), tokenize.get(), faces, facesProperty, startVertex);
		} else if (line[0] == "aabb") {
			float rawMin[3], rawMax[3], min[3], max[3];
			readFloats(line, rawMin, 3, 1);
			readFloats(line, rawMax, 3, 4);

			changeOrientation(axisAlignedOrientation, rawMin);
			changeOrientation(axisAlignedOrientation, rawMax);
			for (uint32_t i = 0; i < 3; ++i) {
				rawMin[i] += position[i];
				rawMax[i] += position[i];
				min[i] = MIN(rawMin[i], rawMax[i]);
				max[i] = MAX(rawMin[i], rawMax[i]);
			}

			Common::AABBNode *rootNode = 0;

			if (axisRotation) {
				rootNode = new Common::AABBNode(min, max);

				Common::AABBNode *leftChild = readAABB(position, axisAlignedOrientation, stream.get(),
													   tokenize.get());
				Common::AABBNode *rightChild = readAABB(position, axisAlignedOrientation, stream.get(),
														tokenize.get());
				rootNode->setChildren(leftChild, rightChild);
			} else {
				rootNode = buildAABBTree(vertices, faces);
			}

			_node = rootNode;
		} else if (line[0] == "endnode") {
			if (filterNode.empty())
				break;

			continue;
		}
	}
}

Common::AABBNode *WalkmeshLoader::getAABB() {
	return _node;
}

void WalkmeshLoader::readFloats(const std::vector<Common::UString> &strings,
                                float *floats, uint32_t n, uint32_t start) {

	if (strings.size() < (start + n))
		throw Common::Exception("Missing tokens");

	for (uint32_t i = 0; i < n; i++)
		Common::parseString(strings[start + i], floats[i]);
}

void WalkmeshLoader::readVerts(size_t n, float *position, Common::SeekableReadStream *stream,
                               Common::StreamTokenizer *tokenize, glm::mat4x4 &rotation,
                               std::vector<float> &vertices) {

	size_t startVertex = vertices.size() / 3;
	vertices.resize(vertices.size() + n * 3);

	for (size_t i = startVertex; i < startVertex + n; ++i) {
		std::vector<Common::UString> line;
		size_t count = tokenize->getTokens(*stream, line, 3);
		tokenize->nextChunk(*stream);

		// Ignore empty lines and comments.
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#')) {
			--i;
			continue;
		}

		for (uint32_t vi = 0; vi < 3; ++vi) {
			float val;
			Common::parseString(line[vi], val);
			vertices[3 * i + vi] = val;
		}

		// Adjust vertex.
		glm::vec4 vert = rotation * glm::vec4(vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2], 1.f);
		for (uint32_t vi = 0; vi < 3; ++vi)
			vertices[3 * i + vi] = vert[vi] + position[vi];

		// Some vertices are exactly the same, remove redundancy.
		for (size_t o = startVertex; o < i; ++o) {
			if (vertices[3 * o] == vertices[3 * i]
			    && vertices[3 * o + 1] == vertices[3 * i + 1]
			    && vertices[3 * o + 2] == vertices[3 * i + 2]) {
				// The vertices are the same.
				_sameVertex[i] = o;
				break;
			}
		}
	}
}

void WalkmeshLoader::readFaces(size_t n, Common::SeekableReadStream *stream,
                               Common::StreamTokenizer *tokenize,
                               std::vector<uint32_t> &faces, std::vector<uint32_t> &facesProperty,
                               uint32_t startVertex) {
	uint32_t startFace = faces.size() / 3;
	faces.resize((startFace + n) * 3);
	facesProperty.resize(startFace + n);

	for (size_t i = startFace; i < startFace + n; ++i) {
		std::vector<Common::UString> line;
		size_t count = tokenize->getTokens(*stream, line, 3);
		tokenize->nextChunk(*stream);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#')) {
			--i;
			continue;
		}

		for (uint32_t vi = 0; vi < 3; ++vi) {
			uint32_t val;
			Common::parseString(line[vi], val);
			if (_sameVertex.find(val + startVertex) != _sameVertex.end()) {
				// Replace redundant vertex.
				faces[3 * i + vi] = _sameVertex[val + startVertex];
			} else {
				faces[3 * i + vi] = val + startVertex;
			}
		}

		// Surface type
		Common::parseString(line[7], facesProperty[i]);
	}
}

void WalkmeshLoader::changeOrientation(uint8_t orientation, float *position) const {
	for (uint8_t o = 0; o < orientation; ++o) {
		float temp = position[0];
		position[0] = - position[1];
		position[1] = temp;
	}
}

Common::AABBNode *WalkmeshLoader::readAABB(float *position, uint8_t orientation,
                                           Common::SeekableReadStream *stream,
                                           Common::StreamTokenizer *tokenize) {
	std::vector<Common::UString> line;
	tokenize->getTokens(*stream, line, 7);
	tokenize->nextChunk(*stream);

	float rawMin[3], rawMax[3], min[3], max[3];
	readFloats(line, rawMin, 3, 0);
	readFloats(line, rawMax, 3, 3);

	changeOrientation(orientation, rawMin);
	changeOrientation(orientation, rawMax);
	for (uint8_t i = 0; i < 3; ++i) {
		rawMin[i] += position[i];
		rawMax[i] += position[i];
		min[i] = MIN(rawMin[i], rawMax[i]);
		max[i] = MAX(rawMin[i], rawMax[i]);
	}

	// If it's a child node, record the related face.
	int32_t face;
	Common::parseString(line[6], face);

	Common::AABBNode *node = new Common::AABBNode(min, max, face);
	if (face < 0) {
		Common::AABBNode *leftChild = readAABB(position, orientation, stream, tokenize);
		Common::AABBNode *rightChild = readAABB(position, orientation, stream, tokenize);
		node->setChildren(leftChild, rightChild);
	}

	return node;
}

Common::AABBNode *WalkmeshLoader::buildAABBTree(std::vector<float> &vertices,
                                                std::vector<uint32_t> &faces) {
	// No face, no tree.
	if (faces.empty())
		return 0;

	std::list<Common::AABBNode *> nodes;

	uint32_t faceCount = faces.size() / 3;
	Common::AABBNode *AABBRoot = createAABB(0, vertices, faces);
	if (faceCount < 2)
		return AABBRoot;

	nodes.push_back(AABBRoot);

	for (uint32_t f = 1; f < faceCount; ++f) {
		// First, create a new AABB from the face.
		Common::AABBNode *AABBFace = createAABB(f, vertices, faces);
		// Find the best match to pair with it.
		Common::AABBNode *bestNode = 0;
		float compatibility = FLT_MAX;
		float min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
		float max[3] = {FLT_MIN, FLT_MIN, FLT_MIN};
		// Use a metric (the total area) to find the best match.
		for (std::list<Common::AABBNode *>::iterator n = nodes.begin(); n != nodes.end(); ++n) {
			float compatTest = AABBCompatibility(AABBFace, *n, min, max);
			// We are looking for a close AABB.
			if (compatTest >= compatibility)
				continue;

			bestNode = *n;
			compatibility = compatTest;
		}

		if (!bestNode) {
			delete AABBFace;
			continue;
		}

		// Try to flatten the tree by pushing to the back the best node so at a following
		// iteration it will not be choosen if another node has the same compatibility.
		nodes.remove(bestNode);
		nodes.push_back(bestNode);
		// Create a new node that will replace the best node and will have the new node and the best
		// node as children.
		Common::AABBNode *newParent = new Common::AABBNode(min, max);
		// Check if the new parent is the root node (has no parent).
		if (!bestNode->hasParent()) {
			AABBRoot = newParent;
		}
		bestNode->replaceBranch(newParent);
		newParent->setChildren(bestNode, AABBFace);
		newParent->surroundParent();

		nodes.push_back(AABBFace);
		nodes.push_back(newParent);
	}
	return AABBRoot;
}

Common::AABBNode *WalkmeshLoader::createAABB(uint32_t face, std::vector<float> &vertices,
                                             std::vector<uint32_t> &faces) const {
	float max[3] = {FLT_MIN, FLT_MIN, FLT_MIN};
	float min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};

	for (uint32_t v = 0; v < 3; ++v) {
		for (uint32_t i = 0; i < 3; ++i) {
			max[i] = MAX(vertices[faces[face * 3 + v] * 3 + i], max[i]);
			min[i] = MIN(vertices[faces[face * 3 + v] * 3 + i], min[i]);
		}
	}

	return new Common::AABBNode(min, max, face);
}

float WalkmeshLoader::AABBCompatibility(Common::AABBNode *nodeA, Common::AABBNode *nodeB,
                                        float min[3], float max[3]) const {
	// Compute the area of the joined AABBs.
	float area = 1.f;
	float minA[3], maxA[3];
	nodeA->getMin(minA[0], minA[1], minA[2]);
	nodeB->getMin(min[0], min[1], min[2]);
	nodeA->getMax(maxA[0], maxA[1], maxA[2]);
	nodeB->getMax(max[0], max[1], max[2]);

	for (uint32_t i = 0; i < 3; ++i) {
		min[i] = MIN(minA[i], min[i]);
		max[i] = MAX(maxA[i], max[i]);
		// Set a cutoff.
		if (max[i] - min[i] < 0.05) {
			area *= 0.05;
		} else {
			area *= max[i] - min[i];
		}
	}
	return area;
}

} // End of namespace NWN

} // End of namespace Engines
