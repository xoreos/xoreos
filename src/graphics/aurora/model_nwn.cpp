/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/aurora/model_nwn.cpp
 *  Loading MDL files found in Neverwinter Nights.
 */

// Disable the "unused variable" warnings while most stuff is still stubbed
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

#include "common/error.h"
#include "common/maths.h"
#include "common/debug.h"
#include "common/stream.h"
#include "common/streamtokenizer.h"

#include "aurora/types.h"
#include "aurora/resman.h"

#include "graphics/aurora/model_nwn.h"

using Common::kDebugGraphics;

static const int kNodeFlagHasHeader    = 0x00000001;
static const int kNodeFlagHasLight     = 0x00000002;
static const int kNodeFlagHasEmitter   = 0x00000004;
static const int kNodeFlagHasReference = 0x00000010;
static const int kNodeFlagHasMesh      = 0x00000020;
static const int kNodeFlagHasSkin      = 0x00000040;
static const int kNodeFlagHasAnim      = 0x00000080;
static const int kNodeFlagHasDangly    = 0x00000100;
static const int kNodeFlagHasAABB      = 0x00000200;

static const uint16 kControllerTypePosition             = 8;
static const uint16 kControllerTypeOrientation          = 20;
static const uint16 kControllerTypeScale                = 36;
static const uint16 kControllerTypeColor                = 76;
static const uint16 kControllerTypeRadius               = 88;
static const uint16 kControllerTypeShadowRadius         = 96;
static const uint16 kControllerTypeVerticalDisplacement = 100;
static const uint16 kControllerTypeMultiplier           = 140;
static const uint16 kControllerTypeAlphaEnd             = 80;
static const uint16 kControllerTypeAlphaStart           = 84;
static const uint16 kControllerTypeBirthRate            = 88;
static const uint16 kControllerTypeBounce_Co            = 92;
static const uint16 kControllerTypeColorEnd             = 96;
static const uint16 kControllerTypeColorStart           = 108;
static const uint16 kControllerTypeCombineTime          = 120;
static const uint16 kControllerTypeDrag                 = 124;
static const uint16 kControllerTypeFPS                  = 128;
static const uint16 kControllerTypeFrameEnd             = 132;
static const uint16 kControllerTypeFrameStart           = 136;
static const uint16 kControllerTypeGrav                 = 140;
static const uint16 kControllerTypeLifeExp              = 144;
static const uint16 kControllerTypeMass                 = 148;
static const uint16 kControllerTypeP2P_Bezier2          = 152;
static const uint16 kControllerTypeP2P_Bezier3          = 156;
static const uint16 kControllerTypeParticleRot          = 160;
static const uint16 kControllerTypeRandVel              = 164;
static const uint16 kControllerTypeSizeStart            = 168;
static const uint16 kControllerTypeSizeEnd              = 172;
static const uint16 kControllerTypeSizeStart_Y          = 176;
static const uint16 kControllerTypeSizeEnd_Y            = 180;
static const uint16 kControllerTypeSpread               = 184;
static const uint16 kControllerTypeThreshold            = 188;
static const uint16 kControllerTypeVelocity             = 192;
static const uint16 kControllerTypeXSize                = 196;
static const uint16 kControllerTypeYSize                = 200;
static const uint16 kControllerTypeBlurLength           = 204;
static const uint16 kControllerTypeLightningDelay       = 208;
static const uint16 kControllerTypeLightningRadius      = 212;
static const uint16 kControllerTypeLightningScale       = 216;
static const uint16 kControllerTypeDetonate             = 228;
static const uint16 kControllerTypeAlphaMid             = 464;
static const uint16 kControllerTypeColorMid             = 468;
static const uint16 kControllerTypePercentStart         = 480;
static const uint16 kControllerTypePercentMid           = 481;
static const uint16 kControllerTypePercentEnd           = 482;
static const uint16 kControllerTypeSizeMid              = 484;
static const uint16 kControllerTypeSizeMid_Y            = 488;
static const uint16 kControllerTypeSelfIllumColor       = 100;
static const uint16 kControllerTypeAlpha                = 128;

