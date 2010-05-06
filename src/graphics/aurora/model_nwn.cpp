/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model_nwn.cpp
 *  Loading MDL files found in Neverwinter Nights.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"
#include "common/streamtokenizer.h"

#include "events/requests.h"

#include "graphics/aurora/model_nwn.h"
#include "graphics/aurora/texture.h"

static const int kNodeFlagHasHeader    = 0x00000001;
static const int kNodeFlagHasLight     = 0x00000002;
static const int kNodeFlagHasEmitter   = 0x00000004;
static const int kNodeFlagHasReference = 0x00000010;
static const int kNodeFlagHasMesh      = 0x00000020;
static const int kNodeFlagHasSkin      = 0x00000040;
static const int kNodeFlagHasAnim      = 0x00000080;
static const int kNodeFlagHasDangly    = 0x00000100;
static const int kNodeFlagHasAABB      = 0x00000200;

static const int kControllerTypePosition             = 8;
static const int kControllerTypeOrientation          = 20;
static const int kControllerTypeScale                = 36;
static const int kControllerTypeColor                = 76;
static const int kControllerTypeRadius               = 88;
static const int kControllerTypeShadowRadius         = 96;
static const int kControllerTypeVerticalDisplacement = 100;
static const int kControllerTypeMultiplier           = 140;
static const int kControllerTypeAlphaEnd             = 80;
static const int kControllerTypeAlphaStart           = 84;
static const int kControllerTypeBirthRate            = 88;
static const int kControllerTypeBounce_Co            = 92;
static const int kControllerTypeColorEnd             = 96;
static const int kControllerTypeColorStart           = 108;
static const int kControllerTypeCombineTime          = 120;
static const int kControllerTypeDrag                 = 124;
static const int kControllerTypeFPS                  = 128;
static const int kControllerTypeFrameEnd             = 132;
static const int kControllerTypeFrameStart           = 136;
static const int kControllerTypeGrav                 = 140;
static const int kControllerTypeLifeExp              = 144;
static const int kControllerTypeMass                 = 148;
static const int kControllerTypeP2P_Bezier2          = 152;
static const int kControllerTypeP2P_Bezier3          = 156;
static const int kControllerTypeParticleRot          = 160;
static const int kControllerTypeRandVel              = 164;
static const int kControllerTypeSizeStart            = 168;
static const int kControllerTypeSizeEnd              = 172;
static const int kControllerTypeSizeStart_Y          = 176;
static const int kControllerTypeSizeEnd_Y            = 180;
static const int kControllerTypeSpread               = 184;
static const int kControllerTypeThreshold            = 188;
static const int kControllerTypeVelocity             = 192;
static const int kControllerTypeXSize                = 196;
static const int kControllerTypeYSize                = 200;
static const int kControllerTypeBlurLength           = 204;
static const int kControllerTypeLightningDelay       = 208;
static const int kControllerTypeLightningRadius      = 212;
static const int kControllerTypeLightningScale       = 216;
static const int kControllerTypeDetonate             = 228;
static const int kControllerTypeAlphaMid             = 464;
static const int kControllerTypeColorMid             = 468;
static const int kControllerTypePercentStart         = 480;
static const int kControllerTypePercentMid           = 481;
static const int kControllerTypePercentEnd           = 482;
static const int kControllerTypeSizeMid              = 484;
static const int kControllerTypeSizeMid_Y            = 488;
static const int kControllerTypeSelfIllumColor       = 100;
static const int kControllerTypeAlpha                = 128;

