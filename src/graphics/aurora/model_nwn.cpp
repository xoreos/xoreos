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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/aurora/model_nwn.cpp
 *  Loading models found in Neverwinter Nights.
 */

// Disable the "unused variable" warnings while most stuff is still stubbed
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

#include <boost/unordered_set.hpp>

#include <OgreSceneNode.h>
#include <OgreMesh.h>
#include <OgreEntity.h>
#include <OgreRoot.h>
#include <OgreVector3.h>

#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"
#include "common/streamtokenizer.h"
#include "common/debug.h"
#include "common/uuid.h"

#include "aurora/resman.h"

#include "graphics/util.h"
#include "graphics/materialman.h"

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

struct Normal {
	uint16 vi;    ///< ID of vertex this normal belongs to.
	float xyz[3]; ///< Vertex normal.
};

struct FaceVert {
	uint32 p;        ///< Position coordinate indices.
	uint32 t;        ///< Texture coordinate indices.
	uint32 i;        ///< Unique vertex ID.
	Ogre::Vector3 n; ///< Normal vector.
};

static bool operator==(const Normal &a, const Normal &b) {
	return a.vi == b.vi;
}

static std::size_t hash_value(const Normal &b) {
	boost::hash<uint16> hasher;
	return hasher(b.vi);
}

static bool fuzzyEqual(const float a[3], const float b[3]) {
	return ABS(a[0] - b[0]) < 1E-4 &&
	       ABS(a[1] - b[1]) < 1E-4 &&
	       ABS(a[2] - b[2]) < 1E-4;
}

static bool operator==(const FaceVert &a, const FaceVert &b) {
	return (a.p == b.p) && (a.t == b.t) && a.n.positionEquals(b.n, 1E-4);
}

static std::size_t hash_value(const FaceVert &b) {
	std::size_t seed = 0;
	boost::hash_combine(seed, b.p);
	boost::hash_combine(seed, b.t);
	boost::hash_combine(seed, uint32(b.n[0] * 1E4));
	boost::hash_combine(seed, uint32(b.n[1] * 1E4));
	boost::hash_combine(seed, uint32(b.n[2] * 1E4));
	return seed;
}


Model_NWN::ParserContext::ParserContext(const Common::UString &name,
                                        const Common::UString &t) :
	mdl(0), texture(t), state(0), nodeEntity(0), hasPosition(false), hasOrientation(false) {

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
}

void Model_NWN::ParserContext::newState() {
	state = 0;

	newNode();
}

void Model_NWN::ParserContext::newNode() {
	material.reset();

	nodeEntity = 0;

	hasPosition    = false;
	hasOrientation = false;
}


Model_NWN::MeshASCII::MeshASCII() : vCount(0), tCount(0), faceCount(0) {
}


Model_NWN::Model_NWN(const Common::UString &name, const Common::UString &texture) :
		_fileName(name) {

	ParserContext ctx(name, texture);

	if (ctx.isASCII)
		loadASCII(ctx);
	else
		loadBinary(ctx);

	setState(_states[""]);
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

	_animationScale = ctx.mdl->readIEEEFloatLE();

	_superModelName.readFixedASCII(*ctx.mdl, 64);

	_rootNode = getOgreSceneManager().getRootSceneNode()->createChildSceneNode(Common::generateIDRandomString().c_str());
	_rootNode->setVisible(false);

	_states.insert(std::make_pair("", new State));
	ctx.state = _states[""];

	ctx.mdl->seek(ctx.offModelData + nodeHeadPointer);
	loadBinaryNode(ctx, _rootNode);

	std::vector<uint32> animOffsets;
	readArray(*ctx.mdl, ctx.offModelData + animOffset, animCount, animOffsets);

	for (std::vector<uint32>::const_iterator offset = animOffsets.begin(); offset != animOffsets.end(); ++offset) {
		ctx.newState();

		readBinaryAnim(ctx, ctx.offModelData + *offset);
	}
}

