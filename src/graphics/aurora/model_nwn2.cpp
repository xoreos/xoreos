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

/** @file graphics/aurora/model_nwn2.cpp
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

#include "graphics/aurora/model_nwn2.h"

using Common::kDebugGraphics;

static const uint32 kMDBID = MKTAG('N', 'W', 'N', '2');

static const uint32 kRigidID = MKTAG('R', 'I', 'G', 'D');
static const uint32 kSkinID  = MKTAG('S', 'K', 'I', 'N');

namespace Graphics {

namespace Aurora {

Model_NWN2::ParserContext::ParserContext(const Common::UString &name) :
	mdb(0), state(0), nodeEntity(0) {

	if (!(mdb = ResMan.getResource(name, ::Aurora::kFileTypeMDB)))
		throw Common::Exception("No such MDB \"%s\"", name.c_str());
}

Model_NWN2::ParserContext::~ParserContext() {
	delete mdb;
}

void Model_NWN2::ParserContext::newState() {
	state = 0;

	newNode();
}

void Model_NWN2::ParserContext::newNode() {
	material.reset();

	nodeEntity = 0;
}


Model_NWN2::Model_NWN2(const Common::UString &name, const Common::UString &scene) : Model(name, scene) {
	ParserContext ctx(name);

	load(ctx);

	setState(_states[""]);
}

Model_NWN2::~Model_NWN2() {
}

void Model_NWN2::load(ParserContext &ctx) {
	if (ctx.mdb->readUint32BE() != kMDBID)
		throw Common::Exception("Not a NWN2 MDB file");

	uint16 verMajor = ctx.mdb->readUint16LE();
	uint16 verMinor = ctx.mdb->readUint16LE();

	debugC(4, kDebugGraphics, "Loading NWN2 binary model \"%s\": %d.%d", _fileName.c_str(),
	       verMajor, verMinor);

	uint32 packetCount = ctx.mdb->readUint32LE();

	std::vector<PacketKey> packetKeys;
	packetKeys.resize(packetCount);
	for (std::vector<PacketKey>::iterator packetKey = packetKeys.begin();
	     packetKey != packetKeys.end(); ++packetKey) {

		packetKey->signature = ctx.mdb->readUint32BE();
		packetKey->offset    = ctx.mdb->readUint32LE();
	}

	_rootNode = getOgreSceneManager(_scene).getRootSceneNode()->createChildSceneNode(Common::generateIDNumberString().c_str());
	_rootNode->setVisible(false);

	_states.insert(std::make_pair("", new State));
	ctx.state = _states[""];

	for (std::vector<PacketKey>::const_iterator packetKey = packetKeys.begin();
	     packetKey != packetKeys.end(); ++packetKey) {

		ctx.newNode();

		ctx.mdb->seek(packetKey->offset);
		loadNode(ctx, _rootNode, packetKey->signature);
	}
}

void Model_NWN2::loadNode(ParserContext &ctx, Ogre::SceneNode *parent, uint32 type) {
	if      (type == kRigidID)
		loadRigid(ctx, parent);
	else if (type == kSkinID)
		loadSkin(ctx, parent);
}

void Model_NWN2::loadRigid(ParserContext &ctx, Ogre::SceneNode *parent) {
	if (ctx.mdb->readUint32BE() != kRigidID)
		throw Common::Exception("Packet signatures do not match");

	uint32 packetSize = ctx.mdb->readUint32LE();

	Common::UString name;
	name.readFixedASCII(*ctx.mdb, 32);

	// Skipping lower level of detail models
	if (name.endsWith("_L01") || name.endsWith("_L02"))
		return;

	debugC(5, kDebugGraphics, "Rigid node \"%s\" in state \"%s\"", name.c_str(), ctx.state->name.c_str());

	Common::UString diffuseMap, normalMap, tintMap, glowMap;
	diffuseMap.readFixedASCII(*ctx.mdb, 32);
	 normalMap.readFixedASCII(*ctx.mdb, 32);
	   tintMap.readFixedASCII(*ctx.mdb, 32);
	   glowMap.readFixedASCII(*ctx.mdb, 32);

	ctx.material.diffuse[0] = ctx.mdb->readIEEEFloatLE();
	ctx.material.diffuse[1] = ctx.mdb->readIEEEFloatLE();
	ctx.material.diffuse[2] = ctx.mdb->readIEEEFloatLE();

	ctx.material.specular[0] = ctx.mdb->readIEEEFloatLE();
	ctx.material.specular[1] = ctx.mdb->readIEEEFloatLE();
	ctx.material.specular[2] = ctx.mdb->readIEEEFloatLE();

	float  specularPower = ctx.mdb->readIEEEFloatLE();
	float  specularValue = ctx.mdb->readIEEEFloatLE();
	uint32 textureFlags  = ctx.mdb->readUint32LE();

	ctx.material.transparency = ((textureFlags & 0x02) || (textureFlags & 0x04)) ?
	                            kTransparencyHintTransparent : kTransparencyHintOpaque;

	uint32 vertexCount = ctx.mdb->readUint32LE();
	uint32 facesCount  = ctx.mdb->readUint32LE();

	if ((vertexCount == 0) || (facesCount == 0))
		return;

	createNode(ctx.nodeEntity, ctx.state, name, parent);

	ctx.material.textures.push_back(diffuseMap);
	uint32 textureCount = ctx.material.textures.size();

	Ogre::MaterialPtr material = MaterialMan.create(ctx.material);


	// Read vertices

	VertexDeclaration vertexDecl(facesCount, vertexCount, textureCount, true);

	float *v  = &vertexDecl.bufferVerticesNormals[0];
	float *vt = &vertexDecl.bufferTexCoords[0];
	for (uint32 i = 0; i < vertexCount; i++) {
		// Position
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();

		// Normal
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();

		ctx.mdb->skip(3 * 4); // Tangent
		ctx.mdb->skip(3 * 4); // Binormal

		// Texture Coords
		*vt++ = ctx.mdb->readIEEEFloatLE();
		*vt++ = ctx.mdb->readIEEEFloatLE();
		*vt++ = ctx.mdb->readIEEEFloatLE();
	}

	// Read faces

	uint16 *f = &vertexDecl.bufferIndices[0];
	for (uint32 i = 0; i < facesCount * 3; i++)
		*f++ = ctx.mdb->readUint16LE();

	// Create an entity with the mesh defined by the vertex declaration and the material
	Ogre::Entity *entity = createEntity(vertexDecl, material);

	// Add the entity to our lists

	_entities.push_back(entity);
	ctx.nodeEntity->entity = entity;
}

void Model_NWN2::loadSkin(ParserContext &ctx, Ogre::SceneNode *parent) {
	if (ctx.mdb->readUint32BE() != kSkinID)
		throw Common::Exception("Packet signatures do not match");

	uint32 packetSize = ctx.mdb->readUint32LE();

	Common::UString name;
	name.readFixedASCII(*ctx.mdb, 32);

	// Skipping lower level of detail models
	if (name.endsWith("_L01") || name.endsWith("_L02"))
		return;

	Common::UString skeletonName;
	skeletonName.readFixedASCII(*ctx.mdb, 32);

	debugC(5, kDebugGraphics, "Skin node \"%s\" in state \"%s\" (\"%s\")",
			name.c_str(), ctx.state->name.c_str(), skeletonName.c_str());

	Common::UString diffuseMap, normalMap, tintMap, glowMap;
	diffuseMap.readFixedASCII(*ctx.mdb, 32);
	 normalMap.readFixedASCII(*ctx.mdb, 32);
	   tintMap.readFixedASCII(*ctx.mdb, 32);
	   glowMap.readFixedASCII(*ctx.mdb, 32);

	ctx.material.diffuse[0] = ctx.mdb->readIEEEFloatLE();
	ctx.material.diffuse[1] = ctx.mdb->readIEEEFloatLE();
	ctx.material.diffuse[2] = ctx.mdb->readIEEEFloatLE();

	ctx.material.specular[0] = ctx.mdb->readIEEEFloatLE();
	ctx.material.specular[1] = ctx.mdb->readIEEEFloatLE();
	ctx.material.specular[2] = ctx.mdb->readIEEEFloatLE();

	float  specularPower = ctx.mdb->readIEEEFloatLE();
	float  specularValue = ctx.mdb->readIEEEFloatLE();
	uint32 textureFlags  = ctx.mdb->readUint32LE();

	ctx.material.transparency = ((textureFlags & 0x02) || (textureFlags & 0x04)) ?
	                            kTransparencyHintTransparent : kTransparencyHintOpaque;

	uint32 vertexCount = ctx.mdb->readUint32LE();
	uint32 facesCount  = ctx.mdb->readUint32LE();

	if ((vertexCount == 0) || (facesCount == 0))
		return;

	createNode(ctx.nodeEntity, ctx.state, name, parent);

	ctx.material.textures.push_back(diffuseMap);
	uint32 textureCount = ctx.material.textures.size();

	Ogre::MaterialPtr material = MaterialMan.create(ctx.material);


	// Read vertices

	VertexDeclaration vertexDecl(facesCount, vertexCount, textureCount, true);

	float *v  = &vertexDecl.bufferVerticesNormals[0];
	float *vt = &vertexDecl.bufferTexCoords[0];
	for (uint32 i = 0; i < vertexCount; i++) {
		// Position
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();

		// Normal
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();

		ctx.mdb->skip(4 * 4); // Bone weights
		ctx.mdb->skip(4 * 1); // Bone indices
		ctx.mdb->skip(3 * 4); // Tangent
		ctx.mdb->skip(3 * 4); // Binormal

		// TexCoords
		*vt++ = ctx.mdb->readIEEEFloatLE();
		*vt++ = ctx.mdb->readIEEEFloatLE();
		*vt++ = ctx.mdb->readIEEEFloatLE();

		ctx.mdb->skip(4); // Bone count
	}

	// Read faces

	uint16 *f = &vertexDecl.bufferIndices[0];
	for (uint32 i = 0; i < facesCount * 3; i++)
		*f++ = ctx.mdb->readUint16LE();

	// Create an entity with the mesh defined by the vertex declaration and the material
	Ogre::Entity *entity = createEntity(vertexDecl, material);

	// Add the entity to our lists

	_entities.push_back(entity);
	ctx.nodeEntity->entity = entity;
}

} // End of namespace Aurora

} // End of namespace Graphics