namespace Graphics {

namespace Aurora {

Model_NWN::ParserContext::ParserContext(const Common::UString &name,
                                        const Common::UString &t) :
	mdl(0), state(0), texture(t) {

	mdl = ResMan.getResource(name, ::Aurora::kFileTypeMDL);
	if (!mdl)
		throw Common::Exception("No such MDL \"%s\"", name.c_str());

	mdl->seek(0);
	isASCII = mdl->readUint32LE() != 0;

	if (isASCII) {
		tokenize = new Common::StreamTokenizer(Common::StreamTokenizer::kRuleIgnoreAll);

		tokenize->addSeparator(' ');
		tokenize->addChunkEnd('\n');
		tokenize->addIgnore('\r');
	} else
		tokenize = 0;
}

Model_NWN::ParserContext::~ParserContext() {
	delete tokenize;
	delete mdl;

	clear();
}

void Model_NWN::ParserContext::clear() {
	for (std::list<ModelNode *>::iterator n = nodes.begin(); n != nodes.end(); ++n)
		delete *n;
	nodes.clear();

	delete state;
	state = 0;
}

bool Model_NWN::ParserContext::findNode(const Common::UString &name,
		ModelNode *&node) const {

	node = 0;

	if (name.empty() || (name == "NULL"))
		return true;

	for (std::list<ModelNode *>::const_iterator n = nodes.begin();
	     n != nodes.end(); ++n) {

		if ((*n)->getName() == name) {
			node = *n;
			return true;
		}
	}

	return false;
}


Model_NWN::Model_NWN(const Common::UString &name, ModelType type,
                     const Common::UString &texture) :
	Model(type) {

	if (_type == kModelTypeGUIFront) {
		// NWN GUI objects use 0.01 units / pixel
		_modelScale[0] = _modelScale[1] = 100.0;
		_modelScale[2] = 1.0;
	}

	_fileName = name;

	ParserContext ctx(name, texture);

	if (ctx.isASCII)
		loadASCII(ctx);
	else
		loadBinary(ctx);

	if(!_superModelName.empty() && _superModelName != "NULL")
		_supermodel = new Model_NWN(_superModelName, type, "");

	finalize();
}

Model_NWN::~Model_NWN() {
}

void Model_NWN::loadBinary(ParserContext &ctx) {
	ctx.mdl->seek(4);

	uint32 sizeModelData = ctx.mdl->readUint32LE();
	uint32 sizeRawData   = ctx.mdl->readUint32LE();

	ctx.offModelData = 12;
	ctx.offRawData   = ctx.offModelData + sizeModelData;

	ctx.mdl->skip(8); // Function pointers

	_name.readFixedASCII(*ctx.mdl, 64);
	debugC(4, kDebugGraphics, "Loading NWN binary model \"%s\": \"%s\"", _fileName.c_str(),
	       _name.c_str());

	uint32 nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32 nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24 + 4); // Unknown + Reference count

	uint8 type = ctx.mdl->readByte();

	ctx.mdl->skip(3 + 2); // Padding + Unknown

	uint8 classification = ctx.mdl->readByte();
	uint8 fogged         = ctx.mdl->readByte();

	ctx.mdl->skip(4); // Unknown

	uint32 animOffset, animCount;
	readArrayDef(*ctx.mdl, animOffset, animCount);

	ctx.mdl->skip(4); // Parent model pointer

	float boundingMin[3], boundingMax[3];

	boundingMin[0] = ctx.mdl->readIEEEFloatLE();
	boundingMin[1] = ctx.mdl->readIEEEFloatLE();
	boundingMin[2] = ctx.mdl->readIEEEFloatLE();

	boundingMax[0] = ctx.mdl->readIEEEFloatLE();
	boundingMax[1] = ctx.mdl->readIEEEFloatLE();
	boundingMax[2] = ctx.mdl->readIEEEFloatLE();

	float radius = ctx.mdl->readIEEEFloatLE();

	float scale = ctx.mdl->readIEEEFloatLE();

	_superModelName.readFixedASCII(*ctx.mdl, 64);

	newState(ctx);

	ModelNode_NWN_Binary *rootNode = new ModelNode_NWN_Binary(*this);
	ctx.nodes.push_back(rootNode);

	ctx.mdl->seek(ctx.offModelData + nodeHeadPointer);
	rootNode->load(ctx);

	addState(ctx);

	std::vector<uint32> animOffsets;
	readArray(*ctx.mdl, ctx.offModelData + animOffset, animCount, animOffsets);

