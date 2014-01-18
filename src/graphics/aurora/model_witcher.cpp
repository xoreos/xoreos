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

/** @file graphics/aurora/model_witcher.cpp
 *  Loading models found in The Witcher.
 */

// Disable the "unused variable" warnings while most stuff is still stubbed
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

#include <OgreSceneNode.h>
#include <OgreMesh.h>
#include <OgreEntity.h>
#include <OgreMeshManager.h>
#include <OgreRoot.h>

#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"
#include "common/debug.h"
#include "common/uuid.h"

#include "aurora/resman.h"

#include "graphics/util.h"
#include "graphics/materialman.h"

#include "graphics/aurora/model_witcher.h"

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

Model_Witcher::ParserContext::ParserContext(const Common::UString &name) : mdb(0), state(0), nodeEntity(0) {
	if (!(mdb = ResMan.getResource(name, ::Aurora::kFileTypeMDB)))
		throw Common::Exception("No such MDB \"%s\"", name.c_str());
}

Model_Witcher::ParserContext::~ParserContext() {
	delete mdb;
}

void Model_Witcher::ParserContext::newState() {
	state = 0;

	newNode();
}

void Model_Witcher::ParserContext::newNode() {
	material.reset();

	nodeEntity = 0;
}


Model_Witcher::Model_Witcher(const Common::UString &name) : _fileName(name) {
	ParserContext ctx(name);

	load(ctx);

	setState(_states[""]);
}

Model_Witcher::~Model_Witcher() {
}

void Model_Witcher::load(ParserContext &ctx) {
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

	_name.readFixedASCII(*ctx.mdb, 64);
	debugC(4, kDebugGraphics, "Loading Witcher binary model \"%s\": \"%s\"", _fileName.c_str(),
	       _name.c_str());

	uint32 offsetRootNode = ctx.mdb->readUint32LE();

	ctx.mdb->skip(32);

	byte type = ctx.mdb->readByte();

	ctx.mdb->skip(3);

	ctx.mdb->skip(48);

	float firstLOD = ctx.mdb->readIEEEFloatLE();
	float lastLOD  = ctx.mdb->readIEEEFloatLE();

	ctx.mdb->skip(16);

	Common::UString detailMap;
	detailMap.readFixedASCII(*ctx.mdb, 64);

	ctx.mdb->skip(4);

	float nodeScale = ctx.mdb->readIEEEFloatLE();

	Common::UString superModel;
	superModel.readFixedASCII(*ctx.mdb, 64);

	ctx.mdb->skip(16);

	_rootNode = getOgreSceneManager().getRootSceneNode()->createChildSceneNode(Common::generateIDRandomString().c_str());
	_rootNode->setVisible(false);

	_states.insert(std::make_pair("", new State));
	ctx.state = _states[""];

	ctx.mdb->seek(ctx.offModelData + offsetRootNode);
	loadNode(ctx, _rootNode);
}

void Model_Witcher::loadNode(ParserContext &ctx, Ogre::SceneNode *parent) {
	ctx.mdb->skip(24);

	uint32 inheritColor = ctx.mdb->readUint32LE();
	uint32 nodeNumber   = ctx.mdb->readUint32LE();

	Common::UString name;
	name.readFixedASCII(*ctx.mdb, 64);

	debugC(5, kDebugGraphics, "Node \"%s\" in state \"%s\"", name.c_str(), ctx.state->name.c_str());

	createNode(ctx.nodeEntity, ctx.state, name, parent);

	ctx.mdb->skip(8); // Parent pointers

	uint32 childrenOffset, childrenCount;
	readArrayDef(*ctx.mdb, childrenOffset, childrenCount);

	std::vector<uint32> children;
	readArray(*ctx.mdb, ctx.offModelData + childrenOffset, childrenCount, children);

	uint32 controllerKeyOffset, controllerKeyCount;
	readArrayDef(*ctx.mdb, controllerKeyOffset, controllerKeyCount);

	uint32 controllerDataOffset, controllerDataCount;
	readArrayDef(*ctx.mdb, controllerDataOffset, controllerDataCount);

	std::vector<float> controllerData;
	readArray(*ctx.mdb, ctx.offModelData + controllerDataOffset, controllerDataCount, controllerData);

	readNodeControllers(ctx, ctx.offModelData + controllerKeyOffset, controllerKeyCount, controllerData);

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

	Ogre::SceneNode *newParent = ctx.nodeEntity->node;
	for (std::vector<uint32>::const_iterator child = children.begin(); child != children.end(); ++child) {
		ctx.newNode();

		ctx.mdb->seek(ctx.offModelData + *child);
		loadNode(ctx, newParent);
	}
}

