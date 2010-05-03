/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/model_nwn.cpp
 *  Loading MDL files found in Neverwinter Nights.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"
#include "common/streamtokenizer.h"

#include "events/requests.h"

#include "graphics/model_nwn.h"
#include "graphics/texture.h"

static const int kNodeFlagHasHeader    = 0x00000001;
static const int kNodeFlagHasLight     = 0x00000002;
static const int kNodeFlagHasEmitter   = 0x00000004;
static const int kNodeFlagHasReference = 0x00000010;
static const int kNodeFlagHasMesh      = 0x00000020;
static const int kNodeFlagHasSkin      = 0x00000040;
static const int kNodeFlagHasAnim      = 0x00000080;
static const int kNodeFlagHasDangly    = 0x00000100;
static const int kNodeFlagHasAABB      = 0x00000200;

namespace Graphics {

Model_NWN::Model_NWN(Common::SeekableReadStream &mdl) {
	_tokenizeASCII = new Common::StreamTokenizer(Common::StreamTokenizer::kRuleIgnoreAll);

	_tokenizeASCII->addSeparator(' ');
	_tokenizeASCII->addChunkEnd('\n');
	_tokenizeASCII->addIgnore('\r');

	load(mdl);

	RequestMan.sync();
}

Model_NWN::~Model_NWN() {
	delete _tokenizeASCII;
}

void Model_NWN::load(Common::SeekableReadStream &mdl) {
	if (mdl.readUint32LE() == 0)
		loadBinary(mdl);
	else
		loadASCII(mdl);
}

void Model_NWN::loadASCII(Common::SeekableReadStream &mdl) {
	mdl.seek(0);

	while (!mdl.eos() && !mdl.err()) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(mdl, line);

		_tokenizeASCII->nextChunk(mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].tolower();

		if        (line[0] == "newmodel") {
			_name = line[1];
		} else if (line[0] == "setsupermodel") {
			if (line[1] != _name)
				throw Common::Exception("setsupermodel with an invalid name");

			if (line[2] != "NULL")
				warning("TODO: setsupermodel");

			_superModel = 0;
		} else if (line[0] == "classification") {
			_class = parseClassification(line[1]);
		} else if (line[0] == "setanimationscale") {
			line[1].parse(_scale);
		} else if (line[0] == "beginmodelgeom") {
			if (line[1] != _name)
				throw Common::Exception("beginmodelgeom with an invalid name");
		} else if (line[0] == "node") {
			parseNodeASCII(mdl, line[1], line[2]);
		} else if (line[0] == "newanim") {
			parseAnimASCII(mdl);
		} else if (line[0] == "filedependancy") {
		} else if (line[0] == "endmodelgeom") {
		} else if (line[0] == "donemodel") {
			break;
		} else
			throw Common::Exception("Unknown MDL command \"%s\"", line[0].c_str());
	}
}