	for (std::vector<uint32>::const_iterator offset = animOffsets.begin(); offset != animOffsets.end(); ++offset) {
		newState(ctx);

		readAnimBinary(ctx, ctx.offModelData + *offset);

		addState(ctx);
	}
}

void Model_NWN::loadASCII(ParserContext &ctx) {
	ctx.mdl->seek(0);

	newState(ctx);

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 3);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].tolower();

		if        (line[0] == "newmodel") {
			if (!_name.empty())
				warning("Model_NWN_ASCII::load(): More than one model definition");

			debugC(4, kDebugGraphics, "Loading NWN ASCII model \"%s\": \"%s\"", _fileName.c_str(),
			       _name.c_str());

			_name = line[1];
		} else if (line[0] == "setsupermodel") {
			if (line[1] != _name)
				warning("Model_NWN_ASCII::load(): setsupermodel: \"%s\" != \"%s\"",
				        line[1].c_str(), _name.c_str());

			if (!line[2].empty() && (line[2] != "NULL"))
				_superModelName = line[2];

		} else if (line[0] == "beginmodelgeom") {
			if (line[1] != _name)
				warning("Model_NWN_ASCII::load(): beginmodelgeom: \"%s\" != \"%s\"",
				        line[1].c_str(), _name.c_str());
		} else if (line[0] == "node") {

			ModelNode_NWN_ASCII *newNode = new ModelNode_NWN_ASCII(*this);
			ctx.nodes.push_back(newNode);

			newNode->load(ctx, line[1], line[2]);

		} else if (line[0] == "newanim") {
			ctx.anims.push_back(ctx.mdl->pos());
			skipAnimASCII(ctx);
		} else if (line[0] == "donemodel") {
			break;
		} else
			;//warning("Unknown MDL command \"%s\"", line[0].c_str());
	}

	addState(ctx);

	for (std::vector<uint32>::iterator a = ctx.anims.begin(); a != ctx.anims.end(); ++a) {
		ctx.mdl->seek(*a);
		readAnimASCII(ctx);
	}
}
void Model_NWN::newState(ParserContext &ctx) {
	ctx.clear();

	ctx.hasPosition    = false;
	ctx.hasOrientation = false;
	ctx.state          = new State;
}

void Model_NWN::skipAnimASCII(ParserContext &ctx) {
	bool end = false;

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 1);

		ctx.tokenize->nextChunk(*ctx.mdl);

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

void Model_NWN::readAnimASCII(ParserContext &ctx) {
	// TODO: Model_NWN_ASCII::readAnimASCII
}

void Model_NWN::addState(ParserContext &ctx) {
	if (!ctx.state || ctx.nodes.empty()) {
		ctx.clear();
		return;
	}

	for (std::list<ModelNode *>::iterator n = ctx.nodes.begin();
	     n != ctx.nodes.end(); ++n) {

		ctx.state->nodeList.push_back(*n);
		ctx.state->nodeMap.insert(std::make_pair((*n)->getName(), *n));

		if (!(*n)->getParent())
			ctx.state->rootNodes.push_back(*n);
	}

	_stateList.push_back(ctx.state);
	_stateMap.insert(std::make_pair(ctx.state->name, ctx.state));

	if (!_currentState)
		_currentState = ctx.state;

	ctx.state = 0;

	ctx.nodes.clear();
}

void Model_NWN::readAnimBinary(ParserContext &ctx, uint32 offset) {
	ctx.mdl->seekTo(offset);

	ctx.mdl->skip(8); // Function pointers

	ctx.state->name.readFixedASCII(*ctx.mdl, 64);

	uint32 nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32 nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24 + 4); // Unknown + Reference count

	uint8 type = ctx.mdl->readByte();

	ctx.mdl->skip(3); // Padding

	float animLength = ctx.mdl->readIEEEFloatLE();
	float transTime  = ctx.mdl->readIEEEFloatLE();

	Common::UString animRoot;
	animRoot.readFixedASCII(*ctx.mdl, 64);

	uint32 eventOffset, eventCount;
	readArrayDef(*ctx.mdl, eventOffset, eventCount);

	// Associated events
	ctx.mdl->seekTo(ctx.offModelData + eventOffset);
	for (uint32 i = 0; i < eventCount; i++) {
		float after = ctx.mdl->readIEEEFloatLE();

		Common::UString eventName;
		eventName.readFixedASCII(*ctx.mdl, 32);
	}

	ModelNode_NWN_Binary *rootNode = new ModelNode_NWN_Binary(*this);
	ctx.nodes.push_back(rootNode);

	ctx.mdl->seek(ctx.offModelData + nodeHeadPointer);
	rootNode->load(ctx);
}