namespace Graphics {

namespace Aurora {

Model_NWN::ParserContext::ParserContext(Common::SeekableReadStream &stream) : mdl(&stream), node(0) {
}

Model_NWN::ParserContext::~ParserContext() {
	delete node;
}


Model_NWN::Model_NWN(Common::SeekableReadStream &mdl) {
	_tokenizeASCII = new Common::StreamTokenizer(Common::StreamTokenizer::kRuleIgnoreAll);

	_tokenizeASCII->addSeparator(' ');
	_tokenizeASCII->addChunkEnd('\n');
	_tokenizeASCII->addIgnore('\r');

	load(mdl);

	_nodeMap.clear();

	RequestMan.sync();
}

Model_NWN::~Model_NWN() {
	delete _tokenizeASCII;
}

void Model_NWN::load(Common::SeekableReadStream &mdl) {
	ParserContext ctx(mdl);

	if (ctx.mdl->readUint32LE() == 0)
		loadBinary(ctx);
	else
		loadASCII(ctx);
}

void Model_NWN::loadASCII(ParserContext &ctx) {
	ctx.mdl->seek(0);

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(*ctx.mdl, line);

		_tokenizeASCII->nextChunk(*ctx.mdl);

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
			parseNodeASCII(ctx, line[1], line[2]);
		} else if (line[0] == "newanim") {
			parseAnimASCII(ctx);
		} else if (line[0] == "filedependancy") {
		} else if (line[0] == "endmodelgeom") {
		} else if (line[0] == "donemodel") {
			break;
		} else
			throw Common::Exception("Unknown MDL command \"%s\"", line[0].c_str());
	}
}

void Model_NWN::parseNodeASCII(ParserContext &ctx, const Common::UString &type, const Common::UString &name) {

	bool end = false;

	bool skipNode = false;

	ctx.node = new Node;

	ctx.node->name = name;

	if ((type == "trimesh") || (type == "danglymesh") || (type == "skin"))
		ctx.node->render = true;
	else
		ctx.node->render = false;

	if ((type == "emitter") || (type == "reference")) {
		warning("TODO: Node type %s", type.c_str());
		skipNode = true;
	}

	if (type == "danglymesh")
		ctx.node->dangly = true;

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(*ctx.mdl, line);

		_tokenizeASCII->nextChunk(*ctx.mdl);

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
				NodeMap::iterator it = _nodeMap.find(line[1]);
				if (it == _nodeMap.end())
					throw Common::Exception("Non-existent parent node");

				ctx.node->parent = it->second;

				ctx.node->parent->children.push_back(ctx.node);
			} else {
				ctx.node->parent = 0;
				_rootNodes.push_back(ctx.node);
			}
		} else if (line[0] == "position") {
			parseFloats(line, ctx.node->position, 3, 1);
		} else if (line[0] == "orientation") {
			parseFloats(line, ctx.node->orientation, 4, 1);

			ctx.node->orientation[3] = Common::rad2deg(ctx.node->orientation[3]);
		} else if (line[0] == "wirecolor") {
			parseFloats(line, ctx.node->wirecolor, 3, 1);
		} else if (line[0] == "ambient") {
			parseFloats(line, ctx.node->ambient, 3, 1);
		} else if (line[0] == "diffuse") {
			parseFloats(line, ctx.node->diffuse, 3, 1);
		} else if (line[0] == "specular") {
			parseFloats(line, ctx.node->specular, 3, 1);
		} else if (line[0] == "shininess") {
			parseFloats(line, &ctx.node->shininess, 1, 1);
		} else if (line[0] == "period") {
			parseFloats(line, &ctx.node->period, 1, 1);
		} else if (line[0] == "tightness") {
			parseFloats(line, &ctx.node->tightness, 1, 1);
		} else if (line[0] == "displacement") {
			parseFloats(line, &ctx.node->displacement, 1, 1);
		} else if (line[0] == "showdispl") {
			line[1].parse(ctx.node->showdispl);
		} else if (line[0] == "displtype") {
			line[1].parse(ctx.node->displtype);
		} else if (line[0] == "center") {
			if (line[1] == "undefined")
				warning("TODO: center == undefined");
			else
				parseFloats(line, ctx.node->center, 3, 1);
		} else if (line[0] == "tilefade") {
			line[1].parse(ctx.node->tilefade);
		} else if (line[0] == "scale") {
			line[1].parse(ctx.node->scale);
		} else if (line[0] == "render") {
			line[1].parse(ctx.node->render);
		} else if (line[0] == "shadow") {
			line[1].parse(ctx.node->shadow);
		} else if (line[0] == "beaming") {
			line[1].parse(ctx.node->beaming);
		} else if (line[0] == "inheritcolor") {
			line[1].parse(ctx.node->inheritcolor);
		} else if (line[0] == "rotatetexture") {
			line[1].parse(ctx.node->rotatetexture);
		} else if (line[0] == "alpha") {
			line[1].parse(ctx.node->alpha);
		} else if (line[0] == "transparencyhint") {
			line[1].parse(ctx.node->transparencyhint);
		} else if (line[0] == "selfillumcolor") {
			parseFloats(line, ctx.node->selfillumcolor, 3, 1);
		} else if (line[0] == "danglymesh") {
			line[1].parse(ctx.node->dangly);
		} else if (line[0] == "gizmo") {
			warning("TODO: gizmo \"%s\"", line[1].c_str());
		} else if (line[0] == "constraints") {
			int n;

			line[1].parse(n);
			parseConstraintsASCII(ctx, ctx.node->constraints, n);
		} else if (line[0] == "weights") {
			warning("TODO: Weights");

			int n;

			line[1].parse(n);
			parseWeightsASCII(ctx, n);
		} else if (line[0] == "bitmap") {
			ctx.node->bitmap = line[1];
		} else if (line[0] == "verts") {
			int n;

			line[1].parse(n);
			parseVerticesASCII(ctx, ctx.vertices, n);
		} else if (line[0] == "tverts") {
			int n;

			line[1].parse(n);
			parseVerticesASCII(ctx, ctx.verticesTexture, n);
		} else if (line[0] == "faces") {
			int n;

			line[1].parse(n);
			parseFacesASCII(ctx, n);
		} else
			throw Common::Exception("Unknown MDL node command \"%s\"", line[0].c_str());
	}

	if (!end)
		throw Common::Exception("node without endnode");

	processNode(ctx);

	_nodes.push_back(ctx.node);
	_nodeMap.insert(std::make_pair(name, ctx.node));
	ctx.node = 0;
}