void Model_NWN::parseNodeASCII(Common::SeekableReadStream &mdl,
		const Common::UString &type, const Common::UString &name) {

	bool end = false;

	bool skipNode = false;

	if ((type == "emitter") || (type == "reference")) {
		warning("TODO: Node type %s", type.c_str());
		skipNode = true;
	}

	Node *node = new Node;

	if (type == "danglymesh")
		node->dangly = true;

	std::vector<float> vertices;
	std::vector<float> verticesTexture;
	std::vector<FaceASCII> faces;

	try {
		while (!mdl.eos() && !mdl.err()) {
			std::vector<Common::UString> line;

			int count = _tokenizeASCII->getTokens(mdl, line);

			_tokenizeASCII->nextChunk(mdl);

			// Ignore empty lines and comments
			if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
				continue;

			line[0].tolower();

			if        (line[0] == "endnode") {
				end = true;
				break;
			} else if (skipNode) {
				continue;
			} else if (line[0] == "parent") {
				if (line[1] != "NULL") {
					NodeMap::iterator it = _nodes.find(line[1]);
					if (it == _nodes.end())
						throw Common::Exception("Non-existent parent node");

					node->parent = it->second;
				} else
					node->parent = 0;
			} else if (line[0] == "position") {
				parseFloats(line, node->position, 3, 1);
			} else if (line[0] == "orientation") {
				parseFloats(line, node->orientation, 4, 1);
			} else if (line[0] == "wirecolor") {
				parseFloats(line, node->wirecolor, 3, 1);
			} else if (line[0] == "ambient") {
				parseFloats(line, node->ambient, 3, 1);
			} else if (line[0] == "diffuse") {
				parseFloats(line, node->diffuse, 3, 1);
			} else if (line[0] == "specular") {
				parseFloats(line, node->specular, 3, 1);
			} else if (line[0] == "shininess") {
				parseFloats(line, &node->shininess, 1, 1);
			} else if (line[0] == "period") {
				parseFloats(line, &node->period, 1, 1);
			} else if (line[0] == "tightness") {
				parseFloats(line, &node->tightness, 1, 1);
			} else if (line[0] == "displacement") {
				parseFloats(line, &node->displacement, 1, 1);
			} else if (line[0] == "showdispl") {
				line[1].parse(node->showdispl);
			} else if (line[0] == "displtype") {
				line[1].parse(node->displtype);
			} else if (line[0] == "center") {
				if (line[1] == "undefined")
					warning("TODO: center == undefined");
				else
					parseFloats(line, node->center, 3, 1);
			} else if (line[0] == "tilefade") {
				line[1].parse(node->displtype);
			} else if (line[0] == "scale") {
				line[1].parse(node->displtype);
			} else if (line[0] == "render") {
				line[1].parse(node->displtype);
			} else if (line[0] == "shadow") {
				line[1].parse(node->displtype);
			} else if (line[0] == "beaming") {
				line[1].parse(node->displtype);
			} else if (line[0] == "inheritcolor") {
				line[1].parse(node->displtype);
			} else if (line[0] == "rotatetexture") {
				line[1].parse(node->displtype);
			} else if (line[0] == "alpha") {
				line[1].parse(node->displtype);
			} else if (line[0] == "transparencyhint") {
				line[1].parse(node->displtype);
			} else if (line[0] == "selfillumcolor") {
				parseFloats(line, node->selfillumcolor, 3, 1);
			} else if (line[0] == "danglymesh") {
				line[1].parse(node->dangly);
			} else if (line[0] == "gizmo") {
				warning("TODO: gizmo \"%s\"", line[1].c_str());
			} else if (line[0] == "constraints") {
				int n;

				line[1].parse(n);
				parseConstraintsASCII(mdl, node->constraints, n);
			} else if (line[0] == "weights") {
				warning("TODO: Weights");

				int n;

				line[1].parse(n);
				parseWeightsASCII(mdl, n);
			} else if (line[0] == "bitmap") {
				node->bitmap = line[1];
			} else if (line[0] == "verts") {
				int n;

				line[1].parse(n);
				parseVerticesASCII(mdl, vertices, n);
			} else if (line[0] == "tverts") {
				int n;

				line[1].parse(n);
				parseVerticesASCII(mdl, verticesTexture, n);
			} else if (line[0] == "faces") {
				int n;

				line[1].parse(n);
				parseFacesASCII(mdl, faces, n);
			} else
				throw Common::Exception("Unknown MDL node command \"%s\"", line[0].c_str());
		}

		if (!end)
			throw Common::Exception("node without endnode");

	} catch (...) {
		delete node;
		throw;
	}

	node->faces.resize(faces.size());
	for (uint i = 0; i < faces.size(); i++) {
		node->faces[i].vertices[0][0] = vertices[3 * faces[i].vertices[0] + 0];
		node->faces[i].vertices[0][1] = vertices[3 * faces[i].vertices[0] + 1];
		node->faces[i].vertices[0][2] = vertices[3 * faces[i].vertices[0] + 2];
		node->faces[i].vertices[1][0] = vertices[3 * faces[i].vertices[1] + 0];
		node->faces[i].vertices[1][1] = vertices[3 * faces[i].vertices[1] + 1];
		node->faces[i].vertices[1][2] = vertices[3 * faces[i].vertices[1] + 2];
		node->faces[i].vertices[2][0] = vertices[3 * faces[i].vertices[2] + 0];
		node->faces[i].vertices[2][1] = vertices[3 * faces[i].vertices[2] + 1];
		node->faces[i].vertices[2][2] = vertices[3 * faces[i].vertices[2] + 2];

		if (faces[i].verticesTexture[0] >= (verticesTexture.size() * 3)) {
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++)
					node->faces[i].verticesTexture[j][k] = 0;
		} else {
			node->faces[i].verticesTexture[0][0] = verticesTexture[3 * faces[i].verticesTexture[0] + 0];
			node->faces[i].verticesTexture[0][1] = verticesTexture[3 * faces[i].verticesTexture[0] + 1];
			node->faces[i].verticesTexture[0][2] = verticesTexture[3 * faces[i].verticesTexture[0] + 2];
			node->faces[i].verticesTexture[1][0] = verticesTexture[3 * faces[i].verticesTexture[1] + 0];
			node->faces[i].verticesTexture[1][1] = verticesTexture[3 * faces[i].verticesTexture[1] + 1];
			node->faces[i].verticesTexture[1][2] = verticesTexture[3 * faces[i].verticesTexture[1] + 2];
			node->faces[i].verticesTexture[2][0] = verticesTexture[3 * faces[i].verticesTexture[2] + 0];
			node->faces[i].verticesTexture[2][1] = verticesTexture[3 * faces[i].verticesTexture[2] + 1];
			node->faces[i].verticesTexture[2][2] = verticesTexture[3 * faces[i].verticesTexture[2] + 2];
		}

		node->faces[i].smoothGroup = faces[i].smoothGroup;
		node->faces[i].material    = faces[i].material;
	}

	if (!node->bitmap.empty() && (node->bitmap != "NULL"))
		node->texture = new Texture(node->bitmap);

	_nodes.insert(std::make_pair(name, node));
}