void Model_NWN::loadASCII(ParserContext &ctx) {
	_rootNode = getOgreSceneManager().getRootSceneNode()->createChildSceneNode(Common::generateIDRandomString().c_str());
	_rootNode->setVisible(false);

	_states.insert(std::make_pair("", new State));
	ctx.state = _states[""];

	ctx.mdl->seek(0);

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
				Common::UString superModelName = line[2];

		} else if (line[0] == "beginmodelgeom") {
			if (line[1] != _name)
				warning("Model_NWN_ASCII::load(): beginmodelgeom: \"%s\" != \"%s\"",
				        line[1].c_str(), _name.c_str());
		} else if (line[0] == "setanimationscale") {
			float animationScale;
			line[1].parse(animationScale);
		} else if (line[0] == "node") {

			ctx.newNode();
			loadASCIINode(ctx, _rootNode, line[1], line[2]);

		} else if (line[0] == "newanim") {
			skipASCIIAnim(ctx);
		} else if (line[0] == "donemodel") {
			break;
		} else
			; // warning("Unknown MDL command \"%s\"", line[0].c_str());
	}

	// TODO: Animations / states
}

void Model_NWN::loadBinaryNode(ParserContext &ctx, Ogre::SceneNode *parent) {
	ctx.mdl->skip(24); // Function pointers

	uint32 inheritColorFlag = ctx.mdl->readUint32LE();
	uint32 partNumber       = ctx.mdl->readUint32LE();

	Common::UString name;
	name.readFixedASCII(*ctx.mdl, 32);

	debugC(5, kDebugGraphics, "Node \"%s\" in state \"%s\"", name.c_str(), ctx.state->name.c_str());

	createNode(ctx.nodeEntity, ctx.state, name, parent);

	ctx.mdl->skip(8); // Parent pointers

	uint32 childrenOffset, childrenCount;
	readArrayDef(*ctx.mdl, childrenOffset, childrenCount);

	std::vector<uint32> children;
	readArray(*ctx.mdl, ctx.offModelData + childrenOffset, childrenCount, children);

	uint32 controllerKeyOffset, controllerKeyCount;
	readArrayDef(*ctx.mdl, controllerKeyOffset, controllerKeyCount);

	uint32 controllerDataOffset, controllerDataCount;
	readArrayDef(*ctx.mdl, controllerDataOffset, controllerDataCount);

	std::vector<float> controllerData;
	readArray(*ctx.mdl, ctx.offModelData + controllerDataOffset, controllerDataCount, controllerData);

	readBinaryNodeControllers(ctx, ctx.offModelData + controllerKeyOffset,
	                          controllerKeyCount, controllerData);

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
		readBinaryMesh(ctx);
	}

	if (flags & kNodeFlagHasSkin) {
		// TODO: Skin
		ctx.mdl->skip(0x64);
	}

	if (flags & kNodeFlagHasAnim) {
		readBinaryNodeAnim(ctx);
	}

	if (flags & kNodeFlagHasDangly) {
		// TODO: Dangly
		ctx.mdl->skip(0x18);
	}

	if (flags & kNodeFlagHasAABB) {
		// TODO: AABB
		ctx.mdl->skip(0x4);
	}

	nodeInherit(ctx, name);

	Ogre::SceneNode *newParent = ctx.nodeEntity->node;
	for (std::vector<uint32>::const_iterator child = children.begin(); child != children.end(); ++child) {
		ctx.newNode();

		ctx.mdl->seek(ctx.offModelData + *child);
		loadBinaryNode(ctx, newParent);
	}
}

