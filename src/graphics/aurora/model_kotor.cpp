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

/** @file graphics/aurora/model_kotor.cpp
 *  Loading models found in Star Wars: Knights of the Old Republic.
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

#include "graphics/aurora/model_kotor.h"

using Common::kDebugGraphics;

static const int kNodeFlagHasHeader    = 0x0001;
static const int kNodeFlagHasLight     = 0x0002;
static const int kNodeFlagHasEmitter   = 0x0004;
static const int kNodeFlagHasReference = 0x0010;
static const int kNodeFlagHasMesh      = 0x0020;
static const int kNodeFlagHasSkin      = 0x0040;
static const int kNodeFlagHasAnim      = 0x0080;
static const int kNodeFlagHasDangly    = 0x0100;
static const int kNodeFlagHasAABB      = 0x0200;

static const uint32 kControllerTypePosition             = 8;
static const uint32 kControllerTypeOrientation          = 20;
static const uint32 kControllerTypeScale                = 36;
static const uint32 kControllerTypeColor                = 76;
static const uint32 kControllerTypeRadius               = 88;
static const uint32 kControllerTypeShadowRadius         = 96;
static const uint32 kControllerTypeVerticalDisplacement = 100;
static const uint32 kControllerTypeMultiplier           = 140;
static const uint32 kControllerTypeAlphaEnd             = 80;
static const uint32 kControllerTypeAlphaStart           = 84;
static const uint32 kControllerTypeBirthRate            = 88;
static const uint32 kControllerTypeBounce_Co            = 92;
static const uint32 kControllerTypeColorEnd             = 96;
static const uint32 kControllerTypeColorStart           = 108;
static const uint32 kControllerTypeCombineTime          = 120;
static const uint32 kControllerTypeDrag                 = 124;
static const uint32 kControllerTypeFPS                  = 128;
static const uint32 kControllerTypeFrameEnd             = 132;
static const uint32 kControllerTypeFrameStart           = 136;
static const uint32 kControllerTypeGrav                 = 140;
static const uint32 kControllerTypeLifeExp              = 144;
static const uint32 kControllerTypeMass                 = 148;
static const uint32 kControllerTypeP2P_Bezier2          = 152;
static const uint32 kControllerTypeP2P_Bezier3          = 156;
static const uint32 kControllerTypeParticleRot          = 160;
static const uint32 kControllerTypeRandVel              = 164;
static const uint32 kControllerTypeSizeStart            = 168;
static const uint32 kControllerTypeSizeEnd              = 172;
static const uint32 kControllerTypeSizeStart_Y          = 176;
static const uint32 kControllerTypeSizeEnd_Y            = 180;
static const uint32 kControllerTypeSpread               = 184;
static const uint32 kControllerTypeThreshold            = 188;
static const uint32 kControllerTypeVelocity             = 192;
static const uint32 kControllerTypeXSize                = 196;
static const uint32 kControllerTypeYSize                = 200;
static const uint32 kControllerTypeBlurLength           = 204;
static const uint32 kControllerTypeLightningDelay       = 208;
static const uint32 kControllerTypeLightningRadius      = 212;
static const uint32 kControllerTypeLightningScale       = 216;
static const uint32 kControllerTypeDetonate             = 228;
static const uint32 kControllerTypeAlphaMid             = 464;
static const uint32 kControllerTypeColorMid             = 468;
static const uint32 kControllerTypePercentStart         = 480;
static const uint32 kControllerTypePercentMid           = 481;
static const uint32 kControllerTypePercentEnd           = 482;
static const uint32 kControllerTypeSizeMid              = 484;
static const uint32 kControllerTypeSizeMid_Y            = 488;
static const uint32 kControllerTypeSelfIllumColor       = 100;
static const uint32 kControllerTypeAlpha                = 128;

namespace Graphics {

namespace Aurora {

Model_KotOR::ParserContext::ParserContext(const Common::UString &name, bool k2,
                                          const Common::UString &t) :
	mdl(0), mdx(0), kotor2(k2), texture(t), state(0), nodeEntity(0) {

	if (!(mdl = ResMan.getResource(name, ::Aurora::kFileTypeMDL)))
		throw Common::Exception("No such MDL \"%s\"", name.c_str());
	if (!(mdx = ResMan.getResource(name, ::Aurora::kFileTypeMDX)))
		throw Common::Exception("No such MDX \"%s\"", name.c_str());
}

Model_KotOR::ParserContext::~ParserContext() {
	delete mdl;
	delete mdx;
}

void Model_KotOR::ParserContext::newState() {
	state = 0;

	newNode();
}

void Model_KotOR::ParserContext::newNode() {
	material.reset();

	nodeEntity = 0;
}


Model_KotOR::Model_KotOR(const Common::UString &name, bool kotor2, const Common::UString &texture) :
	_fileName(name) {

	ParserContext ctx(name, kotor2, texture);

	load(ctx);

	setState(_states[""]);
}

Model_KotOR::~Model_KotOR() {
}

void Model_KotOR::load(ParserContext &ctx) {
	if (ctx.mdl->readUint32LE() != 0)
		throw Common::Exception("Unsupported KotOR ASCII MDL");

	uint32 sizeModelData = ctx.mdl->readUint32LE();
	uint32 sizeRawData   = ctx.mdl->readUint32LE();

	ctx.offModelData = 12;
	ctx.offRawData   = ctx.offModelData + sizeModelData;

	ctx.mdl->skip(8); // Function pointers

	_name.readFixedASCII(*ctx.mdl, 32);
	debugC(4, kDebugGraphics, "Loading KotOR binary model \"%s\": \"%s\"", _fileName.c_str(),
	       _name.c_str());

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

	float modelScale = ctx.mdl->readIEEEFloatLE();

	Common::UString superModelName;
	superModelName.readFixedASCII(*ctx.mdl, 32);

	ctx.mdl->skip(4); // Root node pointer again

	ctx.mdl->skip(12); // Unknown

	uint32 nameOffset, nameCount;
	readArrayDef(*ctx.mdl, nameOffset, nameCount);

	std::vector<uint32> nameOffsets;
	readArray(*ctx.mdl, ctx.offModelData + nameOffset, nameCount, nameOffsets);

	readStrings(*ctx.mdl, nameOffsets, ctx.offModelData, ctx.names);

	_rootNode = getOgreSceneManager().getRootSceneNode()->createChildSceneNode(Common::generateIDRandomString().c_str());
	_rootNode->setVisible(false);

	_states.insert(std::make_pair("", new State));
	ctx.state = _states[""];

	ctx.mdl->seek(ctx.offModelData + nodeHeadPointer);
	loadNode(ctx, _rootNode);
}

void Model_KotOR::loadNode(ParserContext &ctx, Ogre::SceneNode *parent) {
	uint16 flags      = ctx.mdl->readUint16LE();
	uint16 superNode  = ctx.mdl->readUint16LE();
	uint16 nodeNumber = ctx.mdl->readUint16LE();

	Common::UString name;
	if (nodeNumber < ctx.names.size())
		name = ctx.names[nodeNumber];

	debugC(5, kDebugGraphics, "Node \"%s\" in state \"%s\"", name.c_str(), ctx.state->name.c_str());

	createNode(ctx.nodeEntity, ctx.state, name, parent);

	ctx.mdl->skip(6 + 4); // Unknown + parent pointer

	ctx.nodeEntity->position[0] = ctx.mdl->readIEEEFloatLE();
	ctx.nodeEntity->position[1] = ctx.mdl->readIEEEFloatLE();
	ctx.nodeEntity->position[2] = ctx.mdl->readIEEEFloatLE();

	ctx.nodeEntity->orientation[0] = ctx.mdl->readIEEEFloatLE();
	ctx.nodeEntity->orientation[1] = ctx.mdl->readIEEEFloatLE();
	ctx.nodeEntity->orientation[2] = ctx.mdl->readIEEEFloatLE();
	ctx.nodeEntity->orientation[3] = ctx.mdl->readIEEEFloatLE();

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

	readNodeControllers(ctx, ctx.offModelData + controllerKeyOffset, controllerKeyCount, controllerData);

	if ((flags & 0xFC00) != 0)
		throw Common::Exception("Unknown node flags %04X", flags);

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

	Ogre::SceneNode *newParent = ctx.nodeEntity->node;
	for (std::vector<uint32>::const_iterator child = children.begin(); child != children.end(); ++child) {
		ctx.newNode();

		ctx.mdl->seek(ctx.offModelData + *child);
		loadNode(ctx, newParent);
	}
}

void Model_KotOR::readMesh(ParserContext &ctx) {
	uint32 P = ctx.mdl->pos();

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

	ctx.material.diffuse[0] = ctx.mdl->readIEEEFloatLE();
	ctx.material.diffuse[1] = ctx.mdl->readIEEEFloatLE();
	ctx.material.diffuse[2] = ctx.mdl->readIEEEFloatLE();

	ctx.material.ambient[0] = ctx.mdl->readIEEEFloatLE();
	ctx.material.ambient[1] = ctx.mdl->readIEEEFloatLE();
	ctx.material.ambient[2] = ctx.mdl->readIEEEFloatLE();

	ctx.material.specular[0] = 0.0;
	ctx.material.specular[1] = 0.0;
	ctx.material.specular[2] = 0.0;

	ctx.material.shininess = ctx.mdl->readIEEEFloatLE();

	ctx.material.textures.resize(2);
	for (int i = 0; i < 2; i++) {
		ctx.material.textures[i].readFixedASCII(*ctx.mdl, 32);
		if (ctx.material.textures[i] == "NULL")
			ctx.material.textures[i].clear();
	}

	ctx.mdl->skip(24); // Unknown

	ctx.mdl->skip(12); // Vertex indices counts

	uint32 offOffVerts, offOffVertsCount;
	readArrayDef(*ctx.mdl, offOffVerts, offOffVertsCount);

	if (offOffVertsCount > 1)
		throw Common::Exception("Face offsets offsets count wrong (%d)", offOffVertsCount);

	ctx.mdl->skip(12); // Unknown

	ctx.mdl->skip(24 + 16); // Unknown

	uint32 mdxStructSize = ctx.mdl->readUint32LE();

	ctx.mdl->skip(8); // Unknown

	uint32 offNormals = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4); // Unknown

	uint32 offUV[2];
	offUV[0] = ctx.mdl->readUint32LE();
	offUV[1] = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24); // Unknown

	uint16 vertexCount  = ctx.mdl->readUint16LE();
	uint16 textureCount = ctx.mdl->readUint16LE();

	ctx.mdl->skip(2);

	byte unknownFlag1 = ctx.mdl->readByte();
	bool shadow       = ctx.mdl->readByte() == 1;
	byte unknownFlag2 = ctx.mdl->readByte();
	bool render       = ctx.mdl->readByte() == 1;

	if (!render)
		ctx.nodeEntity->dontRender = true;

	ctx.mdl->skip(10);

	if (ctx.kotor2)
		ctx.mdl->skip(8);

	uint32 offNodeData = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4);

	if ((offOffVertsCount < 1) || (vertexCount == 0) || (facesCount == 0))
		return;

	if (textureCount > 2)
		warning("Model_KotOR::readMesh(): textureCount > 2 (%d)", textureCount);

	if ((textureCount > 0) && !ctx.texture.empty())
		ctx.material.textures[0] = ctx.texture;

	ctx.material.textures.resize(textureCount);

	ctx.material.trimTextures();
	textureCount = ctx.material.textures.size();

	if (textureCount == 0)
		ctx.nodeEntity->dontRender = true;

	Ogre::MaterialPtr material = MaterialMan.get(ctx.material);

	uint32 endPos = ctx.mdl->pos();


	// Read vertices

	VertexDeclaration vertexDecl(facesCount, vertexCount, textureCount);

	float *v  = &vertexDecl.bufferVerticesNormals[0];
	float *vt = &vertexDecl.bufferTexCoords[0];
	for (uint32 i = 0; i < vertexCount; i++) {
		// Position
		ctx.mdx->seekTo(offNodeData + i * mdxStructSize);
		*v++ = ctx.mdx->readIEEEFloatLE();
		*v++ = ctx.mdx->readIEEEFloatLE();
		*v++ = ctx.mdx->readIEEEFloatLE();

		// Normal
		//ctx.mdx->seekTo(offNodeData + i * mdxStructSize + offNormals);
		*v++ = ctx.mdx->readIEEEFloatLE();
		*v++ = ctx.mdx->readIEEEFloatLE();
		*v++ = ctx.mdx->readIEEEFloatLE();

		// TexCoords
		for (uint16 t = 0; t < textureCount; t++) {
			if (offUV[t] != 0xFFFFFFFF) {
				ctx.mdx->seekTo(offNodeData + i * mdxStructSize + offUV[t]);
				*vt++ = ctx.mdx->readIEEEFloatLE();
				*vt++ = ctx.mdx->readIEEEFloatLE();
			} else {
				*vt++ = 0.0;
				*vt++ = 0.0;
			}
		}
	}

	// Read faces

	ctx.mdl->seekTo(ctx.offModelData + offOffVerts);
	uint32 offVerts = ctx.mdl->readUint32LE();

	ctx.mdl->seekTo(ctx.offModelData + offVerts);

	uint16 *f = &vertexDecl.bufferIndices[0];
	for (uint32 i = 0; i < facesCount * 3; i++)
		*f++ = ctx.mdl->readUint16LE();

	// Create an entity with the mesh defined by the vertex declaration and the material
	Ogre::Entity *entity = createEntity(vertexDecl, material);

	// Add the entity to our lists

	_entities.push_back(entity);
	ctx.nodeEntity->entity = entity;

	ctx.mdl->seekTo(endPos);
}

void Model_KotOR::readNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data) {
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

	ctx.mdl->seekTo(pos);
}

void Model_KotOR::readStrings(Common::SeekableReadStream &mdl, const std::vector<uint32> &offsets,
		uint32 offset, std::vector<Common::UString> &strings) {

	uint32 pos = mdl.pos();

	strings.resize(offsets.size());
	for (uint32 i = 0; i < offsets.size(); i++) {
		mdl.seekTo(offsets[i] + offset);
		strings[i].readASCII(mdl);
	}

	mdl.seekTo(pos);
}

} // End of namespace Aurora

} // End of namespace Graphics
