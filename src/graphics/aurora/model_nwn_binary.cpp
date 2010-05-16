/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model_nwn_binary.cpp
 *  Loading Binary MDL files found in Neverwinter Nights.
 */

// Disable the "unused variable" warnings while most stuff is still stubbed
#pragma GCC diagnostic ignored "-Wunused-variable"

#include "common/util.h"
#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"
#include "common/streamtokenizer.h"

#include "events/requests.h"

#include "graphics/aurora/model_nwn_binary.h"

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

Model_NWN_Binary::ParserContext::ParserContext(Common::SeekableReadStream &stream) :
	mdl(&stream), state(0), node(0), mesh(0) {
}

Model_NWN_Binary::ParserContext::~ParserContext() {
	delete mesh;
	delete node;
	delete state;
}


Model_NWN_Binary::Model_NWN_Binary(Common::SeekableReadStream &mdl, ModelType type) : Model(type) {
	load(mdl);
	setState();

	createStateNameList();

	_nodeMap.clear();

	RequestMan.sync();
}

Model_NWN_Binary::~Model_NWN_Binary() {
}

bool Model_NWN_Binary::isBinary(Common::SeekableReadStream &mdl) {
	mdl.seek(0);

	return mdl.readUint32LE() == 0;
}

void Model_NWN_Binary::load(Common::SeekableReadStream &mdl) {
	ParserContext ctx(mdl);

	ctx.mdl->seek(4);

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

	uint32 animStart, animCount;
	readArray(*ctx.mdl, animStart, animCount);

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

	ctx.hasPosition    = false;
	ctx.hasOrientation = false;
	ctx.state = new State;

	readNode(ctx, nodeHeadPointer + ctx.offModelData, 0, true);

	_states.insert(std::make_pair(ctx.state->name, ctx.state));
	_currentState = ctx.state;
	ctx.state = 0;

	std::vector<uint32> animOffsets;
	readArrayOffsets(*ctx.mdl, animStart + ctx.offModelData, animCount, animOffsets);

	for (std::vector<uint32>::const_iterator offset = animOffsets.begin(); offset != animOffsets.end(); ++offset) {
		ctx.state = new State;

		readAnimGeometry(ctx, *offset + ctx.offModelData);

		_states.insert(std::make_pair(ctx.state->name, ctx.state));
		ctx.state = 0;
	}
}

void Model_NWN_Binary::readNode(ParserContext &ctx, uint32 offset, Node *parent, bool rootState) {
	ctx.mdl->seekTo(offset);

	ctx.mesh = new Mesh;
	ctx.node = new Node;

	// Correctly anchor the node
	if (parent) {
		ctx.node->parent = parent;
		parent->children.push_back(ctx.node);
	} else
		ctx.state->nodes.push_back(ctx.node);

	ctx.mdl->skip(24); // Function pointers

	uint32 inheritColorFlag = ctx.mdl->readUint32LE();
	uint32 partNumber       = ctx.mdl->readUint32LE();

	ctx.node->name.readASCII(*ctx.mdl, 32);

	if (rootState)
		_nodeMap.insert(std::make_pair(ctx.node->name, ctx.node));

	ctx.mdl->skip(8); // Parent pointers

	uint32 childrenStart, childrenCount;
	readArray(*ctx.mdl, childrenStart, childrenCount);

	std::vector<uint32> children;
	readArrayOffsets(*ctx.mdl, childrenStart + ctx.offModelData, childrenCount, children);

	uint32 controllerKeyStart, controllerKeyCount;
	readArray(*ctx.mdl, controllerKeyStart, controllerKeyCount);

	uint32 controllerDataStart, controllerDataCount;
	readArray(*ctx.mdl, controllerDataStart, controllerDataCount);

	std::vector<float> controllerData;
	readArrayFloats(*ctx.mdl, controllerDataStart + ctx.offModelData, controllerDataCount, controllerData);

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

	readNodeControllers(ctx, controllerKeyStart + ctx.offModelData, controllerKeyCount, controllerData);

	// If the node has no own position controller, inherit the position from the root state
	if (!rootState && !ctx.hasPosition) {
		NodeMap::const_iterator node = _nodeMap.find(ctx.node->name);
		if (node != _nodeMap.end()) {
			ctx.node->position[0] = node->second->position[0];
			ctx.node->position[1] = node->second->position[1];
			ctx.node->position[2] = node->second->position[2];
		}
	}

	// If the node has no own orientation controller, inherit the orientation from the root state
	if (!rootState && !ctx.hasOrientation) {
		NodeMap::const_iterator node = _nodeMap.find(ctx.node->name);
		if (node != _nodeMap.end()) {
			ctx.node->orientation[0] = node->second->orientation[0];
			ctx.node->orientation[1] = node->second->orientation[1];
			ctx.node->orientation[2] = node->second->orientation[2];
			ctx.node->orientation[3] = node->second->orientation[3];
		}
	}

	processMesh(*ctx.mesh, *ctx.node);
	delete ctx.mesh;
	ctx.mesh = 0;

	parent = ctx.node;

	_nodes.push_back(ctx.node);
	ctx.node = 0;

	for (std::vector<uint32>::const_iterator child = children.begin(); child != children.end(); ++child) {
		ctx.hasPosition    = false;
		ctx.hasOrientation = false;
		readNode(ctx, *child + ctx.offModelData, parent, rootState);
	}
}