void Model_NWN::nodeInherit(ParserContext &ctx, const Common::UString &name) {
	if (ctx.nodeEntity->entity && ctx.hasPosition && ctx.hasOrientation)
		return;

	NodeEntities::iterator rootNodeEntity = _states[""]->nodeEntities.find(name);
	if (rootNodeEntity == _states[""]->nodeEntities.end())
		return;

	// If the node has no own geometry, position or orientation, inherit it from the root state

	if (!ctx.nodeEntity->entity) {
		ctx.nodeEntity->entity = rootNodeEntity->second.entity;
		if (rootNodeEntity->second.dontRender)
			ctx.nodeEntity->dontRender = true;
	}
	if (!ctx.hasPosition)
		memcpy(ctx.nodeEntity->position, rootNodeEntity->second.position, 3 * sizeof(float));
	if (!ctx.hasOrientation)
		memcpy(ctx.nodeEntity->orientation, rootNodeEntity->second.orientation, 4 * sizeof(float));

	ctx.nodeEntity->inheritedPosition = !ctx.hasPosition;
}

void Model_NWN::readBinaryMesh(ParserContext &ctx) {
	ctx.mdl->skip(8); // Function pointers

	uint32 facesOffset, facesCount;
	readArrayDef(*ctx.mdl, facesOffset, facesCount);

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

	ctx.material.ambient[0] = ctx.mdl->readIEEEFloatLE();
	ctx.material.ambient[1] = ctx.mdl->readIEEEFloatLE();
	ctx.material.ambient[2] = ctx.mdl->readIEEEFloatLE();

	ctx.material.diffuse[0] = ctx.mdl->readIEEEFloatLE();
	ctx.material.diffuse[1] = ctx.mdl->readIEEEFloatLE();
	ctx.material.diffuse[2] = ctx.mdl->readIEEEFloatLE();

	ctx.material.specular[0] = ctx.mdl->readIEEEFloatLE();
	ctx.material.specular[1] = ctx.mdl->readIEEEFloatLE();
	ctx.material.specular[2] = ctx.mdl->readIEEEFloatLE();

	ctx.material.shininess = ctx.mdl->readIEEEFloatLE();

	bool shadow, beaming, render;
	shadow  = ctx.mdl->readUint32LE() == 1;
	beaming = ctx.mdl->readUint32LE() == 1;
	render  = ctx.mdl->readUint32LE() == 1;

	if (!render)
		ctx.nodeEntity->dontRender = true;

	bool transparencyHint = ctx.mdl->readUint32LE() == 1;
	ctx.material.transparency = transparencyHint ? kTransparencyHintTransparent : kTransparencyHintOpaque;

	ctx.mdl->skip(4); // Unknown

	ctx.material.textures.resize(4);
	for (int i = 0; i < 4; i++) {
		ctx.material.textures[i].readFixedASCII(*ctx.mdl, 64);
		if (ctx.material.textures[i] == "NULL")
			ctx.material.textures[i].clear();
	}

	bool tilefade;
	tilefade = ctx.mdl->readUint32LE();

	ctx.mdl->skip(12); // Vertex indices
	ctx.mdl->skip(12); // Left over faces

	ctx.mdl->skip(12); // Vertex indices counts
	ctx.mdl->skip(12); // Vertex indices offsets

	ctx.mdl->skip(8); // Unknown

	byte triangleMode = ctx.mdl->readByte(); // 3 - Triangle, 4 - TriStrip

	ctx.mdl->skip(3 + 4); // Padding + Unknown

	uint32 vertexOffset = ctx.mdl->readUint32LE();
	uint16 vertexCount  = ctx.mdl->readUint16LE();
	uint16 textureCount = ctx.mdl->readUint16LE();

	uint32 textureVertexOffset[4];
	textureVertexOffset[0] = ctx.mdl->readUint32LE();
	textureVertexOffset[1] = ctx.mdl->readUint32LE();
	textureVertexOffset[2] = ctx.mdl->readUint32LE();
	textureVertexOffset[3] = ctx.mdl->readUint32LE();

	uint32 normalOffset = ctx.mdl->readUint32LE(); // Vertex normals
	uint32 colorOffset = ctx.mdl->readUint32LE(); // Vertex RGBA colors

	uint32 textureAnimOffset[6]; // Texture animation data
	for (uint32 i = 0; i < 6; i++)
		textureAnimOffset[i] = ctx.mdl->readUint32LE();

	bool lightMapped = ctx.mdl->readByte() == 1;

	bool rotatetexture;
	rotatetexture = ctx.mdl->readByte() == 1;

	ctx.mdl->skip(2); // Padding

	ctx.mdl->skip(4); // Normal sum / 2

	ctx.mdl->skip(4); // Unknown

	if ((vertexCount == 0) || (facesCount == 0) || (facesOffset == 0))
		return;

	if (textureCount > 4)
		warning("Model_NWN::readBinaryMesh(): textureCount > 4 (%d)", textureCount);

	if ((textureCount > 0) && !ctx.texture.empty())
		ctx.material.textures[0] = ctx.texture;

	ctx.material.textures.resize(textureCount);

	ctx.material.trimTextures();
	textureCount = ctx.material.textures.size();

	if (textureCount == 0)
		ctx.nodeEntity->dontRender = true;

	Ogre::MaterialPtr material = MaterialMan.get(ctx.material);

	uint32 endPos = ctx.mdl->pos();


	// Read faces

	// NWN stores one normal per face. Converting to one normal per vertex
	// by duplicating vertex data for face vertices with multiple normals

	VertexDeclaration vertexDecl(facesCount, vertexCount, textureCount);

	std::vector<Normal> normalsNew;
	boost::unordered_set<Normal> normals;

	uint16 *f = &vertexDecl.bufferIndices[0];
	ctx.mdl->seekTo(ctx.offModelData + facesOffset);
	for (uint32 i = 0; i < facesCount; i++) {
		Normal n;

		// Face normal
		n.xyz[0] = ctx.mdl->readIEEEFloatLE();
		n.xyz[1] = ctx.mdl->readIEEEFloatLE();
		n.xyz[2] = ctx.mdl->readIEEEFloatLE();

		ctx.mdl->skip(    4); // Plane distance
		ctx.mdl->skip(    4); // Surface ID / smoothing group ??
		ctx.mdl->skip(3 * 2); // Adjacent face number or -1

		// Face indices
		for (uint32 j = 0; j < 3; j++) {
			n.vi = ctx.mdl->readUint16LE();
			assert(n.vi <= vertexCount);

			// check if we have a normal for this vertex already
			std::pair<boost::unordered_set<Normal>::iterator, bool> it = normals.insert(n);
			if (!it.second && !fuzzyEqual(n.xyz, it.first->xyz)) {
				normalsNew.push_back(n);
				n.vi = vertexDecl.vertices++;
			}

			*f++ = n.vi;
		}
	}

	// Read vertex data

	vertexDecl.resize();

	assert (vertexOffset != 0xFFFFFFFF);
	ctx.mdl->seekTo(ctx.offRawData + vertexOffset);

	// Read vertex coordinates
	for (uint32 i = 0; i < vertexCount; i++) {
		float *v = &vertexDecl.bufferVerticesNormals[6 * i + 0];

		*v++ = ctx.mdl->readIEEEFloatLE();
		*v++ = ctx.mdl->readIEEEFloatLE();
		*v++ = ctx.mdl->readIEEEFloatLE();
	}

	// Duplicate positions for unique normals
	for (uint32 i = 0; i < normalsNew.size(); i++) {
		float *vDst = &vertexDecl.bufferVerticesNormals[6 * vertexCount + 6 * i + 0];
		float *vSrc = &vertexDecl.bufferVerticesNormals[normalsNew[i].vi * 6 + 0];

		*vDst++ = *vSrc++;
		*vDst++ = *vSrc++;
		*vDst++ = *vSrc++;
	}

	// Read vertex normals
	for (boost::unordered_set<Normal>::iterator i = normals.begin(); i != normals.end(); ++i) {
		float *n = &vertexDecl.bufferVerticesNormals[6 * i->vi + 3];

		*n++ = i->xyz[0];
		*n++ = i->xyz[1];
		*n++ = i->xyz[2];
	}

	// Additional unique vertex normals
	for (uint32 i = 0; i < normalsNew.size(); i++) {
		float *n = &vertexDecl.bufferVerticesNormals[6 * vertexCount + 6 * i + 3];

		*n++ = normalsNew[i].xyz[0];
		*n++ = normalsNew[i].xyz[1];
		*n++ = normalsNew[i].xyz[2];
	}

	for (uint16 t = 0; t < textureCount; t++) {
		bool hasTexture = textureVertexOffset[t] != 0xFFFFFFFF;
		if (hasTexture)
			ctx.mdl->seekTo(ctx.offRawData + textureVertexOffset[t]);

	// Read texture coordinates
		for (uint32 i = 0; i < vertexCount; i++) {
			float *v = &vertexDecl.bufferTexCoords[2 * t * vertexDecl.vertices + 2 * i];

			*v++ = hasTexture ? ctx.mdl->readIEEEFloatLE() : 0.0;
			*v++ = hasTexture ? ctx.mdl->readIEEEFloatLE() : 0.0;
		}

		// Duplicate tcoords for unique normals
		for (uint32 i = 0; i < normalsNew.size(); i++) {
			float *vDst = &vertexDecl.bufferTexCoords[2 * t * vertexDecl.vertices + 2 * (vertexCount + i)];
			float *vSrc = &vertexDecl.bufferTexCoords[2 * t * vertexDecl.vertices + 2 * normalsNew[i].vi];

			*vDst++ = *vSrc++;
			*vDst++ = *vSrc++;
		}
	}

	// Create an entity with the mesh defined by the vertex declaration and the material
	Ogre::Entity *entity = createEntity(vertexDecl, material);

	// Add the entity to our lists

	_entities.push_back(entity);
	ctx.nodeEntity->entity = entity;

	ctx.mdl->seekTo(endPos);
}

