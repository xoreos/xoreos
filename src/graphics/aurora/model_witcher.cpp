/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model_witcher.cpp
 *  Loading MDB files found in The Witcher
 */

// Disable the "unused variable" warnings while most stuff is still stubbed
#pragma GCC diagnostic ignored "-Wunused-variable"

#include "common/util.h"
#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"

#include "events/requests.h"

#include "graphics/aurora/model_witcher.h"
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
static const int kNodeFlagHasUnknown1  = 0x00000400;
static const int kNodeFlagHasUnknown2  = 0x00000800;
static const int kNodeFlagHasUnknown3  = 0x00001000;
static const int kNodeFlagHasUnknown4  = 0x00002000;
static const int kNodeFlagHasUnknown5  = 0x00004000;
static const int kNodeFlagHasUnknown6  = 0x00008000;
static const int kNodeFlagHasUnknown7  = 0x00010000;
static const int kNodeFlagHasUnknown8  = 0x00020000;

static const uint32 kControllerTypePosition    = 84;
static const uint32 kControllerTypeOrientation = 96;

namespace Graphics {

namespace Aurora {

Model_Witcher::ParserContext::ParserContext(Common::SeekableReadStream &mdbStream) :
	mdb(&mdbStream), state(0), node(0), mesh(0) {
}

Model_Witcher::ParserContext::~ParserContext() {
	delete mesh;
	delete node;
	delete state;
}


Model_Witcher::Model_Witcher(Common::SeekableReadStream &mdb, ModelType type) : Model(type) {
	load(mdb);
	setState();

	buildLists();

	RequestMan.sync();
}

Model_Witcher::~Model_Witcher() {
}

void Model_Witcher::load(Common::SeekableReadStream &mdb) {
	ParserContext ctx(mdb);

	if (ctx.mdb->readByte() != 0) {
		ctx.mdb->seek(0);

		Common::UString type;
		type.readASCII(*ctx.mdb);
		if (type.beginsWith("binarycompositemodel"))
			throw Common::Exception("TODO: binarycompositemodel");

		throw Common::Exception("Not a The Witcher MDB file");
	}

	ctx.mdb->seek(4);

	ctx.fileVersion = ctx.mdb->readUint16LE();

	ctx.mdb->skip(10);

	ctx.modelDataSize = ctx.mdb->readUint32LE();

	ctx.mdb->skip(4);

	ctx.offModelData = 32;

	if (ctx.fileVersion == 133) {
		ctx.offRawData  = ctx.mdb->readUint32LE() + ctx.offModelData;
		ctx.rawDataSize = ctx.mdb->readUint32LE();
		ctx.offTexData  = ctx.offModelData;
		ctx.texDatasize = 0;
	} else {
		ctx.offRawData  = ctx.offModelData;
		ctx.rawDataSize = 0;
		ctx.offTexData  = ctx.mdb->readUint32LE() + ctx.offModelData;
		ctx.texDatasize = ctx.mdb->readUint32LE();
	}

	ctx.mdb->skip(8);

	Common::UString name;
	name.readASCII(*ctx.mdb, 64);

	uint32 offsetRootNode = ctx.mdb->readUint32LE();

	ctx.mdb->skip(32);

	byte type = ctx.mdb->readByte();

	ctx.mdb->skip(3);

	ctx.mdb->skip(48);

	float firstLOD = ctx.mdb->readIEEEFloatLE();
	float lastLOD  = ctx.mdb->readIEEEFloatLE();

	ctx.mdb->skip(16);

	Common::UString detailMap;
	detailMap.readASCII(*ctx.mdb, 64);

	ctx.mdb->skip(4);

	_scale = ctx.mdb->readIEEEFloatLE();

	Common::UString superModel;
	superModel.readASCII(*ctx.mdb, 64);

	ctx.mdb->skip(16);

	warning("\"%s\", %d, %d, %d, %d, %f, \"%s\", \"%s\"", name.c_str(), ctx.fileVersion, ctx.modelDataSize, offsetRootNode,
			type, _scale, detailMap.c_str(), superModel.c_str());

	ctx.state = new State;

	readNode(ctx, offsetRootNode + ctx.offModelData, 0);

	_states.insert(std::make_pair(ctx.state->name, ctx.state));
	ctx.state = 0;
}

void Model_Witcher::readNode(ParserContext &ctx, uint32 offset, Node *parent) {
	ctx.mdb->seekTo(offset);

	ctx.mesh = new Mesh;
	ctx.node = new Node;

	if (parent) {
		ctx.node->parent = parent;
		parent->children.push_back(ctx.node);
	} else
		ctx.state->nodes.push_back(ctx.node);

	ctx.mdb->skip(24);

	uint32 inheritColor = ctx.mdb->readUint32LE();
	uint32 nodeNumber   = ctx.mdb->readUint32LE();

	ctx.node->name.readASCII(*ctx.mdb, 64);

	ctx.mdb->skip(8); // Parent pointers

	uint32 childrenStart, childrenCount;
	readArray(*ctx.mdb, childrenStart, childrenCount);

	std::vector<uint32> children;
	readArrayOffsets(*ctx.mdb, childrenStart + ctx.offModelData, childrenCount, children);

	uint32 controllerKeyStart, controllerKeyCount;
	readArray(*ctx.mdb, controllerKeyStart, controllerKeyCount);

	uint32 controllerDataStart, controllerDataCount;
	readArray(*ctx.mdb, controllerDataStart, controllerDataCount);

	std::vector<float> controllerData;
	readArrayFloats(*ctx.mdb, controllerDataStart + ctx.offModelData, controllerDataCount, controllerData);

	readNodeControllers(ctx, controllerKeyStart + ctx.offModelData, controllerKeyCount, controllerData);

	ctx.mdb->skip(20);

	uint32 flags = ctx.mdb->readUint32LE();
	if ((flags & 0xFFFC0000) != 0)
		throw Common::Exception("Unknown node flags %08X", flags);

	if (flags & kNodeFlagHasLight) {
		// TODO: Light
	}

	if (flags & kNodeFlagHasEmitter) {
		// TODO: Emitter
	}

	if (flags & kNodeFlagHasReference) {
		// TODO: Reference
	}

	if (flags & kNodeFlagHasMesh) {
		readMesh(ctx);
	}

	if (flags & kNodeFlagHasSkin) {
		// TODO: Skin
	}

	if (flags & kNodeFlagHasAnim) {
		// TODO: Anim
	}

	if (flags & kNodeFlagHasDangly) {
		// TODO: Dangly
	}

	if (flags & kNodeFlagHasAABB) {
		// TODO: AABB
	}

	processMesh(*ctx.mesh, *ctx.node);
	delete ctx.mesh;
	ctx.mesh = 0;

	parent = ctx.node;

	_nodes.push_back(ctx.node);
	ctx.node = 0;

	for (std::vector<uint32>::const_iterator child = children.begin(); child != children.end(); ++child)
		readNode(ctx, *child + ctx.offModelData, parent);
}

void Model_Witcher::readMesh(ParserContext &ctx) {
	ctx.mdb->skip(8);

	uint32 offMeshArrays = ctx.mdb->readUint32LE();

	ctx.mdb->skip(76);

	ctx.node->ambient [0] = ctx.mdb->readIEEEFloatLE();
	ctx.node->ambient [1] = ctx.mdb->readIEEEFloatLE();
	ctx.node->ambient [2] = ctx.mdb->readIEEEFloatLE();
	ctx.node->diffuse [0] = ctx.mdb->readIEEEFloatLE();
	ctx.node->diffuse [1] = ctx.mdb->readIEEEFloatLE();
	ctx.node->diffuse [2] = ctx.mdb->readIEEEFloatLE();
	ctx.node->specular[0] = ctx.mdb->readIEEEFloatLE();
	ctx.node->specular[1] = ctx.mdb->readIEEEFloatLE();
	ctx.node->specular[2] = ctx.mdb->readIEEEFloatLE();

	ctx.node->shininess = ctx.mdb->readIEEEFloatLE();

	ctx.mdb->skip(20);

	Common::UString texture[4];
	texture[0].readASCII(*ctx.mdb, 64);
	texture[1].readASCII(*ctx.mdb, 64);
	texture[2].readASCII(*ctx.mdb, 64);
	texture[3].readASCII(*ctx.mdb, 64);

	ctx.mdb->skip(20);

	uint32 fourCC = ctx.mdb->readUint32BE();

	ctx.mdb->skip(8);

	float coronaCenterX = ctx.mdb->readIEEEFloatLE();

	ctx.mdb->skip(8);

	float enlargeStartDistance = ctx.mdb->readIEEEFloatLE();

	ctx.mdb->skip(308);

	uint32 offTextureInfo = ctx.mdb->readUint32LE();

	ctx.mdb->skip(4);

	uint32 endPos = ctx.mdb->seekTo(ctx.offRawData + offMeshArrays);

	ctx.mdb->skip(4);

	uint32 verticesStart, verticesCount;
	readArray(*ctx.mdb, verticesStart, verticesCount);

	uint32 normalsStart, normalsCount;
	readArray(*ctx.mdb, normalsStart, normalsCount);

	uint32 tangentsStart, tangentsCount;
	readArray(*ctx.mdb, tangentsStart, tangentsCount);

	uint32 biNormalsStart, biNormalsCount;
	readArray(*ctx.mdb, biNormalsStart, biNormalsCount);

	uint32 tVerts0Start, tVerts0Count;
	readArray(*ctx.mdb, tVerts0Start, tVerts0Count);

	uint32 tVerts1Start, tVerts1Count;
	readArray(*ctx.mdb, tVerts1Start, tVerts1Count);

	uint32 tVerts2Start, tVerts2Count;
	readArray(*ctx.mdb, tVerts2Start, tVerts2Count);

	uint32 tVerts3Start, tVerts3Count;
	readArray(*ctx.mdb, tVerts3Start, tVerts3Count);

	uint32 unknownStart, unknownCount;
	readArray(*ctx.mdb, unknownStart, unknownCount);

	uint32 facesStart, facesCount;
	readArray(*ctx.mdb, facesStart, facesCount);

	if (ctx.fileVersion == 133)
		ctx.offTexData = ctx.mdb->readUint32LE();

	ctx.mdb->seekTo(ctx.offRawData + verticesStart);

	ctx.mesh->verts.resize(3 * verticesCount);
	for (uint32 i = 0; i < verticesCount; i++) {
		ctx.mesh->verts[3 * i + 0] = ctx.mdb->readIEEEFloatLE();
		ctx.mesh->verts[3 * i + 1] = ctx.mdb->readIEEEFloatLE();
		ctx.mesh->verts[3 * i + 2] = ctx.mdb->readIEEEFloatLE();
	}

	ctx.mdb->seekTo(ctx.offRawData + tVerts0Start);

	ctx.mesh->tverts.resize(3 * tVerts0Count);
	for (uint32 i = 0; i < tVerts0Count; i++) {
		ctx.mesh->tverts[3 * i + 0] = ctx.mdb->readIEEEFloatLE();
		ctx.mesh->tverts[3 * i + 1] = ctx.mdb->readIEEEFloatLE();
		ctx.mesh->tverts[3 * i + 2] = 0;
	}

	ctx.mdb->seekTo(ctx.offRawData + facesStart);

	ctx.mesh->faceCount = facesCount;

	ctx.mesh-> vertIndices.resize(3 * facesCount);
	ctx.mesh->tvertIndices.resize(3 * facesCount);
	for (uint32 i = 0; i < facesCount; i++) {
		ctx.mdb->skip(4 * 4 + 4);

		if (ctx.fileVersion == 133)
			ctx.mdb->skip(3 * 4);

		ctx.mesh->vertIndices[i * 3 + 0] = ctx.mesh->tvertIndices[i * 3 + 0] = ctx.mdb->readUint32LE();
		ctx.mesh->vertIndices[i * 3 + 1] = ctx.mesh->tvertIndices[i * 3 + 1] = ctx.mdb->readUint32LE();
		ctx.mesh->vertIndices[i * 3 + 2] = ctx.mesh->tvertIndices[i * 3 + 2] = ctx.mdb->readUint32LE();

		if (ctx.fileVersion == 133)
			ctx.mdb->skip(4);
	}

	if (texture[0] != "NULL") {
		uint32 offset;
		if (ctx.fileVersion == 133)
			offset = ctx.offRawData + ctx.offTexData;
		else
			offset = ctx.offTexData + offTextureInfo;

		ctx.mdb->seek(offset);

		uint32 textureCount = ctx.mdb->readUint32LE();
		uint32 offTexture   = ctx.mdb->readUint32LE();

		std::vector<Common::UString> textureLine;
		textureLine.resize(textureCount);
		for (std::vector<Common::UString>::iterator line = textureLine.begin(); line != textureLine.end(); ++line) {
			line->readLineASCII(*ctx.mdb);
			ctx.mdb->skip(1);

			line->trim();
		}

		for (std::vector<Common::UString>::const_iterator line = textureLine.begin(); line != textureLine.end(); ++line) {
			int n = -1;
			if      (line->beginsWith("texture texture0 "))
				n = 17;
			else if (line->beginsWith("texture tex "))
				n = 12;

			if (n != -1) {
				Common::UString::iterator it = line->begin();
				while (n-- > 0)
					it++;

				ctx.mesh->textures.clear();

				Common::UString texture;
				while (it != line->end())
					texture += *it++;

				ctx.mesh->textures.push_back(texture);
			}
		}

	} else
		ctx.node->render = false;

	ctx.mdb->seekTo(endPos);
}

void Model_Witcher::readNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data) {
	uint32 pos = ctx.mdb->seekTo(offset);

	// TODO: Implement this properly :P

	for (uint32 i = 0; i < count; i++) {
		uint32 type        = ctx.mdb->readUint32LE();
		uint16 rowCount    = ctx.mdb->readUint16LE();
		uint16 timeIndex   = ctx.mdb->readUint16LE();
		uint16 dataIndex   = ctx.mdb->readUint16LE();
		uint8  columnCount = ctx.mdb->readByte();
		ctx.mdb->skip(1);

		if (rowCount == 0xFFFF) {
			warning("TODO: Model_Witcher::readNodeControllers(): rowCount == 0xFFFF");
			continue;
		}

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

	ctx.mdb->seekTo(pos);
}

} // End of namespace Aurora

} // End of namespace Graphics