void Model_Witcher::readMesh(ParserContext &ctx) {
	ctx.mdb->skip(8);

	uint32 offMeshArrays = ctx.mdb->readUint32LE();

	ctx.mdb->skip(76);

	ctx.material.ambient[0] = ctx.mdb->readIEEEFloatLE();
	ctx.material.ambient[1] = ctx.mdb->readIEEEFloatLE();
	ctx.material.ambient[2] = ctx.mdb->readIEEEFloatLE();

	ctx.material.diffuse[0] = ctx.mdb->readIEEEFloatLE();
	ctx.material.diffuse[1] = ctx.mdb->readIEEEFloatLE();
	ctx.material.diffuse[2] = ctx.mdb->readIEEEFloatLE();

	ctx.material.specular[0] = ctx.mdb->readIEEEFloatLE();
	ctx.material.specular[1] = ctx.mdb->readIEEEFloatLE();
	ctx.material.specular[2] = ctx.mdb->readIEEEFloatLE();

	ctx.material.shininess = ctx.mdb->readIEEEFloatLE();

	ctx.mdb->skip(20);

	Common::UString texture[4];
	texture[0].readFixedASCII(*ctx.mdb, 64);
	texture[1].readFixedASCII(*ctx.mdb, 64);
	texture[2].readFixedASCII(*ctx.mdb, 64);
	texture[3].readFixedASCII(*ctx.mdb, 64);

	ctx.mdb->skip(20);

	uint32 fourCC = ctx.mdb->readUint32BE();

	if (fourCC == MKTAG('O', 'P', 'Q', 'E'))
		ctx.material.transparency = kTransparencyHintOpaque;

	ctx.mdb->skip(8);

	float coronaCenterX = ctx.mdb->readIEEEFloatLE();

	ctx.mdb->skip(8);

	float enlargeStartDistance = ctx.mdb->readIEEEFloatLE();

	ctx.mdb->skip(308);

	ctx.offTextureInfo = ctx.mdb->readUint32LE();

	ctx.mdb->skip(4);

	uint32 endPos = ctx.mdb->seekTo(ctx.offRawData + offMeshArrays);

	ctx.mdb->skip(4);

	uint32 vertexOffset, vertexCount;
	Model::readArrayDef(*ctx.mdb, vertexOffset, vertexCount);

	uint32 normalsOffset, normalsCount;
	Model::readArrayDef(*ctx.mdb, normalsOffset, normalsCount);

	uint32 tangentsOffset, tangentsCount;
	Model::readArrayDef(*ctx.mdb, tangentsOffset, tangentsCount);

	uint32 biNormalsOffset, biNormalsCount;
	Model::readArrayDef(*ctx.mdb, biNormalsOffset, biNormalsCount);

	uint32 tVerts0Offset, tVerts0Count;
	Model::readArrayDef(*ctx.mdb, tVerts0Offset, tVerts0Count);

	uint32 tVerts1Offset, tVerts1Count;
	Model::readArrayDef(*ctx.mdb, tVerts1Offset, tVerts1Count);

	uint32 tVerts2Offset, tVerts2Count;
	Model::readArrayDef(*ctx.mdb, tVerts2Offset, tVerts2Count);

	uint32 tVerts3Offset, tVerts3Count;
	Model::readArrayDef(*ctx.mdb, tVerts3Offset, tVerts3Count);

	uint32 unknownOffset, unknownCount;
	Model::readArrayDef(*ctx.mdb, unknownOffset, unknownCount);

	uint32 facesOffset, facesCount;
	Model::readArrayDef(*ctx.mdb, facesOffset, facesCount);

	if (ctx.fileVersion == 133)
		ctx.offTexData = ctx.mdb->readUint32LE();


	if ((vertexCount == 0) || (facesCount == 0)) {
		ctx.mdb->seekTo(endPos);
		return;
	}

	std::vector<Common::UString> textures;
	readTextures(ctx, texture[0], ctx.material.textures);

	uint16 textureCount = ctx.material.textures.size();
	if (textureCount == 0)
		ctx.nodeEntity->dontRender = true;

	Ogre::MaterialPtr material = MaterialMan.get(ctx.material);


	// Read vertices

	assert(vertexCount  <= 65535);
	assert(normalsCount <= 65535);
	assert(tVerts0Count <= 65535);

	assert(normalsCount == vertexCount);
	assert(tVerts0Count == vertexCount);

	VertexDeclaration vertexDecl(facesCount, vertexCount, textureCount);

	// Read vertex position
	ctx.mdb->seekTo(ctx.offRawData + vertexOffset);
	float *vp = &vertexDecl.bufferVerticesNormals[0];
	for (uint32 i = 0; i< vertexCount; i++) {
		*vp++ = ctx.mdb->readIEEEFloatLE();
		*vp++ = ctx.mdb->readIEEEFloatLE();
		*vp++ = ctx.mdb->readIEEEFloatLE();

		vp += 3;
	}

	// Read vertex normals
	ctx.mdb->seekTo(ctx.offRawData + normalsOffset);
	float *vn = &vertexDecl.bufferVerticesNormals[3];
	for (uint32 i = 0; i < normalsCount; i++) {
		*vn++ = ctx.mdb->readIEEEFloatLE();
		*vn++ = ctx.mdb->readIEEEFloatLE();
		*vn++ = ctx.mdb->readIEEEFloatLE();

		vn += 3;
	}

	if (textureCount > 0) {
		// Read texture coordinates
		ctx.mdb->seekTo(ctx.offRawData + tVerts0Offset);
		float *vt = &vertexDecl.bufferTexCoords[0];
		for (uint32 i = 0; i < tVerts0Count; i++) {
			*vt++ = ctx.mdb->readIEEEFloatLE();
			*vt++ = ctx.mdb->readIEEEFloatLE();
		}
	}

	// Read faces
	ctx.mdb->seekTo(ctx.offRawData + facesOffset);
	uint16 *f = &vertexDecl.bufferIndices[0];
	for (uint32 i = 0; i < facesCount; i++) {
		ctx.mdb->skip(4 * 4 + 4);

		if (ctx.fileVersion == 133)
			ctx.mdb->skip(3 * 4);

		// Vertex indices
		*f++ = ctx.mdb->readUint32LE();
		*f++ = ctx.mdb->readUint32LE();
		*f++ = ctx.mdb->readUint32LE();

		if (ctx.fileVersion == 133)
			ctx.mdb->skip(4);
	}

	// Create an entity with the mesh defined by the vertex declaration and the material
	Ogre::Entity *entity = createEntity(vertexDecl, material);

	// Add the entity to our lists

	_entities.push_back(entity);
	ctx.nodeEntity->entity = entity;

	ctx.mdb->seekTo(endPos);
}