ModelNode_NWN_Binary::ModelNode_NWN_Binary(Model &model) : ModelNode(model) {
	_hasTransparencyHint = true;
	_transparencyHint = false;
}

ModelNode_NWN_Binary::~ModelNode_NWN_Binary() {
}

void ModelNode_NWN_Binary::load(Model_NWN::ParserContext &ctx) {
	ctx.mdl->skip(24); // Function pointers

	uint32 inheritColorFlag = ctx.mdl->readUint32LE();
	uint32 partNumber       = ctx.mdl->readUint32LE();

	_name.readFixedASCII(*ctx.mdl, 32);

	debugC(5, kDebugGraphics, "Node \"%s\" in state \"%s\"", _name.c_str(),
	       ctx.state->name.c_str());

	ctx.mdl->skip(8); // Parent pointers

	uint32 childrenOffset, childrenCount;
	Model::readArrayDef(*ctx.mdl, childrenOffset, childrenCount);

	std::vector<uint32> children;
	Model::readArray(*ctx.mdl, ctx.offModelData + childrenOffset, childrenCount, children);

	uint32 controllerKeyOffset, controllerKeyCount;
	Model::readArrayDef(*ctx.mdl, controllerKeyOffset, controllerKeyCount);

	uint32 controllerDataOffset, controllerDataCount;
	Model::readArrayDef(*ctx.mdl, controllerDataOffset, controllerDataCount);

	std::vector<float> controllerData;
	Model::readArray(*ctx.mdl, ctx.offModelData + controllerDataOffset,
	                 controllerDataCount, controllerData);

	uint32 flags = ctx.mdl->readUint32LE();

	if ((flags & 0xFFFFFC00) != 0)
		throw Common::Exception("Unknown model node flags %08X", flags);

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
		readMesh(ctx);
	}

	if (flags & kNodeFlagHasSkin) {
		// TODO: Skin
		ctx.mdl->skip(0x64);
	}

	if (flags & kNodeFlagHasAnim) {
		readAnim(ctx);
	}

	if (flags & kNodeFlagHasDangly) {
		// TODO: Dangly
		ctx.mdl->skip(0x18);
	}

	if (flags & kNodeFlagHasAABB) {
		// TODO: AABB
		ctx.mdl->skip(0x4);
	}

	// If the node has no own geometry, inherit the geometry from the root state
	if (!(flags & kNodeFlagHasMesh)) {
		ModelNode *node = _model->getNode(_name);
		if (node && (node != this))
			node->inheritGeometry(*this);
	}

	readNodeControllers(ctx, ctx.offModelData + controllerKeyOffset,
	                    controllerKeyCount, controllerData);

	// If the node has no own position controller, inherit the position from the root state
	if (!ctx.hasPosition) {
		ModelNode *node = _model->getNode(_name);
		if (node)
			node->inheritPosition(*this);
	}

	if (!ctx.hasOrientation) {
		ModelNode *node = _model->getNode(_name);
		if (node)
			node->inheritOrientation(*this);
	}


	for (std::vector<uint32>::const_iterator child = children.begin(); child != children.end(); ++child) {
		ctx.hasPosition    = false;
		ctx.hasOrientation = false;

		ModelNode_NWN_Binary *childNode = new ModelNode_NWN_Binary(*_model);
		ctx.nodes.push_back(childNode);

		childNode->setParent(this);

		ctx.mdl->seek(ctx.offModelData + *child);
		childNode->load(ctx);
	}

}

