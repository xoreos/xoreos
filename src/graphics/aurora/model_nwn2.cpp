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
 *  Loading MDB files found in Neverwinter Nights 2
 */

// Disable the "unused variable" warnings while most stuff is still stubbed
#pragma GCC diagnostic ignored "-Wunused-variable"

#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"

#include "aurora/types.h"
#include "aurora/resman.h"

#include "graphics/aurora/model_nwn2.h"

static const uint32 kMDBID = MKTAG('N', 'W', 'N', '2');

static const uint32 kRigidID = MKTAG('R', 'I', 'G', 'D');
static const uint32 kSkinID  = MKTAG('S', 'K', 'I', 'N');

namespace Graphics {

namespace Aurora {

Model_NWN2::ParserContext::ParserContext(const Common::UString &name) : mdb(0), state(0) {
	mdb = ResMan.getResource(name, ::Aurora::kFileTypeMDB);
	if (!mdb)
		throw Common::Exception("No such MDB \"%s\"", name.c_str());
}

Model_NWN2::ParserContext::~ParserContext() {
	delete mdb;

	clear();
}

void Model_NWN2::ParserContext::clear() {
	for (std::list<ModelNode_NWN2 *>::iterator n = nodes.begin(); n != nodes.end(); ++n)
		delete *n;
	nodes.clear();

	delete state;
	state = 0;
}


Model_NWN2::Model_NWN2(const Common::UString &name, ModelType type) : Model(type) {
	_fileName = name;

	ParserContext ctx(name);

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

	_name.readFixedASCII(*ctx.mdb, 32);

	// Skipping lower level of detail models
	if (_name.endsWith("_L01") || _name.endsWith("_L02"))
		return false;

	Common::UString diffuseMap, normalMap, tintMap, glowMap;
	diffuseMap.readFixedASCII(*ctx.mdb, 32);
	 normalMap.readFixedASCII(*ctx.mdb, 32);
	   tintMap.readFixedASCII(*ctx.mdb, 32);
	   glowMap.readFixedASCII(*ctx.mdb, 32);

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


	// Read vertices (interleaved)

	assert(!_vertData);

	GLsizei vpsize = 3;
	GLsizei vnsize = 3;
	GLsizei vtsize = 2;
	_vertSize = (vpsize + vnsize + vtsize) * sizeof(float);
	_vertCount = vertexCount;
	_vertData = std::malloc(_vertCount * _vertSize);

	VertexAttrib vp;
	vp.index = VPOSITION;
	vp.size = vpsize;
	vp.type = GL_FLOAT;
	vp.stride = _vertSize;
	vp.pointer = (float*) _vertData;
	_vertDecl.push_back(vp);

	VertexAttrib vn;
	vn.index = VNORMAL;
	vn.size = vnsize;
	vn.type = GL_FLOAT;
	vn.stride = _vertSize;
	vn.pointer = (float*) _vertData + vpsize;
	_vertDecl.push_back(vn);

	VertexAttrib vt;
	vt.index = VTCOORD;
	vt.size = vtsize;
	vt.type = GL_FLOAT;
	vt.stride = _vertSize;
	vt.pointer = (float*) _vertData + vpsize + vnsize + vtsize;
	_vertDecl.push_back(vt);

	float *v = (float *) _vertData;
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
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		//*v++ = ctx.mdb->readIEEEFloatLE();
		ctx.mdb->skip(4); // Third tcoord component
	}


	// Read faces

	assert(!_faceData);

	_faceCount = faceCount;
	_faceSize = 3 * sizeof(uint16);
	_faceType = GL_UNSIGNED_SHORT;
	_faceData = std::malloc(_faceCount * _faceSize);

	uint16 *f = (uint16 *) _faceData;
	for (uint32 i = 0; i < _faceCount * 3; i++)
		f[i] = ctx.mdb->readUint16LE();

	createBound();

	_render = true;

	return true;
}

bool ModelNode_NWN2::loadSkin(Model_NWN2::ParserContext &ctx) {
	if (ctx.mdb->readUint32BE() != kSkinID)
		throw Common::Exception("Packet signatures do not match");

	uint32 packetSize = ctx.mdb->readUint32LE();

	_name.readFixedASCII(*ctx.mdb, 32);

	// Skipping lower level of detail models
	if (_name.endsWith("_L01") || _name.endsWith("_L02"))
		return false;

	Common::UString skeletonName;
	skeletonName.readFixedASCII(*ctx.mdb, 32);

	Common::UString diffuseMap, normalMap, tintMap, glowMap;
	diffuseMap.readFixedASCII(*ctx.mdb, 32);
	 normalMap.readFixedASCII(*ctx.mdb, 32);
	   tintMap.readFixedASCII(*ctx.mdb, 32);
	   glowMap.readFixedASCII(*ctx.mdb, 32);

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


	// Read vertices (interleaved)

	assert(!_vertData);

	GLsizei vpsize = 3;
	GLsizei vnsize = 3;
	GLsizei vtsize = 2;
	_vertSize = (vpsize + vnsize + vtsize) * sizeof(float);
	_vertCount = vertexCount;
	_vertData = std::malloc(_vertCount * _vertSize);

	VertexAttrib vp;
	vp.index = VPOSITION;
	vp.size = vpsize;
	vp.type = GL_FLOAT;
	vp.stride = _vertSize;
	vp.pointer = (float*) _vertData;
	_vertDecl.push_back(vp);

	VertexAttrib vn;
	vn.index = VNORMAL;
	vn.size = vnsize;
	vn.type = GL_FLOAT;
	vn.stride = _vertSize;
	vn.pointer = (float*) _vertData + vpsize;
	_vertDecl.push_back(vn);

	VertexAttrib vt;
	vt.index = VTCOORD;
	vt.size = vtsize;
	vt.type = GL_FLOAT;
	vt.stride = _vertSize;
	vt.pointer = (float*) _vertData + vpsize + vnsize + vtsize;
	_vertDecl.push_back(vt);

	float *v = (float *) _vertData;
	for (uint32 i = 0; i < _vertCount; i++) {
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
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		//*v++ = ctx.mdb->readIEEEFloatLE();
		ctx.mdb->skip(4); // Third tcoord component

		ctx.mdb->skip(4); // Bone count
	}


	// Read faces

	assert(!_faceData);

	_faceCount = faceCount;
	_faceSize = 3 * sizeof(uint16);
	_faceType = GL_UNSIGNED_SHORT;
	_faceData = std::malloc(_faceCount * _faceSize);

	uint16 *f = (uint16 *) _faceData;
	for (uint32 i = 0; i < _faceCount * 3; i++)
		f[i] = ctx.mdb->readUint16LE();

	createBound();

	_render = true;

	return true;
}


} // End of namespace Aurora

} // End of namespace Graphics