void Model_NWN::readBinaryNodeAnim(ParserContext &ctx) {
	float samplePeriod = ctx.mdl->readIEEEFloatLE();

	uint32 a0S, a0C;
	readArrayDef(*ctx.mdl, a0S, a0C);

	uint32 a1S, a1C;
	readArrayDef(*ctx.mdl, a1S, a1C);

	uint32 a2S, a2C;
	readArrayDef(*ctx.mdl, a2S, a2C);

	uint32 offAnimVertices        = ctx.mdl->readUint32LE();
	uint32 offAnimTextureVertices = ctx.mdl->readUint32LE();

	uint32 verticesCount        = ctx.mdl->readUint32LE();
	uint32 textureVerticesCount = ctx.mdl->readUint32LE();
}

void Model_NWN::readBinaryNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data) {
	uint32 pos = ctx.mdl->seekTo(offset);

	// TODO: readNodeControllers: Implement this properly :P

	for (uint32 i = 0; i < count; i++) {
		Ogre::NodeAnimationTrack *track = 0;

		uint32 type        = ctx.mdl->readUint32LE();
		uint16 rowCount    = ctx.mdl->readUint16LE();
		uint16 timeIndex   = ctx.mdl->readUint16LE();
		uint16 dataIndex   = ctx.mdl->readUint16LE();
		uint8  columnCount = ctx.mdl->readByte();
		ctx.mdl->skip(1);

		if (rowCount == 0xFFFF)
			// TODO: Controller row count = 0xFFFF
			continue;

		if (type == kControllerTypePosition) {
			if (columnCount != 3)
				throw Common::Exception("Position controller with %d values", columnCount);

			if (ctx.state->animation)
				track = ctx.state->animation->createNodeTrack(ctx.state->animation->getNumNodeTracks(), ctx.nodeEntity->node);

			for (int r = 0; r < rowCount; r++) {
				float pT = data[timeIndex + r];
				float pX = data[dataIndex + (r * columnCount) + 0];
				float pY = data[dataIndex + (r * columnCount) + 1];
				float pZ = data[dataIndex + (r * columnCount) + 2];

				if (track)
					track->createNodeKeyFrame(pT)->setTranslate(Ogre::Vector3(pX, pY, pZ));

				// Starting position
				if (pT == 0.0) {
					ctx.hasPosition = true;

					ctx.nodeEntity->position[0] = pX;
					ctx.nodeEntity->position[1] = pY;
					ctx.nodeEntity->position[2] = pZ;
				}
			}

		} else if (type == kControllerTypeOrientation) {
			if (columnCount != 4)
				throw Common::Exception("Orientation controller with %d values", columnCount);

			if (ctx.state->animation)
				track = ctx.state->animation->createNodeTrack(ctx.state->animation->getNumNodeTracks(), ctx.nodeEntity->node);

			for (int r = 0; r < rowCount; r++) {
				float qT = data[timeIndex + r];
				float qX = data[dataIndex + (r * columnCount) + 0];
				float qY = data[dataIndex + (r * columnCount) + 1];
				float qZ = data[dataIndex + (r * columnCount) + 2];
				float qQ = data[dataIndex + (r * columnCount) + 3];

				if (track)
					track->createNodeKeyFrame(qT)->setRotation(Ogre::Quaternion(qQ, qX, qY, qZ));

				// Starting orientation
				if (qT == 0.0) {
					ctx.hasOrientation = true;

					ctx.nodeEntity->orientation[0] = qQ;
					ctx.nodeEntity->orientation[1] = qX;
					ctx.nodeEntity->orientation[2] = qY;
					ctx.nodeEntity->orientation[3] = qZ;
				}
			}

		} else if (type == kControllerTypeAlpha) {
			if (columnCount != 1)
				throw Common::Exception("Alpha controller with %d values", columnCount);

			// Starting alpha
			if (data[timeIndex + 0] == 0.0)
				if (data[dataIndex + 0] == 0.0)
					ctx.nodeEntity->dontRender = true;
		}

	}

	ctx.nodeEntity->node->setInitialState();
	ctx.mdl->seekTo(pos);
}