void ModelNode_NWN_Binary::readMesh(Model_NWN::ParserContext &ctx) {
	ctx.mdl->skip(8); // Function pointers

	uint32 facesOffset, facesCount;
	Model::readArrayDef(*ctx.mdl, facesOffset, facesCount);

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

	_ambient[0] = ctx.mdl->readIEEEFloatLE();
	_ambient[1] = ctx.mdl->readIEEEFloatLE();
	_ambient[2] = ctx.mdl->readIEEEFloatLE();

	_diffuse[0] = ctx.mdl->readIEEEFloatLE();
	_diffuse[1] = ctx.mdl->readIEEEFloatLE();
	_diffuse[2] = ctx.mdl->readIEEEFloatLE();

	_specular[0] = ctx.mdl->readIEEEFloatLE();
	_specular[1] = ctx.mdl->readIEEEFloatLE();
	_specular[2] = ctx.mdl->readIEEEFloatLE();

	_shininess = ctx.mdl->readIEEEFloatLE();

	_shadow  = ctx.mdl->readUint32LE() == 1;
	_beaming = ctx.mdl->readUint32LE() == 1;
	_render  = ctx.mdl->readUint32LE() == 1;

	_transparencyHint = ctx.mdl->readUint32LE() == 1;

	ctx.mdl->skip(4); // Unknown

	std::vector<Common::UString> textures;
	textures.resize(4);
	textures[0].readFixedASCII(*ctx.mdl, 64);
	textures[1].readFixedASCII(*ctx.mdl, 64);
	textures[2].readFixedASCII(*ctx.mdl, 64);
	textures[3].readFixedASCII(*ctx.mdl, 64);

	_tilefade = ctx.mdl->readUint32LE();

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

	uint32 textureVertexOffset[4];
	textureVertexOffset[0] = ctx.mdl->readUint32LE();
	textureVertexOffset[1] = ctx.mdl->readUint32LE();
	textureVertexOffset[2] = ctx.mdl->readUint32LE();
	textureVertexOffset[3] = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4); // Vertex normals
	ctx.mdl->skip(4); // Vertex RGBA colors

	ctx.mdl->skip(6 * 4); // Texture animation data

	bool lightMapped = ctx.mdl->readByte() == 1;

	_rotatetexture = ctx.mdl->readByte() == 1;

	ctx.mdl->skip(2); // Padding

	ctx.mdl->skip(4); // Normal sum / 2

	ctx.mdl->skip(4); // Unknown

	if ((vertexCount == 0) || (facesCount == 0) || (facesOffset == 0))
		return;

	if (textureCount > 4) {
		warning("ModelNode_NWN_Binary::readMesh(): textureCount > 4 (%d)", textureCount);
		textureCount = 4;
	}

	textures.resize(textureCount);
	loadTextures(textures);

	uint32 endPos = ctx.mdl->pos();

	// Read vertex coordinates
	std::vector<float> vX, vY, vZ;
	if (vertexOffset != 0xFFFFFFFF) {
		ctx.mdl->seekTo(ctx.offRawData + vertexOffset);

		vX.resize(vertexCount);
		vY.resize(vertexCount);
		vZ.resize(vertexCount);

		for (uint32 i = 0; i < vertexCount; i++) {
			vX[i] = ctx.mdl->readIEEEFloatLE();
			vY[i] = ctx.mdl->readIEEEFloatLE();
			vZ[i] = ctx.mdl->readIEEEFloatLE();
		}
	}

	// Read texture coordinates
	std::vector< std::vector<float> > tX, tY;
	tX.resize(textureCount);
	tY.resize(textureCount);
	for (uint16 t = 0; t < textureCount; t++) {
		tX[t].resize(vertexCount);
		tY[t].resize(vertexCount);

		bool hasTexture = textureVertexOffset[t] != 0xFFFFFFFF;
		if (hasTexture)
			ctx.mdl->seekTo(ctx.offRawData + textureVertexOffset[t]);

		for (uint32 i = 0; i < vertexCount; i++) {
			tX[t][i] = hasTexture ? ctx.mdl->readIEEEFloatLE() : 0.0;
			tY[t][i] = hasTexture ? ctx.mdl->readIEEEFloatLE() : 0.0;
		}
	}

	// Read faces

	if (!createFaces(facesCount)) {
		ctx.mdl->seekTo(endPos);
		return;
	}

	ctx.mdl->seekTo(ctx.offModelData + facesOffset);
	for (uint32 i = 0; i < facesCount; i++) {
		ctx.mdl->skip(3 * 4); // Normal
		ctx.mdl->skip(    4); // Distance
		ctx.mdl->skip(    4); // ID
		ctx.mdl->skip(3 * 2); // Adjacent face number

		// Vertex indices
		const uint16 v1 = ctx.mdl->readUint16LE();
		const uint16 v2 = ctx.mdl->readUint16LE();
		const uint16 v3 = ctx.mdl->readUint16LE();

		// Vertex coordinates
		_vX[3 * i + 0] = v1 < vX.size() ? vX[v1] : 0.0;
		_vY[3 * i + 0] = v1 < vY.size() ? vY[v1] : 0.0;
		_vZ[3 * i + 0] = v1 < vZ.size() ? vZ[v1] : 0.0;
		_boundBox.add(_vX[3 * i + 0], _vY[3 * i + 0], _vZ[3 * i + 0]);

		_vX[3 * i + 1] = v2 < vX.size() ? vX[v2] : 0.0;
		_vY[3 * i + 1] = v2 < vY.size() ? vY[v2] : 0.0;
		_vZ[3 * i + 1] = v2 < vZ.size() ? vZ[v2] : 0.0;
		_boundBox.add(_vX[3 * i + 1], _vY[3 * i + 1], _vZ[3 * i + 1]);

		_vX[3 * i + 2] = v3 < vX.size() ? vX[v3] : 0.0;
		_vY[3 * i + 2] = v3 < vY.size() ? vY[v3] : 0.0;
		_vZ[3 * i + 2] = v3 < vZ.size() ? vZ[v3] : 0.0;
		_boundBox.add(_vX[3 * i + 2], _vY[3 * i + 2], _vZ[3 * i + 2]);

		// Texture coordinates
		for (uint32 t = 0; t < textureCount; t++) {
			_tX[3 * textureCount * i + 3 * t + 0] = v1 < tX[t].size() ? tX[t][v1] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 0] = v1 < tY[t].size() ? tY[t][v1] : 0.0;

			_tX[3 * textureCount * i + 3 * t + 1] = v2 < tX[t].size() ? tX[t][v2] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 1] = v2 < tY[t].size() ? tY[t][v2] : 0.0;

			_tX[3 * textureCount * i + 3 * t + 2] = v3 < tX[t].size() ? tX[t][v3] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 2] = v3 < tY[t].size() ? tY[t][v3] : 0.0;
		}

	}

	createCenter();

	ctx.mdl->seekTo(endPos);
}