void Model_NWN::parseVerticesASCII(ParserContext &ctx, std::vector<float> &vertices, int n) {
	vertices.resize(3 * n);

	float *verts = &vertices[0];
	while (n > 0) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(*ctx.mdl, line);

		_tokenizeASCII->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		parseFloats(line, verts, 3, 0);

		n--;
		verts += 3;
	}
}

void Model_NWN::parseFacesASCII(ParserContext &ctx, int n) {
	ctx.faces.resize(n);

	for (int i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(*ctx.mdl, line);

		_tokenizeASCII->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		FaceNWN &face = ctx.faces[i++];

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

void Model_NWN::parseConstraintsASCII(ParserContext &ctx, std::vector<float> &constraints, int n) {
	constraints.resize(n);

	for (int i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(*ctx.mdl, line);

		_tokenizeASCII->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].parse(constraints[i++]);
	}
}

void Model_NWN::parseWeightsASCII(ParserContext &ctx, int n) {
	for (int i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(*ctx.mdl, line);

		_tokenizeASCII->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		i++;
	}
}

void Model_NWN::parseAnimASCII(ParserContext &ctx) {
	bool end = false;

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = _tokenizeASCII->getTokens(*ctx.mdl, line);

		_tokenizeASCII->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].tolower();

		if (line[0] == "doneanim") {
			end = true;
			break;
		}
	}

	if (!end)
		throw Common::Exception("anim without doneanim");
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

