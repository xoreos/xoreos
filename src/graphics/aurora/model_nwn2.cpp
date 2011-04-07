/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model_nwn2.cpp
 *  Loading MDB files found in Neverwinter Nights 2
 */

// Disable the "unused variable" warnings while most stuff is still stubbed
#pragma GCC diagnostic ignored "-Wunused-variable"

#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"

#include "graphics/aurora/model_nwn2.h"

static const uint32 kMDBID = MKID_BE('NWN2');

static const uint32 kRigidID = MKID_BE('RIGD');
static const uint32 kSkinID  = MKID_BE('SKIN');

namespace Graphics {

namespace Aurora {

Model_NWN2::ParserContext::ParserContext(Common::SeekableReadStream &stream) :
	mdb(&stream), state(0) {

}

Model_NWN2::ParserContext::~ParserContext() {
	clear();
}

void Model_NWN2::ParserContext::clear() {
	for (std::list<ModelNode_NWN2 *>::iterator n = nodes.begin(); n != nodes.end(); ++n)
		delete *n;
	nodes.clear();

	delete state;
	state = 0;
}


Model_NWN2::Model_NWN2(Common::SeekableReadStream &mdb, ModelType type) :
	Model(type) {

	ParserContext ctx(mdb);

	load(ctx);

	finalize();
}

Model_NWN2::~Model_NWN2() {
}

void Model_NWN2::load(ParserContext &ctx) {
	if (ctx.mdb->readUint32BE() != kMDBID)
		throw Common::Exception("Not a NWN2 MDB file");

	uint16 verMajor = ctx.mdb->readUint16LE();
	uint16 verMinor = ctx.mdb->readUint16LE();

	uint32 packetCount = ctx.mdb->readUint32LE();

	std::vector<PacketKey> packetKeys;
	packetKeys.resize(packetCount);
	for (std::vector<PacketKey>::iterator packetKey = packetKeys.begin();
	     packetKey != packetKeys.end(); ++packetKey) {

		packetKey->signature = ctx.mdb->readUint32BE();
		packetKey->offset    = ctx.mdb->readUint32LE();
	}

	newState(ctx);

	for (std::vector<PacketKey>::const_iterator packetKey = packetKeys.begin();
	     packetKey != packetKeys.end(); ++packetKey) {

		ctx.mdb->seek(packetKey->offset);

		ModelNode_NWN2 *newNode = new ModelNode_NWN2(*this);
		bool success = false;

		if      (packetKey->signature == kRigidID)
			success = newNode->loadRigid(ctx);
		else if (packetKey->signature == kSkinID)
			success = newNode->loadSkin (ctx);

		if (success)
			ctx.nodes.push_back(newNode);
		else
			delete newNode;
	}

	addState(ctx);
}

void Model_NWN2::newState(ParserContext &ctx) {
	ctx.clear();

	ctx.state = new State;
}

void Model_NWN2::addState(ParserContext &ctx) {
	if (!ctx.state || ctx.nodes.empty()) {
		ctx.clear();
		return;
	}

	for (std::list<ModelNode_NWN2 *>::iterator n = ctx.nodes.begin();
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


ModelNode_NWN2::ModelNode_NWN2(Model &model) : ModelNode(model) {
}

ModelNode_NWN2::~ModelNode_NWN2() {
}

bool ModelNode_NWN2::loadRigid(Model_NWN2::ParserContext &ctx) {
	if (ctx.mdb->readUint32BE() != kRigidID)
		throw Common::Exception("Packet signatures do not match");

	uint32 packetSize = ctx.mdb->readUint32LE();

	_name.readASCII(*ctx.mdb, 32);

	// Skipping lower level of detail models
	if (_name.endsWith("_L01") || _name.endsWith("_L02"))
		return false;

	Common::UString diffuseMap, normalMap, tintMap, glowMap;
	diffuseMap.readASCII(*ctx.mdb, 32);
	 normalMap.readASCII(*ctx.mdb, 32);
	   tintMap.readASCII(*ctx.mdb, 32);
	   glowMap.readASCII(*ctx.mdb, 32);

	_diffuse [0] = ctx.mdb->readIEEEFloatLE();
	_diffuse [1] = ctx.mdb->readIEEEFloatLE();
	_diffuse [2] = ctx.mdb->readIEEEFloatLE();
	_specular[0] = ctx.mdb->readIEEEFloatLE();
	_specular[1] = ctx.mdb->readIEEEFloatLE();
	_specular[2] = ctx.mdb->readIEEEFloatLE();

	float  specularPower = ctx.mdb->readIEEEFloatLE();
	float  specularValue = ctx.mdb->readIEEEFloatLE();
	uint32 textureFlags  = ctx.mdb->readUint32LE();

	uint32 vertexCount = ctx.mdb->readUint32LE();
	uint32 faceCount   = ctx.mdb->readUint32LE();

	if ((vertexCount == 0) || (faceCount == 0))
		return false;

	std::vector<Common::UString> textures;
	textures.push_back(diffuseMap);

	uint32 textureCount = textures.size();

	loadTextures(textures);
	if (!createFaces(faceCount))
		return false;


	// Read vertex coordinates

	std::vector<float> vX, vY, vZ;
	vX.resize(vertexCount);
	vY.resize(vertexCount);
	vZ.resize(vertexCount);

	std::vector<float> tX, tY, tZ;
	tX.resize(vertexCount);
	tY.resize(vertexCount);
	tZ.resize(vertexCount);

	for (uint32 i = 0; i < vertexCount; i++) {
		vX[i] = ctx.mdb->readIEEEFloatLE();
		vY[i] = ctx.mdb->readIEEEFloatLE();
		vZ[i] = ctx.mdb->readIEEEFloatLE();

		ctx.mdb->skip(3 * 4); // Normals
		ctx.mdb->skip(3 * 4); // Tangents
		ctx.mdb->skip(3 * 4); // Binormals

		tX[i] = ctx.mdb->readIEEEFloatLE();
		tY[i] = ctx.mdb->readIEEEFloatLE();
		tZ[i] = ctx.mdb->readIEEEFloatLE();
	}


	// Read faces

	for (uint32 i = 0; i < faceCount; i++) {
		const uint16 v1 = ctx.mdb->readUint16LE();
		const uint16 v2 = ctx.mdb->readUint16LE();
		const uint16 v3 = ctx.mdb->readUint16LE();

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
			_tX[3 * textureCount * i + 3 * t + 0] = v1 < tX.size() ? tX[v1] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 0] = v1 < tY.size() ? tY[v1] : 0.0;

			_tX[3 * textureCount * i + 3 * t + 1] = v2 < tX.size() ? tX[v2] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 1] = v2 < tY.size() ? tY[v2] : 0.0;

			_tX[3 * textureCount * i + 3 * t + 2] = v3 < tX.size() ? tX[v3] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 2] = v3 < tY.size() ? tY[v3] : 0.0;
		}

	}