void ModelNode_NWN_Binary::readAnim(Model_NWN::ParserContext &ctx) {
	float samplePeriod = ctx.mdl->readIEEEFloatLE();

	uint32 a0S, a0C;
	Model::readArrayDef(*ctx.mdl, a0S, a0C);

	uint32 a1S, a1C;
	Model::readArrayDef(*ctx.mdl, a1S, a1C);

	uint32 a2S, a2C;
	Model::readArrayDef(*ctx.mdl, a2S, a2C);

	uint32 offAnimVertices        = ctx.mdl->readUint32LE();
	uint32 offAnimTextureVertices = ctx.mdl->readUint32LE();

	uint32 verticesCount        = ctx.mdl->readUint32LE();
	uint32 textureVerticesCount = ctx.mdl->readUint32LE();
}

void ModelNode_NWN_Binary::readNodeControllers(Model_NWN::ParserContext &ctx,
	uint32 offset, uint32 count, std::vector<float> &data) {

	uint32 pos = ctx.mdl->seekTo(offset);

	// TODO: readNodeControllers: Implement this properly :P

	for (uint32 i = 0; i < count; i++) {
		uint32 type        = ctx.mdl->readUint32LE();
		uint16 rowCount    = ctx.mdl->readUint16LE();
		uint16 timeIndex   = ctx.mdl->readUint16LE();
		uint16 dataIndex   = ctx.mdl->readUint16LE();
		uint8  columnCount = ctx.mdl->readByte();
		ctx.mdl->skip(1);

		if (rowCount == 0xFFFF)
			// TODO: Controller row count = 0xFFFF
			continue;

		if        (type == kControllerTypePosition) {
			if (columnCount != 3)
				throw Common::Exception("Position controller with %d values", columnCount);

			// Starting position
			if (data[timeIndex + 0] == 0.0) {
				_position[0] = data[dataIndex + 0];
				_position[1] = data[dataIndex + 1];
				_position[2] = data[dataIndex + 2];

				ctx.hasPosition = true;
			}

		} else if (type == kControllerTypeOrientation) {
			if (columnCount != 4)
				throw Common::Exception("Orientation controller with %d values", columnCount);

			// Starting orientation
			if (data[timeIndex + 0] == 0.0) {
				_orientation[0] = data[dataIndex + 0];
				_orientation[1] = data[dataIndex + 1];
				_orientation[2] = data[dataIndex + 2];
				_orientation[3] = Common::rad2deg(acos(data[dataIndex + 3]) * 2.0);

				ctx.hasOrientation = true;
			}

		} else if (type == kControllerTypeAlpha) {
			if (columnCount != 1)
				throw Common::Exception("Alpha controller with %d values", columnCount);

			// Starting alpha
			if (data[timeIndex + 0] == 0.0)
				if (data[dataIndex + 0] == 0.0)
					// TODO: Just disabled rendering if alpha == 0.0 for now
					_render = false;
		}

	}

	ctx.mdl->seekTo(pos);
}