void Model_NWN::loadBinary(ParserContext &ctx) {
	uint32 sizeModelData = ctx.mdl->readUint32LE();
	uint32 sizeRawData   = ctx.mdl->readUint32LE();

	ctx.offModelData = 12;
	ctx.offRawData   = ctx.offModelData + sizeModelData;

	ctx.mdl->skip(8); // Function pointers

	_name.readASCII(*ctx.mdl, 64);

	uint32 nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32 nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24 + 4); // Unknown + Reference count

	uint8 type = ctx.mdl->readByte();

	ctx.mdl->skip(3 + 2); // Padding + Unknown

	uint8 classification = ctx.mdl->readByte();
	uint8 fogged         = ctx.mdl->readByte();

	ctx.mdl->skip(4); // Unknown

	ctx.mdl->skip(12); // TODO: Animation Header Pointer Array

	ctx.mdl->skip(4); // Parent model pointer

	float boundingMin[3], boundingMax[3];

	boundingMin[0] = ctx.mdl->readIEEEFloatLE();
	boundingMin[1] = ctx.mdl->readIEEEFloatLE();
	boundingMin[2] = ctx.mdl->readIEEEFloatLE();

	boundingMax[0] = ctx.mdl->readIEEEFloatLE();
	boundingMax[1] = ctx.mdl->readIEEEFloatLE();
	boundingMax[2] = ctx.mdl->readIEEEFloatLE();

	float radius = ctx.mdl->readIEEEFloatLE();

	_scale = ctx.mdl->readIEEEFloatLE();

	Common::UString superModelName;

	superModelName.readASCII(*ctx.mdl, 64);

	parseNodeBinary(ctx, nodeHeadPointer + ctx.offModelData, 0);
}

void Model_NWN::parseNodeBinary(ParserContext &ctx, uint32 offset, Node *parent) {
	ctx.mdl->seekTo(offset);

	ctx.node = new Node;

	if (parent) {
		ctx.node->parent = parent;
		parent->children.push_back(ctx.node);
	} else
		_rootNodes.push_back(ctx.node);

	ctx.mdl->skip(24); // Function pointers

	uint32 inheritColorFlag = ctx.mdl->readUint32LE();
	uint32 partNumber       = ctx.mdl->readUint32LE();

	ctx.node->name.readASCII(*ctx.mdl, 32);

	ctx.mdl->skip(8); // Parent pointers

	uint32 childrenStart, childrenCount;
	readArray(*ctx.mdl, childrenStart, childrenCount);

	std::vector<uint32> children;
	readOffsetArray(*ctx.mdl, childrenStart + ctx.offModelData, childrenCount, children);

	uint32 controllerKeyStart, controllerKeyCount;
	readArray(*ctx.mdl, controllerKeyStart, controllerKeyCount);

	uint32 controllerDataStart, controllerDataCount;
	readArray(*ctx.mdl, controllerDataStart, controllerDataCount);

	std::vector<float> controllerData;
	readFloatsArray(*ctx.mdl, controllerDataStart + ctx.offModelData, controllerDataCount, controllerData);

	parseNodeControllers(ctx, controllerKeyStart + ctx.offModelData, controllerKeyCount, controllerData);

	uint32 flags = ctx.mdl->readUint32LE();

	if ((flags & 0xFFFFFC00) != 0)
		throw Common::Exception("Unknown node flags %08X", flags);

	if (flags & kNodeFlagHasLight) {
		// TODO: Light
		ctx.mdl->skip(0x5C);
	}

	if (flags & kNodeFlagHasEmitter) {
		// TODO: Emitter
		ctx.mdl->skip(0xD8);
	}

	if (flags & kNodeFlagHasReference) {
		// TODO: Reference
		ctx.mdl->skip(0x44);
	}

	if (flags & kNodeFlagHasMesh) {
		parseMeshBinary(ctx);
	}

	if (flags & kNodeFlagHasSkin) {
		// TODO: Skin
		ctx.mdl->skip(0x64);
	}

	if (flags & kNodeFlagHasAnim) {
		// TODO: Anim
		ctx.mdl->skip(0x38);
	}

	if (flags & kNodeFlagHasDangly) {
		// TODO: Dangly
		ctx.mdl->skip(0x18);
	}

	if (flags & kNodeFlagHasAABB) {
		// TODO: AABB
		ctx.mdl->skip(0x4);
	}

	processNode(ctx);

	parent = ctx.node;

	_nodes.push_back(ctx.node);
	ctx.node = 0;

	for (std::vector<uint32>::const_iterator child = children.begin(); child != children.end(); ++child)
		parseNodeBinary(ctx, *child + ctx.offModelData, parent);
}