void Model_NWN::parseVerticesASCII(Common::SeekableReadStream &mdl, std::vector<float> &vertices, int n) {
	vertices.resize(3 * n);

	float *verts = &vertices[0];
	while (n > 0) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(mdl, line);

		_tokenizeASCII->nextChunk(mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		parseFloats(line, verts, 3, 0);

		n--;
		verts += 3;
	}
}

void Model_NWN::parseFacesASCII(Common::SeekableReadStream &mdl, std::vector<FaceASCII> &faces, int n) {
	faces.resize(n);

	for (int i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(mdl, line);

		_tokenizeASCII->nextChunk(mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		FaceASCII &face = faces[i++];

		line[0].parse(face.vertices[0]);
		line[1].parse(face.vertices[1]);
		line[2].parse(face.vertices[2]);

		line[3].parse(face.smoothGroup);

		line[4].parse(face.verticesTexture[0]);
		line[5].parse(face.verticesTexture[1]);
		line[6].parse(face.verticesTexture[2]);

		line[7].parse(face.material);
	}
}

void Model_NWN::parseConstraintsASCII(Common::SeekableReadStream &mdl,
		std::vector<float> &constraints, int n) {

	constraints.resize(n);

	for (int i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(mdl, line);

		_tokenizeASCII->nextChunk(mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].parse(constraints[i++]);
	}
}

void Model_NWN::parseWeightsASCII(Common::SeekableReadStream &mdl, int n) {
	for (int i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(mdl, line);

		_tokenizeASCII->nextChunk(mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		i++;
	}
}

void Model_NWN::parseAnimASCII(Common::SeekableReadStream &mdl) {
	bool end = false;

	try {
		while (!mdl.eos() && !mdl.err()) {
			std::vector<Common::UString> line;

			int count = _tokenizeASCII->getTokens(mdl, line);

			_tokenizeASCII->nextChunk(mdl);

			// Ignore empty lines and comments
			if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
				continue;

			line[0].tolower();

			if        (line[0] == "doneanim") {
				end = true;
				break;
			}
		}

		if (!end)
			throw Common::Exception("anim without doneanim");

	} catch (...) {
		throw;
	}

}

Model_NWN::Classification Model_NWN::parseClassification(Common::UString classification) {
	classification.tolower();

	if (classification == "effect")
		return kClassEffect;
	if (classification == "effects")
		return kClassEffect;
	if (classification == "tile")
		return kClassTile;
	if (classification == "character")
		return kClassCharacter;
	if (classification == "door")
		return kClassDoor;
	if (classification == "item")
		return kClassItem;
	if (classification == "gui")
		return kClassGUI;

	return kClassOther;
}

void Model_NWN::parseFloats(const std::vector<Common::UString> &strings, float *floats, int n, int start) {
	if (strings.size() < ((uint) (start + n)))
		throw Common::Exception("Missing tokens");

	for (int i = 0; i < n; i++)
		strings[start + i].parse(floats[i]);
}

void Model_NWN::loadBinary(Common::SeekableReadStream &mdl) {
	uint32 sizeModelData = mdl.readUint32LE();
	uint32 sizeRawData   = mdl.readUint32LE();

	mdl.skip(8); // Function pointers

	_name.readASCII(mdl, 64);

	uint32 nodeHeadPointer = mdl.readUint32LE();
	uint32 nodeCount       = mdl.readUint32LE();

	mdl.skip(24 + 4); // Unknown + Reference count

	uint8 type = mdl.readByte();

	mdl.skip(3 + 2); // Padding + Unknown

	uint8 classification = mdl.readByte();
	uint8 fogged         = mdl.readByte();

	mdl.skip(12); // TODO: Animation Header Pointer Array

	mdl.skip(4); // Parent model pointer

	float boundingMin[3], boundingMax[3];

	boundingMin[0] = mdl.readIEEEFloatLE();
	boundingMin[1] = mdl.readIEEEFloatLE();
	boundingMin[2] = mdl.readIEEEFloatLE();

	boundingMax[0] = mdl.readIEEEFloatLE();
	boundingMax[1] = mdl.readIEEEFloatLE();
	boundingMax[2] = mdl.readIEEEFloatLE();

	float radius = mdl.readIEEEFloatLE();

	_scale = mdl.readIEEEFloatLE();

	Common::UString superModelName;

	superModelName.readASCII(mdl, 64);

	warning("\"%s\", %d, %X, %d, \"%s\"", _name.c_str(), nodeCount, classification, fogged, superModelName.c_str());

	parseNodeBinary(mdl, nodeHeadPointer + 12);

	throw Common::Exception("TODO: Binary format");
}

void Model_NWN::parseNodeBinary(Common::SeekableReadStream &mdl, uint32 offset) {
	mdl.seekTo(offset);

	mdl.skip(24); // Function pointers

	uint32 inheritColorFlag = mdl.readUint32LE();
	uint32 partNumber       = mdl.readUint32LE();

	Common::UString name;

	name.readASCII(mdl, 32);

	mdl.skip(8); // Parent pointers

	uint32 childrenStart, childrenCount;
	readArray(mdl, childrenStart, childrenCount);

	std::vector<uint32> children;
	readOffsetArray(mdl, childrenStart + 12, childrenCount, children);

	mdl.skip(12); // Controller keys
	mdl.skip(12); // Controller data values

	uint32 flags = mdl.readUint32LE();

	if ((flags & 0xFFFFFC00) != 0)
		throw Common::Exception("Unknown node flags %08X", flags);

	if (flags & kNodeFlagHasLight) {
		// TODO: Light
		mdl.skip(0x5C);
	}

	if (flags & kNodeFlagHasEmitter) {
		// TODO: Emitter
		mdl.skip(0xD8);
	}

	if (flags & kNodeFlagHasReference) {
		// TODO: Reference
		mdl.skip(0x44);
	}

	if (flags & kNodeFlagHasMesh) {
		// TODO: Mesh
		mdl.skip(0x200);
	}

	if (flags & kNodeFlagHasSkin) {
		// TODO: Skin
		mdl.skip(0x64);
	}

	if (flags & kNodeFlagHasAnim) {
		// TODO: Anim
		mdl.skip(0x38);
	}

	if (flags & kNodeFlagHasDangly) {
		// TODO: Dangly
		mdl.skip(0x18);
	}

	if (flags & kNodeFlagHasAABB) {
		// TODO: AABB
		mdl.skip(0x4);
	}

	warning("%d: \"%s\", %X, %d", partNumber, name.c_str(), flags, childrenCount);

	for (std::vector<uint32>::const_iterator child = children.begin(); child != children.end(); ++child)
		parseNodeBinary(mdl, *child + 12);
}

void Model_NWN::readArray(Common::SeekableReadStream &mdl, uint32 &start, uint32 &count) {
	start = mdl.readUint32LE();

	uint32 usedCount      = mdl.readUint32LE();
	uint32 allocatedCount = mdl.readUint32LE();

	if (usedCount != allocatedCount)
		warning("Model_NWN::readArray(): usedCount != allocatedCount (%d, %d)", usedCount, allocatedCount);

	count = usedCount;
}

void Model_NWN::readOffsetArray(Common::SeekableReadStream &mdl, uint32 start, uint32 count,
		std::vector<uint32> &offsets) {

	uint32 pos = mdl.seekTo(start);

	offsets.reserve(count);
	while (count-- > 0)
		offsets.push_back(mdl.readUint32LE());

	mdl.seekTo(pos);
}

} // End of namespace Graphics
