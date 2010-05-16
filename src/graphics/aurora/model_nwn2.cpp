/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "common/util.h"
#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"

#include "events/requests.h"

#include "graphics/aurora/model_nwn2.h"
#include "graphics/aurora/texture.h"

static const uint32 kMDBID = MKID_BE('NWN2');

static const uint32 kRigidID = MKID_BE('RIGD');
static const uint32 kSkinID  = MKID_BE('SKIN');

namespace Graphics {

namespace Aurora {

Model_NWN2::ParserContext::ParserContext(Common::SeekableReadStream &mdbStream) : mdb(&mdbStream), state(0), node(0) {
}

Model_NWN2::ParserContext::~ParserContext() {
	delete node;
	delete state;
}


Model_NWN2::Model_NWN2(Common::SeekableReadStream &mdb, ModelType type) : Model(type) {
	load(mdb);
	setState();

	RequestMan.sync();
}

Model_NWN2::~Model_NWN2() {
}

void Model_NWN2::load(Common::SeekableReadStream &mdb) {
	ParserContext ctx(mdb);

	if (ctx.mdb->readUint32BE() != kMDBID)
		throw Common::Exception("Not a NWN2 MDB file");

	uint16 verMajor = ctx.mdb->readUint16LE();
	uint16 verMinor = ctx.mdb->readUint16LE();

	uint32 packetCount = ctx.mdb->readUint32LE();

	std::vector<PacketKey> packetKeys;
	packetKeys.resize(packetCount);
	for (std::vector<PacketKey>::iterator packetKey = packetKeys.begin(); packetKey != packetKeys.end(); ++packetKey) {
		packetKey->signature = ctx.mdb->readUint32BE();
		packetKey->offset    = ctx.mdb->readUint32LE();
	}

	ctx.state = new State;

	warning("%d.%d: %d", verMajor, verMinor, packetCount);
	for (std::vector<PacketKey>::const_iterator packetKey = packetKeys.begin(); packetKey != packetKeys.end(); ++packetKey) {
		if      (packetKey->signature == kRigidID)
			parseRigid(ctx, packetKey->offset);
		else if (packetKey->signature == kSkinID)
			parseSkin (ctx, packetKey->offset);
	}

	_states.insert(std::make_pair(ctx.state->name, ctx.state));
	ctx.state = 0;
}

void Model_NWN2::parseRigid(ParserContext &ctx, uint32 offset) {
	ctx.mdb->seekTo(offset);

	if (ctx.mdb->readUint32BE() != kRigidID)
		throw Common::Exception("Packet signatures do not match");

	uint32 packetSize = ctx.mdb->readUint32LE();

	ctx.node = new Node;

	ctx.node->name.readASCII(*ctx.mdb, 32);

	// Skipping lower level of detail models
	if (ctx.node->name.endsWith("_L01") || ctx.node->name.endsWith("_L02")) {
		delete ctx.node;
		ctx.node = 0;
		return;
	}

	Common::UString diffuseMap, normalMap, tintMap, glowMap;
	diffuseMap.readASCII(*ctx.mdb, 32);
	normalMap.readASCII (*ctx.mdb, 32);
	tintMap.readASCII   (*ctx.mdb, 32);
	glowMap.readASCII   (*ctx.mdb, 32);

	ctx.node->diffuse [0] = ctx.mdb->readIEEEFloatLE();
	ctx.node->diffuse [1] = ctx.mdb->readIEEEFloatLE();
	ctx.node->diffuse [2] = ctx.mdb->readIEEEFloatLE();
	ctx.node->specular[0] = ctx.mdb->readIEEEFloatLE();
	ctx.node->specular[1] = ctx.mdb->readIEEEFloatLE();
	ctx.node->specular[2] = ctx.mdb->readIEEEFloatLE();

	float  specularPower = ctx.mdb->readIEEEFloatLE();
	float  specularValue = ctx.mdb->readIEEEFloatLE();
	uint32 textureFlags  = ctx.mdb->readUint32LE();

	uint32 vertexCount = ctx.mdb->readUint32LE();
	uint32 faceCount   = ctx.mdb->readUint32LE();

	warning("\"%s\" (%8d) - %d, %d - %08X (%d)", ctx.node->name.c_str(),
			packetSize, vertexCount, faceCount, textureFlags, textureFlags);

	ctx.vertices.resize(3 * vertexCount);
	ctx.verticesTexture.resize(3 * vertexCount);
	for (uint32 i = 0; i < vertexCount; i++) {
		ctx.vertices[3 * i + 0] = ctx.mdb->readIEEEFloatLE();
		ctx.vertices[3 * i + 1] = ctx.mdb->readIEEEFloatLE();
		ctx.vertices[3 * i + 2] = ctx.mdb->readIEEEFloatLE();

		ctx.mdb->skip(3 * 4); // Normals
		ctx.mdb->skip(3 * 4); // Tangents
		ctx.mdb->skip(3 * 4); // Binormals

		ctx.verticesTexture[3 * i + 0] = ctx.mdb->readIEEEFloatLE();
		ctx.verticesTexture[3 * i + 1] = ctx.mdb->readIEEEFloatLE();
		ctx.verticesTexture[3 * i + 2] = ctx.mdb->readIEEEFloatLE();
	}

	ctx.faces.reserve(3 * faceCount);
	for (uint32 i = 0; i < faceCount; i++) {
		ctx.faces.push_back(ctx.mdb->readUint16LE());
		ctx.faces.push_back(ctx.mdb->readUint16LE());
		ctx.faces.push_back(ctx.mdb->readUint16LE());
	}

	ctx.texture = diffuseMap;

	processNode(ctx);

	ctx.state->nodes.push_back(ctx.node);

	ctx.node = 0;
}

void Model_NWN2::parseSkin(ParserContext &ctx, uint32 offset) {
	ctx.mdb->seekTo(offset);

	if (ctx.mdb->readUint32BE() != kSkinID)
		throw Common::Exception("Packet signatures do not match");

	uint32 packetSize = ctx.mdb->readUint32LE();

	ctx.node = new Node;

	ctx.node->name.readASCII(*ctx.mdb, 32);

	// Skipping lower level of detail models
	if (ctx.node->name.endsWith("_L01") || ctx.node->name.endsWith("_L02")) {
		delete ctx.node;
		ctx.node = 0;
		return;
	}

	Common::UString skeletonName;
	skeletonName.readASCII(*ctx.mdb, 32);

	Common::UString diffuseMap, normalMap, tintMap, glowMap;
	diffuseMap.readASCII(*ctx.mdb, 32);
	normalMap.readASCII (*ctx.mdb, 32);
	tintMap.readASCII   (*ctx.mdb, 32);
	glowMap.readASCII   (*ctx.mdb, 32);

	ctx.node->diffuse [0] = ctx.mdb->readIEEEFloatLE();
	ctx.node->diffuse [1] = ctx.mdb->readIEEEFloatLE();
	ctx.node->diffuse [2] = ctx.mdb->readIEEEFloatLE();
	ctx.node->specular[0] = ctx.mdb->readIEEEFloatLE();
	ctx.node->specular[1] = ctx.mdb->readIEEEFloatLE();
	ctx.node->specular[2] = ctx.mdb->readIEEEFloatLE();

	float  specularPower = ctx.mdb->readIEEEFloatLE();
	float  specularValue = ctx.mdb->readIEEEFloatLE();
	uint32 textureFlags  = ctx.mdb->readUint32LE();

	uint32 vertexCount = ctx.mdb->readUint32LE();
	uint32 faceCount   = ctx.mdb->readUint32LE();

	warning("\"%s\".\"%s\" (%8d) - %d, %d - %08X", ctx.node->name.c_str(), skeletonName.c_str(),
			packetSize, vertexCount, faceCount, textureFlags);

	ctx.vertices.resize(3 * vertexCount);
	ctx.verticesTexture.resize(3 * vertexCount);
	for (uint32 i = 0; i < vertexCount; i++) {
		ctx.vertices[3 * i + 0] = ctx.mdb->readIEEEFloatLE();
		ctx.vertices[3 * i + 1] = ctx.mdb->readIEEEFloatLE();
		ctx.vertices[3 * i + 2] = ctx.mdb->readIEEEFloatLE();

		ctx.mdb->skip(3 * 4); // Normals

		ctx.mdb->skip(4 * 4); // Bone weights
		ctx.mdb->skip(4 * 1); // Bone indices

		ctx.mdb->skip(3 * 4); // Tangents
		ctx.mdb->skip(3 * 4); // Binormals

		ctx.verticesTexture[3 * i + 0] = ctx.mdb->readIEEEFloatLE();
		ctx.verticesTexture[3 * i + 1] = ctx.mdb->readIEEEFloatLE();
		ctx.verticesTexture[3 * i + 2] = ctx.mdb->readIEEEFloatLE();

		ctx.mdb->skip(4); // Bone count
	}

	ctx.faces.reserve(3 * faceCount);
	for (uint32 i = 0; i < faceCount; i++) {
		ctx.faces.push_back(ctx.mdb->readUint16LE());
		ctx.faces.push_back(ctx.mdb->readUint16LE());
		ctx.faces.push_back(ctx.mdb->readUint16LE());
	}

	ctx.texture = diffuseMap;

	processNode(ctx);

	ctx.state->nodes.push_back(ctx.node);

	ctx.node = 0;
}

void Model_NWN2::processNode(ParserContext &ctx) {
	ctx.node->faces.resize(ctx.faces.size() / 3);

	for (uint i = 0; i < ctx.node->faces.size(); i++) {
		Face &face = ctx.node->faces[i];

		face.vertices[0][0] = ctx.vertices[3 * ctx.faces[3 * i + 0] + 0];
		face.vertices[0][1] = ctx.vertices[3 * ctx.faces[3 * i + 0] + 1];
		face.vertices[0][2] = ctx.vertices[3 * ctx.faces[3 * i + 0] + 2];
		face.vertices[1][0] = ctx.vertices[3 * ctx.faces[3 * i + 1] + 0];
		face.vertices[1][1] = ctx.vertices[3 * ctx.faces[3 * i + 1] + 1];
		face.vertices[1][2] = ctx.vertices[3 * ctx.faces[3 * i + 1] + 2];
		face.vertices[2][0] = ctx.vertices[3 * ctx.faces[3 * i + 2] + 0];
		face.vertices[2][1] = ctx.vertices[3 * ctx.faces[3 * i + 2] + 1];
		face.vertices[2][2] = ctx.vertices[3 * ctx.faces[3 * i + 2] + 2];

		face.verticesTexture[0][0] = ctx.verticesTexture[3 * ctx.faces[3 * i + 0] + 0];
		face.verticesTexture[0][1] = ctx.verticesTexture[3 * ctx.faces[3 * i + 0] + 1];
		face.verticesTexture[0][2] = ctx.verticesTexture[3 * ctx.faces[3 * i + 0] + 2];
		face.verticesTexture[1][0] = ctx.verticesTexture[3 * ctx.faces[3 * i + 1] + 0];
		face.verticesTexture[1][1] = ctx.verticesTexture[3 * ctx.faces[3 * i + 1] + 1];
		face.verticesTexture[1][2] = ctx.verticesTexture[3 * ctx.faces[3 * i + 1] + 2];
		face.verticesTexture[2][0] = ctx.verticesTexture[3 * ctx.faces[3 * i + 2] + 0];
		face.verticesTexture[2][1] = ctx.verticesTexture[3 * ctx.faces[3 * i + 2] + 1];
		face.verticesTexture[2][2] = ctx.verticesTexture[3 * ctx.faces[3 * i + 2] + 2];
	}

	try {
		if (!ctx.texture.empty() && (ctx.texture != "NULL"))
			ctx.node->texture = TextureMan.get(ctx.texture);
	} catch (...) {
		ctx.node->texture.clear();
	}

	ctx.texture.clear();
	ctx.vertices.clear();
	ctx.verticesTexture.clear();
	ctx.faces.clear();
}

} // End of namespace Aurora

} // End of namespace Graphics