void Model_NWN::parseMeshBinary(ParserContext &ctx) {
	ctx.mdl->skip(8); // Function pointers

	uint32 facesStart, facesCount;
	readArray(*ctx.mdl, facesStart, facesCount);

	float boundingMin[3], boundingMax[3];

	boundingMin[0] = ctx.mdl->readIEEEFloatLE();
	boundingMin[1] = ctx.mdl->readIEEEFloatLE();
	boundingMin[2] = ctx.mdl->readIEEEFloatLE();

	boundingMax[0] = ctx.mdl->readIEEEFloatLE();
	boundingMax[1] = ctx.mdl->readIEEEFloatLE();
	boundingMax[2] = ctx.mdl->readIEEEFloatLE();

	float radius = ctx.mdl->readIEEEFloatLE();

	float pointsAverage[3];
	pointsAverage[0] = ctx.mdl->readIEEEFloatLE();
	pointsAverage[1] = ctx.mdl->readIEEEFloatLE();
	pointsAverage[2] = ctx.mdl->readIEEEFloatLE();

	ctx.node->ambient[0] = ctx.mdl->readIEEEFloatLE();
	ctx.node->ambient[1] = ctx.mdl->readIEEEFloatLE();
	ctx.node->ambient[2] = ctx.mdl->readIEEEFloatLE();

	ctx.node->diffuse[0] = ctx.mdl->readIEEEFloatLE();
	ctx.node->diffuse[1] = ctx.mdl->readIEEEFloatLE();
	ctx.node->diffuse[2] = ctx.mdl->readIEEEFloatLE();

	ctx.node->specular[0] = ctx.mdl->readIEEEFloatLE();
	ctx.node->specular[1] = ctx.mdl->readIEEEFloatLE();
	ctx.node->specular[2] = ctx.mdl->readIEEEFloatLE();

	ctx.node->shininess = ctx.mdl->readIEEEFloatLE();

	ctx.node->shadow  = ctx.mdl->readUint32LE() == 1;
	ctx.node->beaming = ctx.mdl->readUint32LE() == 1;
	ctx.node->render  = ctx.mdl->readUint32LE() == 1;

	ctx.node->transparencyhint = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4); // Unknown

	Common::UString textures[4];
	textures[0].readASCII(*ctx.mdl, 64);
	textures[1].readASCII(*ctx.mdl, 64);
	textures[2].readASCII(*ctx.mdl, 64);
	textures[3].readASCII(*ctx.mdl, 64);

	ctx.node->tilefade = ctx.mdl->readUint32LE();

	ctx.mdl->skip(12); // Vertex indices
	ctx.mdl->skip(12); // Left over faces

	ctx.mdl->skip(12); // Vertex indices counts
	ctx.mdl->skip(12); // Vertex indices offsets

	ctx.mdl->skip(8); // Unknown

	byte triangleMode = ctx.mdl->readByte();

	ctx.mdl->skip(3 + 4); // Padding + Unknown

	uint32 vertexOffset = ctx.mdl->readUint32LE();
	uint16 vertexCount  = ctx.mdl->readUint16LE();
	uint16 textureCount = ctx.mdl->readUint16LE();

	uint32 texture0VertexOffset = ctx.mdl->readUint32LE();
	ctx.mdl->skip(3 * 4); // Texture vertex data

	ctx.mdl->skip(4); // Vertex normals
	ctx.mdl->skip(4); // Vertex RGBA colors

	ctx.mdl->skip(6 * 4); // Texture animation data

	bool lightMapped = ctx.mdl->readByte() == 1;

	ctx.node->rotatetexture = ctx.mdl->readByte() == 1;

	ctx.mdl->skip(2); // Padding

	ctx.mdl->skip(4); // Normal sum / 2

	ctx.mdl->skip(4); // Unknown

	if (textureCount > 1)
		warning("Model_NWN::parseMeshBinary(): textureCount == %d (\"%s\", \"%s\", \"%s\", \"%s\")",
				textureCount, textures[0].c_str(), textures[1].c_str(), textures[2].c_str(), textures[3].c_str());

	ctx.node->bitmap = textures[0];

	if (vertexOffset != 0xFFFFFFFF) {
		ctx.mdl->seekTo(vertexOffset + ctx.offRawData);

		ctx.vertices.resize(3 * vertexCount);

		for (int i = 0; i < (3 * vertexCount); i++)
			ctx.vertices[i] = ctx.mdl->readIEEEFloatLE();
	}

	ctx.verticesTexture.resize(3 * vertexCount);
	if (texture0VertexOffset != 0xFFFFFFFF) {

		ctx.mdl->seekTo(texture0VertexOffset + ctx.offRawData);

		for (int i = 0; i < vertexCount; i++) {
			ctx.verticesTexture[3 * i + 0] = ctx.mdl->readIEEEFloatLE();
			ctx.verticesTexture[3 * i + 1] = ctx.mdl->readIEEEFloatLE();
			ctx.verticesTexture[3 * i + 2] = 0;
		}
	} else
		for (int i = 0; i < (3 * vertexCount); i++)
			ctx.verticesTexture[i] = 0;

	ctx.faces.resize(facesCount);
	if ((facesStart != 0) && (facesCount > 0)) {
		ctx.mdl->seekTo(facesStart + ctx.offModelData);

		for (uint32 i = 0; i < facesCount; i++) {
			ctx.mdl->skip(3 * 4); // Normal
			ctx.mdl->skip(    4); // Distance
			ctx.mdl->skip(    4); // ID
			ctx.mdl->skip(3 * 2); // Adjacent face number

			ctx.faces[i].vertices[0] = ctx.mdl->readUint16LE();
			ctx.faces[i].vertices[1] = ctx.mdl->readUint16LE();
			ctx.faces[i].vertices[2] = ctx.mdl->readUint16LE();
			ctx.faces[i].verticesTexture[0] = ctx.faces[i].vertices[0];
			ctx.faces[i].verticesTexture[1] = ctx.faces[i].vertices[1];
			ctx.faces[i].verticesTexture[2] = ctx.faces[i].vertices[2];
		}
	}

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