void Model_NWN::readBinaryAnim(ParserContext &ctx, uint32 offset) {
	ctx.mdl->seekTo(offset);

	ctx.mdl->skip(8); // Function pointers

	Common::UString name;
	name.readFixedASCII(*ctx.mdl, 64);

	_states.insert(std::make_pair(name, new State(name)));
	ctx.state = _states[name];

	uint32 nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32 nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24 + 4); // Unknown + Reference count

	uint8 type = ctx.mdl->readByte();

	ctx.mdl->skip(3); // Padding

	float animLength = ctx.mdl->readIEEEFloatLE();
	float transTime  = ctx.mdl->readIEEEFloatLE();

	ctx.state->animation = getOgreSceneManager().createAnimation(Common::generateIDRandomString().c_str(), animLength);

	Common::UString animRoot;
	animRoot.readFixedASCII(*ctx.mdl, 64);

	uint32 eventOffset, eventCount;
	readArrayDef(*ctx.mdl, eventOffset, eventCount);

	ctx.mdl->seek(ctx.offModelData + nodeHeadPointer);
	loadBinaryNode(ctx, _rootNode);

	ctx.state->animationState = getOgreSceneManager().createAnimationState(ctx.state->animation->getName());
	ctx.state->animationState->setEnabled(false);
}