void Model_NWN_Binary::readMesh(ParserContext &ctx) {
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
		warning("Model_NWN_Binary::readMesh(): textureCount == %d (\"%s\", \"%s\", \"%s\", \"%s\")",
				textureCount, textures[0].c_str(), textures[1].c_str(), textures[2].c_str(), textures[3].c_str());

	ctx.mesh->texture = textures[0];

	uint32 endPos = ctx.mdl->pos();

	// Read vertices
	if (vertexOffset != 0xFFFFFFFF) {
		ctx.mdl->seekTo(vertexOffset + ctx.offRawData);

		ctx.mesh->verts.resize(3 * vertexCount);

		for (int i = 0; i < (3 * vertexCount); i++)
			ctx.mesh->verts[i] = ctx.mdl->readIEEEFloatLE();
	}

	// Read texture vertices
	ctx.mesh->tverts.resize(3 * vertexCount);
	if (texture0VertexOffset != 0xFFFFFFFF) {

		ctx.mdl->seekTo(texture0VertexOffset + ctx.offRawData);

		for (int i = 0; i < vertexCount; i++) {
			ctx.mesh->tverts[3 * i + 0] = ctx.mdl->readIEEEFloatLE();
			ctx.mesh->tverts[3 * i + 1] = ctx.mdl->readIEEEFloatLE();
			ctx.mesh->tverts[3 * i + 2] = 0;
		}
	} else
		for (int i = 0; i < (3 * vertexCount); i++)
			ctx.mesh->tverts[i] = 0;

	// Read faces
	ctx.mesh->faces.resize(facesCount);
	if ((facesStart != 0) && (facesCount > 0)) {
		ctx.mdl->seekTo(facesStart + ctx.offModelData);

		for (uint32 i = 0; i < facesCount; i++) {
			ctx.mdl->skip(3 * 4); // Normal
			ctx.mdl->skip(    4); // Distance
			ctx.mdl->skip(    4); // ID
			ctx.mdl->skip(3 * 2); // Adjacent face number

			MeshFace &face = ctx.mesh->faces[i];

			face.tverts[0] = face.verts[0] = ctx.mdl->readUint16LE();
			face.tverts[1] = face.verts[1] = ctx.mdl->readUint16LE();
			face.tverts[2] = face.verts[2] = ctx.mdl->readUint16LE();
		}
	}

	ctx.mdl->seekTo(endPos);
}

void Model_NWN_Binary::readAnim(ParserContext &ctx) {
	float samplePeriod = ctx.mdl->readIEEEFloatLE();

	uint32 a0S, a0C;
	readArray(*ctx.mdl, a0S, a0C);

	uint32 a1S, a1C;
	readArray(*ctx.mdl, a1S, a1C);

	uint32 a2S, a2C;
	readArray(*ctx.mdl, a2S, a2C);

	uint32 offAnimVertices        = ctx.mdl->readUint32LE();
	uint32 offAnimTextureVertices = ctx.mdl->readUint32LE();

	uint32 verticesCount        = ctx.mdl->readUint32LE();
	uint32 textureVerticesCount = ctx.mdl->readUint32LE();
}

void Model_NWN_Binary::readNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data) {
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
			throw Common::Exception("TODO: Model_NWN_Binary::readNodeControllers(): columnCount == 0xFFFF");

		if        (type == kControllerTypePosition) {
			if (columnCount != 3)
				throw Common::Exception("Position controller with %d values", columnCount);

			// Starting position
			if (data[timeIndex + 0] == 0.0) {
				ctx.node->position[0] = data[dataIndex + 0];
				ctx.node->position[1] = data[dataIndex + 1];
				ctx.node->position[2] = data[dataIndex + 2];

				ctx.hasPosition = true;
			}

		} else if (type == kControllerTypeOrientation) {
			if (columnCount != 4)
				throw Common::Exception("Orientation controller with %d values", columnCount);

			// Starting orientation
			if (data[timeIndex + 0] == 0.0) {
				ctx.node->orientation[0] = data[dataIndex + 0];
				ctx.node->orientation[1] = data[dataIndex + 1];
				ctx.node->orientation[2] = data[dataIndex + 2];
				ctx.node->orientation[3] = Common::rad2deg(acos(data[dataIndex + 3]) * 2.0);

				ctx.hasOrientation = true;
			}

		} else if (type == kControllerTypeAlpha) {
			if (columnCount != 1)
				throw Common::Exception("Alpha controller with %d values", columnCount);

			// Starting alpha
			if (data[timeIndex + 0] == 0.0)
				if (data[dataIndex + 0] == 0.0)
					// TODO: Just disabled rendering if alpha == 0.0 for now
					ctx.node->render = false;
		}

	}

	ctx.mdl->seekTo(pos);
}

void Model_NWN_Binary::readAnimGeometry(ParserContext &ctx, uint32 offset) {
	ctx.mdl->seekTo(offset);

	ctx.mdl->skip(8); // Function pointers

	ctx.state->name.readASCII(*ctx.mdl, 64);

	uint32 nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32 nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24 + 4); // Unknown + Reference count

	uint8 type = ctx.mdl->readByte();

	ctx.mdl->skip(3); // Padding

	float animLength = ctx.mdl->readIEEEFloatLE();
	float transTime  = ctx.mdl->readIEEEFloatLE();

	Common::UString animRoot;
	animRoot.readASCII(*ctx.mdl, 64);

	uint32 eventStart, eventCount;
	readArray(*ctx.mdl, eventStart, eventCount);

	// Associated events
	ctx.mdl->seekTo(eventStart + ctx.offModelData);
	for (uint32 i = 0; i < eventCount; i++) {
		float after = ctx.mdl->readIEEEFloatLE();

		Common::UString eventName;
		eventName.readASCII(*ctx.mdl, 32);
	}

	ctx.hasPosition    = false;
	ctx.hasOrientation = false;
	readNode(ctx, nodeHeadPointer + ctx.offModelData, 0, false);
}

} // End of namespace Aurora

} // End of namespace Graphics