void Model_NWN::readFloatsArray(Common::SeekableReadStream &mdl, uint32 start, uint32 count,
		std::vector<float> &floats) {

	uint32 pos = mdl.seekTo(start);

	floats.reserve(count);
	while (count-- > 0)
		floats.push_back(mdl.readIEEEFloatLE());

	mdl.seekTo(pos);
}

void Model_NWN::parseNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data) {
	uint32 pos = ctx.mdl->seekTo(offset);

	// TODO: Implement this properly :P

	for (uint32 i = 0; i < count; i++) {
		uint32 type        = ctx.mdl->readUint32LE();
		uint16 rowCount    = ctx.mdl->readUint16LE();
		uint16 timeIndex   = ctx.mdl->readUint16LE();
		uint16 dataIndex   = ctx.mdl->readUint16LE();
		uint8  columnCount = ctx.mdl->readByte();
		ctx.mdl->skip(1);

		if (columnCount == 0xFFFF)
			throw Common::Exception("TODO: Model_NWN::parseNodeControllers(): columnCount == 0xFFFF");

		if        (type == kControllerTypePosition) {
			if (columnCount != 3)
				throw Common::Exception("Position controller with %d values", columnCount);

			ctx.node->position[0] = data[dataIndex + 0];
			ctx.node->position[1] = data[dataIndex + 1];
			ctx.node->position[2] = data[dataIndex + 2];

		} else if (type == kControllerTypeOrientation) {
			if (columnCount != 4)
				throw Common::Exception("Orientation controller with %d values", columnCount);

			ctx.node->orientation[0] = data[dataIndex + 0];
			ctx.node->orientation[1] = data[dataIndex + 1];
			ctx.node->orientation[2] = data[dataIndex + 2];
			ctx.node->orientation[3] = Common::rad2deg(acos(data[dataIndex + 3]) * 2.0);
		}

	}

	ctx.mdl->seekTo(pos);
}