void Model_Witcher::readNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data) {
	uint32 pos = ctx.mdb->seekTo(offset);

	// TODO: readNodeControllers: Implement this properly :P

	for (uint32 i = 0; i < count; i++) {
		uint32 type        = ctx.mdb->readUint32LE();
		uint16 rowCount    = ctx.mdb->readUint16LE();
		uint16 timeIndex   = ctx.mdb->readUint16LE();
		uint16 dataIndex   = ctx.mdb->readUint16LE();
		uint8  columnCount = ctx.mdb->readByte();
		ctx.mdb->skip(1);

		if (rowCount == 0xFFFF)
			// TODO: Controller row count = 0xFFFF
			continue;

		if        (type == kControllerTypePosition) {
			if (columnCount != 3)
				throw Common::Exception("Position controller with %d values", columnCount);

			for (int r = 0; r < rowCount; r++) {
				float pT = data[timeIndex + r];
				float pX = data[dataIndex + (r * columnCount) + 0];
				float pY = data[dataIndex + (r * columnCount) + 1];
				float pZ = data[dataIndex + (r * columnCount) + 2];

				// Starting position
				if (pT == 0.0) {
					ctx.nodeEntity->position[0] = pX;
					ctx.nodeEntity->position[1] = pY;
					ctx.nodeEntity->position[2] = pZ;
				}
			}

		} else if (type == kControllerTypeOrientation) {
			if (columnCount != 4)
				throw Common::Exception("Orientation controller with %d values", columnCount);

			for (int r = 0; r < rowCount; r++) {
				float qT = data[timeIndex + r];
				float qX = data[dataIndex + (r * columnCount) + 0];
				float qY = data[dataIndex + (r * columnCount) + 1];
				float qZ = data[dataIndex + (r * columnCount) + 2];
				float qQ = data[dataIndex + (r * columnCount) + 3];

				// Starting orientation
				if (qT == 0.0) {
					ctx.nodeEntity->orientation[0] = qQ;
					ctx.nodeEntity->orientation[1] = qX;
					ctx.nodeEntity->orientation[2] = qY;
					ctx.nodeEntity->orientation[3] = qZ;
				}
			}
		}

	}

	ctx.mdb->seekTo(pos);
}

void Model_Witcher::readTextures(ParserContext &ctx, const Common::UString &texture,
                                 std::vector<Common::UString> &textures) {

	if (texture == "NULL") {
		ctx.nodeEntity->dontRender = true;
		return;
	}

	uint32 offset;
	if (ctx.fileVersion == 133)
		offset = ctx.offRawData + ctx.offTexData;
	else
		offset = ctx.offTexData + ctx.offTextureInfo;

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

			textures.clear();

			Common::UString t;
			while (it != line->end())
				t += *it++;

			textures.push_back(t);
		}
	}

}

} // End of namespace Aurora

} // End of namespace Graphics