void Model_NWN::skipASCIIAnim(ParserContext &ctx) {
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
		throw Common::Exception("Model_NWN::skipASCIIAnim(): anim without doneanim");
}

void Model_NWN::loadASCIINode(ParserContext &ctx, Ogre::SceneNode *parent,
                              const Common::UString &type, const Common::UString &name) {

	bool end      = false;
	bool skipNode = false;

	debugC(5, kDebugGraphics, "Node \"%s\" in state \"%s\"", name.c_str(),
	       ctx.state->name.c_str());

	bool render = false;
	if ((type == "trimesh") || (type == "danglymesh") || (type == "skin"))
		render = true;

	if ((type == "emitter") || (type == "reference") || (type == "aabb")) {
		// warning("TODO: Node type %s", type.c_str());
		skipNode = true;
	}

	bool dangly = false;
	if (type == "danglymesh")
		dangly = true;

	MeshASCII mesh;
	Common::UString parentName;

	float position[3]     = { 0.0, 0.0, 0.0 };
	float orientation[4]  = { 1.0, 0.0, 0.0, 0.0 };

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
			parentName = line[1];
		} else if (line[0] == "position") {
			ctx.hasPosition = true;

			readASCIIFloats(line, position, 3, 1);
		} else if (line[0] == "orientation") {
			ctx.hasOrientation = true;

			readASCIIFloats(line, orientation, 4, 1);
		} else if (line[0] == "render") {
			line[1].parse(render);
		} else if (line[0] == "transparencyhint") {
			bool transparencyHint;
			line[1].parse(transparencyHint);

			ctx.material.transparency = transparencyHint ? kTransparencyHintTransparent : kTransparencyHintOpaque;
		} else if (line[0] == "danglymesh") {
			line[1].parse(dangly);
		} else if (line[0] == "constraints") {
			uint32 n;

			line[1].parse(n);
			readASCIIConstraints(ctx, n);
		} else if (line[0] == "weights") {
			uint32 n;

			line[1].parse(n);
			readASCIIWeights(ctx, n);
		} else if (line[0] == "bitmap") {
			ctx.material.textures.push_back(line[1]);
		} else if (line[0] == "verts") {
			line[1].parse(mesh.vCount);

			readASCIIVCoords(ctx, mesh);
		} else if (line[0] == "tverts") {
			if (mesh.tCount != 0)
				warning("Model_NWN::loadASCIINode(): Node \"%s\": Multiple texture coordinates", name.c_str());

			line[1].parse(mesh.tCount);

			readASCIITCoords(ctx, mesh);
		} else if (line[0] == "faces") {
			line[1].parse(mesh.faceCount);

			readASCIIFaces(ctx, mesh);
		} else
			; // warning("Unknown MDL node command \"%s\"", line[0].c_str());
	}

	if (!end)
		throw Common::Exception("Model_NWN::loadASCIINode(): Node \"%s\": Missing endnode", name.c_str());

	if (!ctx.material.textures.empty() && !ctx.texture.empty())
		ctx.material.textures[0] = ctx.texture;

	if (!parentName.empty() && (parentName != "NULL")) {
		NodeEntities::iterator parentNode = ctx.state->nodeEntities.find(parentName);
		if (parentNode != ctx.state->nodeEntities.end())
			parent = parentNode->second.node;
		else
			warning("Model_NWN::loadASCIINode(): Node \"%s\" has an invalid parent (\"%s\")",
			        name.c_str(), parentName.c_str());
	}

	createNode(ctx.nodeEntity, ctx.state, name, parent);

	ctx.nodeEntity->dontRender = !render;

	processASCIIMesh(ctx, mesh);

	if ((orientation[0] == 0.0) && (orientation[1] == 0.0) && (orientation[2] == 0.0) && (orientation[3] == 0.0))
		orientation[3] = 1.0;

	SWAP(orientation[3], orientation[0]);

	if (ctx.hasPosition)
		memcpy(ctx.nodeEntity->position, position, 3 * sizeof(float));
	if (ctx.hasOrientation)
		memcpy(ctx.nodeEntity->orientation, orientation, 4 * sizeof(float));

	nodeInherit(ctx, name);
}