void Model_NWN::processNode(ParserContext &ctx) {
	ctx.node->faces.resize(ctx.faces.size());

	for (uint i = 0; i < ctx.faces.size(); i++) {
		Face &face = ctx.node->faces[i];

		const float *verts = &ctx.vertices[0];
		const int    vert0 = ctx.faces[i].vertices[0];
		const int    vert1 = ctx.faces[i].vertices[1];
		const int    vert2 = ctx.faces[i].vertices[2];

		face.vertices[0][0] = verts[3 * vert0 + 0];
		face.vertices[0][1] = verts[3 * vert0 + 1];
		face.vertices[0][2] = verts[3 * vert0 + 2];
		face.vertices[1][0] = verts[3 * vert1 + 0];
		face.vertices[1][1] = verts[3 * vert1 + 1];
		face.vertices[1][2] = verts[3 * vert1 + 2];
		face.vertices[2][0] = verts[3 * vert2 + 0];
		face.vertices[2][1] = verts[3 * vert2 + 1];
		face.vertices[2][2] = verts[3 * vert2 + 2];

		const float *tverts = &ctx.verticesTexture[0];
		const int    tvert0 = ctx.faces[i].verticesTexture[0];
		const int    tvert1 = ctx.faces[i].verticesTexture[1];
		const int    tvert2 = ctx.faces[i].verticesTexture[2];

		if (ctx.faces[i].verticesTexture[0] >= (ctx.verticesTexture.size() * 3)) {
			face.verticesTexture[0][0] = 0;
			face.verticesTexture[0][1] = 0;
			face.verticesTexture[0][2] = 0;
			face.verticesTexture[1][0] = 0;
			face.verticesTexture[1][1] = 0;
			face.verticesTexture[1][2] = 0;
			face.verticesTexture[2][0] = 0;
			face.verticesTexture[2][1] = 0;
			face.verticesTexture[2][2] = 0;
		} else {
			face.verticesTexture[0][0] = tverts[3 * tvert0 + 0];
			face.verticesTexture[0][1] = tverts[3 * tvert0 + 1];
			face.verticesTexture[0][2] = tverts[3 * tvert0 + 2];
			face.verticesTexture[1][0] = tverts[3 * tvert1 + 0];
			face.verticesTexture[1][1] = tverts[3 * tvert1 + 1];
			face.verticesTexture[1][2] = tverts[3 * tvert1 + 2];
			face.verticesTexture[2][0] = tverts[3 * tvert2 + 0];
			face.verticesTexture[2][1] = tverts[3 * tvert2 + 1];
			face.verticesTexture[2][2] = tverts[3 * tvert2 + 2];
		}

		face.smoothGroup = ctx.faces[i].smoothGroup;
		face.material    = ctx.faces[i].material;
	}

	try {
		if (!ctx.node->bitmap.empty() && (ctx.node->bitmap != "NULL"))
			ctx.node->texture = new Texture(ctx.node->bitmap);
	} catch (...) {
		ctx.node->bitmap.clear();
		ctx.node->texture = 0;
	}

	ctx.vertices.clear();
	ctx.verticesTexture.clear();
	ctx.faces.clear();
}

} // End of namespace Aurora

} // End of namespace Graphics