ModelNode_NWN_ASCII::Mesh::Mesh() : vCount(0), tCount(0), faceCount(0) {
}


ModelNode_NWN_ASCII::ModelNode_NWN_ASCII(Model &model) : ModelNode(model) {
	_hasTransparencyHint = true;
	_transparencyHint = false;
}

ModelNode_NWN_ASCII::~ModelNode_NWN_ASCII() {
}

void ModelNode_NWN_ASCII::load(Model_NWN::ParserContext &ctx,
                               const Common::UString &type, const Common::UString &name) {

	bool end      = false;
	bool skipNode = false;

	_name = name;

	debugC(5, kDebugGraphics, "Node \"%s\" in state \"%s\"", _name.c_str(),
	       ctx.state->name.c_str());

	if ((type == "trimesh") || (type == "danglymesh") || (type == "skin"))
		_render = true;
	else
		_render = false;

	if ((type == "emitter") || (type == "reference") || (type == "aabb")) {
		// warning("TODO: Node type %s", type.c_str());
		skipNode = true;
	}

	if (type == "danglymesh")
		_dangly = true;

	Mesh mesh;

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 5);

		ctx.tokenize->nextChunk(*ctx.mdl);

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
			ModelNode *parent = 0;

			if (!ctx.findNode(line[1], parent))
				warning("ModelNode_NWN_ASCII::load(): Non-existent parent node \"%s\"",
				        line[1].c_str());

			setParent(parent);

		} else if (line[0] == "position") {
			readFloats(line, _position, 3, 1);
		} else if (line[0] == "orientation") {
			readFloats(line, _orientation, 4, 1);

			_orientation[3] = Common::rad2deg(_orientation[3]);
		} else if (line[0] == "render") {
			line[1].parse(_render);
		} else if (line[0] == "transparencyhint") {
			line[1].parse(_transparencyHint);
		} else if (line[0] == "danglymesh") {
			line[1].parse(_dangly);
		} else if (line[0] == "constraints") {
			uint32 n;

			line[1].parse(n);
			readConstraints(ctx, n);
		} else if (line[0] == "weights") {
			uint32 n;

			line[1].parse(n);
			readWeights(ctx, n);
		} else if (line[0] == "bitmap") {
			mesh.textures.push_back(line[1]);
		} else if (line[0] == "verts") {
			line[1].parse(mesh.vCount);

			readVCoords(ctx, mesh);
		} else if (line[0] == "tverts") {
			if (mesh.tCount != 0)
				warning("ModelNode_NWN_ASCII::load(): Multiple texture coordinates!");

			line[1].parse(mesh.tCount);

			readTCoords(ctx, mesh);
		} else if (line[0] == "faces") {
			line[1].parse(mesh.faceCount);

			readFaces(ctx, mesh);
		} else
			;//warning("Unknown MDL node command \"%s\"", line[0].c_str());
	}

	if (!end)
		throw Common::Exception("ModelNode_NWN_ASCII::load(): node without endnode");

	if (!mesh.textures.empty() && !ctx.texture.empty())
		mesh.textures[0] = ctx.texture;

	processMesh(mesh);
}

void ModelNode_NWN_ASCII::readConstraints(Model_NWN::ParserContext &ctx, uint32 n) {
	for (uint32 i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 1);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		i++;
	}
}

void ModelNode_NWN_ASCII::readWeights(Model_NWN::ParserContext &ctx, uint32 n) {
	for (uint32 i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 1);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		i++;
	}
}

void ModelNode_NWN_ASCII::readFloats(const std::vector<Common::UString> &strings,
                                     float *floats, uint32 n, uint32 start) {

	if (strings.size() < (start + n))
		throw Common::Exception("Missing tokens");

	for (uint32 i = 0; i < n; i++)
		strings[start + i].parse(floats[i]);
}

void ModelNode_NWN_ASCII::readVCoords(Model_NWN::ParserContext &ctx, Mesh &mesh) {
	mesh.vX.resize(mesh.vCount);
	mesh.vY.resize(mesh.vCount);
	mesh.vZ.resize(mesh.vCount);

	for (uint32 i = 0; i < mesh.vCount; ) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 3);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].parse(mesh.vX[i]);
		line[1].parse(mesh.vY[i]);
		line[2].parse(mesh.vZ[i]);

		i++;
	}
}