void Model_NWN::readASCIIConstraints(ParserContext &ctx, uint32 n) {
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

void Model_NWN::readASCIIWeights(ParserContext &ctx, uint32 n) {
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

void Model_NWN::readASCIIFloats(const std::vector<Common::UString> &strings,
                                float *floats, uint32 n, uint32 start) {

	if (strings.size() < (start + n))
		throw Common::Exception("Missing tokens");

	for (uint32 i = 0; i < n; i++)
		strings[start + i].parse(floats[i]);
}

void Model_NWN::readASCIIVCoords(ParserContext &ctx, MeshASCII &mesh) {
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

void Model_NWN::readASCIITCoords(ParserContext &ctx, MeshASCII &mesh) {
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

void Model_NWN::readASCIIFaces(ParserContext &ctx, MeshASCII &mesh) {
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

void Model_NWN::processASCIIMesh(ParserContext &ctx, MeshASCII &mesh) {
	if ((mesh.vCount == 0) || (mesh.tCount == 0) || (mesh.faceCount == 0))
		return;

	// Load the material

	ctx.material.trimTextures();

	const uint32 textureCount = ctx.material.textures.size();
	if (textureCount > 1)
		warning("Model_NWN::processASCIIMesh(): textureCount == %d", textureCount);

	if (textureCount == 0)
		ctx.nodeEntity->dontRender = true;

	Ogre::MaterialPtr material = MaterialMan.get(ctx.material);

	VertexDeclaration vertexDecl(mesh.faceCount, 0, textureCount);

	// Process faces and calculate normals

	boost::unordered_set<FaceVert> vertices;
	uint16 *f = &vertexDecl.bufferIndices[0];
	for (uint16 i = 0; i < mesh.faceCount; i++) {
		const uint32 v[3] = {mesh.vIA[i], mesh.vIB[i], mesh.vIC[i]};
		const uint32 t[3] = {mesh.tIA[i], mesh.tIB[i], mesh.tIC[i]};

		// Face normal
		const Ogre::Vector3 p1(mesh.vX[v[0]], mesh.vY[v[0]], mesh.vZ[v[0]]);
		const Ogre::Vector3 p2(mesh.vX[v[1]], mesh.vY[v[1]], mesh.vZ[v[1]]);
		const Ogre::Vector3 p3(mesh.vX[v[2]], mesh.vY[v[2]], mesh.vZ[v[2]]);

		Ogre::Vector3 n = (p2 - p1).crossProduct(p3 - p2);

		n.normalise();

		for (uint32 j = 0; j < 3; j++) {
			FaceVert fv;
			fv.i = vertexDecl.vertices;
			fv.p = v[j];
			fv.t = t[j];
			fv.n = n;

			std::pair<boost::unordered_set<FaceVert>::iterator, bool> it = vertices.insert(fv);
			if (it.second)
				vertexDecl.vertices++;

			*f++ = it.first->i;
		}
	}

	vertexDecl.resize();

	// Process the vertices and texture coordinates

	for (boost::unordered_set<FaceVert>::iterator i = vertices.begin(); i != vertices.end(); ++i) {
		float *v = &vertexDecl.bufferVerticesNormals[6 * i->i];

		// Position
		*v++ = mesh.vX[i->p];
		*v++ = mesh.vY[i->p];
		*v++ = mesh.vZ[i->p];

		// Normal
		*v++ = i->n[0];
		*v++ = i->n[1];
		*v++ = i->n[2];

		// Textures
		for (uint16 t = 0; t < textureCount; t++) {
			float *vt = &vertexDecl.bufferTexCoords[2 * t * vertexDecl.vertices + 2 * i->i];

			*vt++ = ((t == 0) && (i->t < mesh.tCount)) ? mesh.tX[i->t] : 0.0;
			*vt++ = ((t == 0) && (i->t < mesh.tCount)) ? mesh.tY[i->t] : 0.0;
		}
	}

	// Create an entity with the mesh defined by the vertex declaration and the material
	Ogre::Entity *entity = createEntity(vertexDecl, material);

	// Add the entity to our lists

	_entities.push_back(entity);
	ctx.nodeEntity->entity = entity;
}

} // End of namespace Aurora

} // End of namespace Graphics