	createCenter();

	_render = true;

	return true;
}

bool ModelNode_NWN2::loadSkin(Model_NWN2::ParserContext &ctx) {
	if (ctx.mdb->readUint32BE() != kSkinID)
		throw Common::Exception("Packet signatures do not match");

	uint32 packetSize = ctx.mdb->readUint32LE();

	_name.readASCII(*ctx.mdb, 32);

	// Skipping lower level of detail models
	if (_name.endsWith("_L01") || _name.endsWith("_L02"))
		return false;

	Common::UString skeletonName;
	skeletonName.readASCII(*ctx.mdb, 32);

	Common::UString diffuseMap, normalMap, tintMap, glowMap;
	diffuseMap.readASCII(*ctx.mdb, 32);
	 normalMap.readASCII(*ctx.mdb, 32);
	   tintMap.readASCII(*ctx.mdb, 32);
	   glowMap.readASCII(*ctx.mdb, 32);

	_diffuse [0] = ctx.mdb->readIEEEFloatLE();
	_diffuse [1] = ctx.mdb->readIEEEFloatLE();
	_diffuse [2] = ctx.mdb->readIEEEFloatLE();
	_specular[0] = ctx.mdb->readIEEEFloatLE();
	_specular[1] = ctx.mdb->readIEEEFloatLE();
	_specular[2] = ctx.mdb->readIEEEFloatLE();

	float  specularPower = ctx.mdb->readIEEEFloatLE();
	float  specularValue = ctx.mdb->readIEEEFloatLE();
	uint32 textureFlags  = ctx.mdb->readUint32LE();

	uint32 vertexCount = ctx.mdb->readUint32LE();
	uint32 faceCount   = ctx.mdb->readUint32LE();

	if ((vertexCount == 0) || (faceCount == 0))
		return false;

	std::vector<Common::UString> textures;
	textures.push_back(diffuseMap);

	uint32 textureCount = textures.size();

	loadTextures(textures);
	if (!createFaces(faceCount))
		return false;


	// Read vertex coordinates

	std::vector<float> vX, vY, vZ;
	vX.resize(vertexCount);
	vY.resize(vertexCount);
	vZ.resize(vertexCount);

	std::vector<float> tX, tY, tZ;
	tX.resize(vertexCount);
	tY.resize(vertexCount);
	tZ.resize(vertexCount);

	for (uint32 i = 0; i < vertexCount; i++) {
		vX[i] = ctx.mdb->readIEEEFloatLE();
		vY[i] = ctx.mdb->readIEEEFloatLE();
		vZ[i] = ctx.mdb->readIEEEFloatLE();

		ctx.mdb->skip(3 * 4); // Normals

		ctx.mdb->skip(4 * 4); // Bone weights
		ctx.mdb->skip(4 * 1); // Bone indices

		ctx.mdb->skip(3 * 4); // Tangents
		ctx.mdb->skip(3 * 4); // Binormals

		tX[i] = ctx.mdb->readIEEEFloatLE();
		tY[i] = ctx.mdb->readIEEEFloatLE();
		tZ[i] = ctx.mdb->readIEEEFloatLE();

		ctx.mdb->skip(4); // Bone count
	}


	// Read faces

	for (uint32 i = 0; i < faceCount; i++) {
		const uint16 v1 = ctx.mdb->readUint16LE();
		const uint16 v2 = ctx.mdb->readUint16LE();
		const uint16 v3 = ctx.mdb->readUint16LE();

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
			_tX[3 * textureCount * i + 3 * t + 0] = v1 < tX.size() ? tX[v1] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 0] = v1 < tY.size() ? tY[v1] : 0.0;

			_tX[3 * textureCount * i + 3 * t + 1] = v2 < tX.size() ? tX[v2] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 1] = v2 < tY.size() ? tY[v2] : 0.0;

			_tX[3 * textureCount * i + 3 * t + 2] = v3 < tX.size() ? tX[v3] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 2] = v3 < tY.size() ? tY[v3] : 0.0;
		}

	}

	createCenter();

	_render = true;

	return true;
}


} // End of namespace Aurora

} // End of namespace Graphics