void ModelNode_NWN_ASCII::readTCoords(Model_NWN::ParserContext &ctx, Mesh &mesh) {
	mesh.tX.resize(mesh.tCount);
	mesh.tY.resize(mesh.tCount);

	for (uint32 i = 0; i < mesh.tCount; ) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 2);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].parse(mesh.tX[i]);
		line[1].parse(mesh.tY[i]);

		i++;
	}
}

void ModelNode_NWN_ASCII::readFaces(Model_NWN::ParserContext &ctx, Mesh &mesh) {
	mesh.vIA.resize(mesh.faceCount);
	mesh.vIB.resize(mesh.faceCount);
	mesh.vIC.resize(mesh.faceCount);

	mesh.tIA.resize(mesh.faceCount);
	mesh.tIB.resize(mesh.faceCount);
	mesh.tIC.resize(mesh.faceCount);

	mesh.smooth.resize(mesh.faceCount);
	mesh.mat.resize(mesh.faceCount);

	for (uint32 i = 0; i < mesh.faceCount; ) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 8);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].parse(mesh.vIA[i]);
		line[1].parse(mesh.vIB[i]);
		line[2].parse(mesh.vIC[i]);

		line[3].parse(mesh.smooth[i]);

		line[4].parse(mesh.tIA[i]);
		line[5].parse(mesh.tIB[i]);
		line[6].parse(mesh.tIC[i]);

		line[7].parse(mesh.mat[i]);

		i++;
	}
}

void ModelNode_NWN_ASCII::processMesh(Mesh &mesh) {
	loadTextures(mesh.textures);
	if (!createFaces(mesh.faceCount))
		return;

	const uint32 textureCount = mesh.textures.size();
	if (textureCount > 1)
		warning("ModelNode_NWN_ASCII::processMesh(): textureCount == %d", textureCount);

	for (uint32 i = 0; i < mesh.faceCount; i++) {
		const uint32 v1 = mesh.vIA[i];
		const uint32 v2 = mesh.vIB[i];
		const uint32 v3 = mesh.vIC[i];

		// Vertex coordinates
		_vX[3 * i + 0] = v1 < mesh.vCount ? mesh.vX[v1] : 0.0;
		_vY[3 * i + 0] = v1 < mesh.vCount ? mesh.vY[v1] : 0.0;
		_vZ[3 * i + 0] = v1 < mesh.vCount ? mesh.vZ[v1] : 0.0;
		_boundBox.add(_vX[3 * i + 0], _vY[3 * i + 0], _vZ[3 * i + 0]);

		_vX[3 * i + 1] = v2 < mesh.vCount ? mesh.vX[v2] : 0.0;
		_vY[3 * i + 1] = v2 < mesh.vCount ? mesh.vY[v2] : 0.0;
		_vZ[3 * i + 1] = v2 < mesh.vCount ? mesh.vZ[v2] : 0.0;
		_boundBox.add(_vX[3 * i + 1], _vY[3 * i + 1], _vZ[3 * i + 1]);

		_vX[3 * i + 2] = v3 < mesh.vCount ? mesh.vX[v3] : 0.0;
		_vY[3 * i + 2] = v3 < mesh.vCount ? mesh.vY[v3] : 0.0;
		_vZ[3 * i + 2] = v3 < mesh.vCount ? mesh.vZ[v3] : 0.0;
		_boundBox.add(_vX[3 * i + 2], _vY[3 * i + 2], _vZ[3 * i + 2]);

		const uint32 t1 = mesh.tIA[i];
		const uint32 t2 = mesh.tIB[i];
		const uint32 t3 = mesh.tIC[i];

		// Texture coordinates
		for (uint32 t = 0; t < textureCount; t++) {
			_tX[3 * textureCount * i + 3 * t + 0] = t1 < mesh.tCount ? mesh.tX[t1] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 0] = t1 < mesh.tCount ? mesh.tY[t1] : 0.0;

			_tX[3 * textureCount * i + 3 * t + 1] = t2 < mesh.tCount ? mesh.tX[t2] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 1] = t2 < mesh.tCount ? mesh.tY[t2] : 0.0;

			_tX[3 * textureCount * i + 3 * t + 2] = t3 < mesh.tCount ? mesh.tX[t3] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 2] = t3 < mesh.tCount ? mesh.tY[t3] : 0.0;
		}

	}

	createCenter();
}

} // End of namespace Aurora

} // End of namespace Graphics
