/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Loading MDB files found in Neverwinter Nights 2
 */

/* Based on Tazpn's MDB specs found in the NWN2Wiki
 * (<http://nwn2.wikia.com/wiki/MDB_Format>).
 */

#include <cstring>

#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"
#include "src/common/strutil.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"

#include "src/graphics/images/decoder.h"
#include "src/graphics/images/surface.h"

#include "src/graphics/aurora/model_nwn2.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

// Disable the "unused variable" warnings while most stuff is still stubbed
IGNORE_UNUSED_VARIABLES


static const uint32_t kMDBID = MKTAG('N', 'W', 'N', '2');

static const uint32_t kRigidID = MKTAG('R', 'I', 'G', 'D');
static const uint32_t kSkinID  = MKTAG('S', 'K', 'I', 'N');

namespace Graphics {

namespace Aurora {

Model_NWN2::ParserContext::ParserContext(const Common::UString &name) : mdb(0), state(0) {
	mdb = ResMan.getResource(name, ::Aurora::kFileTypeMDB);
	if (!mdb)
		throw Common::Exception("No such MDB \"%s\"", name.c_str());
	this->mdlName = name;
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

void Model_NWN2::setTint(const float tint[3][4]) {
	for (StateList::iterator s = _stateList.begin(); s != _stateList.end(); ++s)
		for (NodeList::iterator n = (*s)->nodeList.begin(); n != (*s)->nodeList.end(); ++n)
			dynamic_cast<ModelNode_NWN2 &>(**n).setTint(tint);
}

void Model_NWN2::setTintFloor(const float tint[3][4]) {
	// Tileset model floor node names are of the form TL_XX_YYYY_##_F

	for (StateList::iterator s = _stateList.begin(); s != _stateList.end(); ++s)
		for (NodeList::iterator n = (*s)->nodeList.begin(); n != (*s)->nodeList.end(); ++n)
			if ((*n)->getName().beginsWith("TL_"))
				if ((*n)->getName().endsWith("_F"))
					dynamic_cast<ModelNode_NWN2 &>(**n).setTint(tint);
}

void Model_NWN2::setTintWalls(const float tint[3][4]) {
	// Tileset model wall node names are of the form TL_XX_YYYY_##.
	// Exclude floors (TL_XX_YYYY_##_F), roof (TL_XX_YYYY_##_R),
	// walk meshes (TL_XX_YYYY_##_W) and collision meshes (TL_XX_YYYY_##_C3)

	for (StateList::iterator s = _stateList.begin(); s != _stateList.end(); ++s)
		for (NodeList::iterator n = (*s)->nodeList.begin(); n != (*s)->nodeList.end(); ++n)
			if ((*n)->getName().beginsWith("TL_"))
				if (!(*n)->getName().endsWith("_F") && !(*n)->getName().endsWith("_R") &&
				    !(*n)->getName().endsWith("_W") && !(*n)->getName().endsWith("_C3"))
					dynamic_cast<ModelNode_NWN2 &>(**n).setTint(tint);
}

void Model_NWN2::load(ParserContext &ctx) {
	uint32_t tag = ctx.mdb->readUint32BE();
	if (tag != kMDBID)
		throw Common::Exception("Not a NWN2 MDB file (%s)", Common::debugTag(tag).c_str());

	uint16_t verMajor = ctx.mdb->readUint16LE();
	uint16_t verMinor = ctx.mdb->readUint16LE();

	uint32_t packetCount = ctx.mdb->readUint32LE();

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


ModelNode_NWN2::ModelNode_NWN2(Model &model) : ModelNode(model), _tintedMapIndex(-1) {
}

ModelNode_NWN2::~ModelNode_NWN2() {
}

bool ModelNode_NWN2::loadRigid(Model_NWN2::ParserContext &ctx) {
	uint32_t tag = ctx.mdb->readUint32BE();
	if (tag != kRigidID)
		throw Common::Exception("Invalid rigid packet signature (%s)", Common::debugTag(tag).c_str());

	uint32_t packetSize = ctx.mdb->readUint32LE();

	_name = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);

	// Skipping lower level of detail models
	if (_name.endsWith("_L01") || _name.endsWith("_L02"))
		return false;

	Common::UString diffuseMap = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);
	Common::UString normalMap  = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);
	                _tintMap   = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);
	Common::UString glowMap    = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);

	_mesh = new Mesh();

	_mesh->diffuse [0] = ctx.mdb->readIEEEFloatLE();
	_mesh->diffuse [1] = ctx.mdb->readIEEEFloatLE();
	_mesh->diffuse [2] = ctx.mdb->readIEEEFloatLE();
	_mesh->specular[0] = ctx.mdb->readIEEEFloatLE();
	_mesh->specular[1] = ctx.mdb->readIEEEFloatLE();
	_mesh->specular[2] = ctx.mdb->readIEEEFloatLE();

	float  specularPower = ctx.mdb->readIEEEFloatLE();
	float  specularValue = ctx.mdb->readIEEEFloatLE();
	uint32_t textureFlags  = ctx.mdb->readUint32LE();

	uint32_t vertexCount = ctx.mdb->readUint32LE();
	uint32_t facesCount  = ctx.mdb->readUint32LE();

	if ((vertexCount == 0) || (facesCount == 0))
		return false;

	_render = _mesh->render = true;
	_mesh->data = new MeshData();
	_mesh->data->rawMesh = new Graphics::Mesh::Mesh();

	std::vector<Common::UString> textures;
	textures.push_back(diffuseMap);

	loadTextures(textures);

	// Read vertices (interleaved)

	VertexDecl vertexDecl;

	vertexDecl.push_back(VertexAttrib(VPOSITION, 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VNORMAL  , 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VTCOORD  , 3, GL_FLOAT));

	if (!_tintMap.empty())
		vertexDecl.push_back(VertexAttrib(VTCOORD + 1, 3, GL_FLOAT));

	_mesh->data->rawMesh->getVertexBuffer()->setVertexDeclInterleave(vertexCount, vertexDecl);

	float *v = reinterpret_cast<float *>(_mesh->data->rawMesh->getVertexBuffer()->getData());
	for (uint32_t i = 0; i < vertexCount; i++) {
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
		*v++ = ctx.mdb->readIEEEFloatLE();

		// TintMap TexCoords
		if (!_tintMap.empty()) {
			v[0] = v[-3];
			v[1] = v[-2];
			v[2] = v[-1];
			v += 3;
		}
	}


	// Read faces

	_mesh->data->rawMesh->getIndexBuffer()->setSize(facesCount * 3, sizeof(uint16_t), GL_UNSIGNED_SHORT);

	uint16_t *f = reinterpret_cast<uint16_t *>(_mesh->data->rawMesh->getIndexBuffer()->getData());
	for (uint32_t i = 0; i < facesCount * 3; i++)
		f[i] = ctx.mdb->readUint16LE();

	Common::UString meshName = ctx.mdlName;
	meshName += ".";
	if (ctx.state->name.size() != 0) {
		meshName += ctx.state->name;
	} else {
		meshName += "xoreos.default";
	}
	meshName += ".";
	meshName += _name;

	Graphics::Mesh::Mesh *checkMesh = MeshMan.getMesh(meshName);
	if (checkMesh) {
		warning("Warning: probable mesh duplication of: %s, attempting to correct", meshName.c_str());
		delete _mesh->data->rawMesh;
		_mesh->data->rawMesh = checkMesh;
	} else {
		_mesh->data->rawMesh->setName(meshName);
		_mesh->data->rawMesh->init();
		MeshMan.addMesh(_mesh->data->rawMesh);
	}

	createBound();

	return true;
}

bool ModelNode_NWN2::loadSkin(Model_NWN2::ParserContext &ctx) {
	uint32_t tag = ctx.mdb->readUint32BE();
	if (tag != kSkinID)
		throw Common::Exception("Invalid skin packet signature (%s)", Common::debugTag(tag).c_str());

	uint32_t packetSize = ctx.mdb->readUint32LE();

	_name = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);

	// Skipping lower level of detail models
	if (_name.endsWith("_L01") || _name.endsWith("_L02"))
		return false;

	Common::UString skeletonName = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);

	Common::UString diffuseMap = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);
	Common::UString normalMap  = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);
	                _tintMap   = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);
	Common::UString glowMap    = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);

	_mesh = new Mesh();

	_mesh->diffuse [0] = ctx.mdb->readIEEEFloatLE();
	_mesh->diffuse [1] = ctx.mdb->readIEEEFloatLE();
	_mesh->diffuse [2] = ctx.mdb->readIEEEFloatLE();
	_mesh->specular[0] = ctx.mdb->readIEEEFloatLE();
	_mesh->specular[1] = ctx.mdb->readIEEEFloatLE();
	_mesh->specular[2] = ctx.mdb->readIEEEFloatLE();

	float  specularPower = ctx.mdb->readIEEEFloatLE();
	float  specularValue = ctx.mdb->readIEEEFloatLE();
	uint32_t textureFlags  = ctx.mdb->readUint32LE();

	uint32_t vertexCount = ctx.mdb->readUint32LE();
	uint32_t facesCount  = ctx.mdb->readUint32LE();

	if ((vertexCount == 0) || (facesCount == 0))
		return false;

	_render = _mesh->render = true;
	_mesh->data = new MeshData();
	_mesh->data->rawMesh = new Graphics::Mesh::Mesh();

	std::vector<Common::UString> textures;
	textures.push_back(diffuseMap);

	loadTextures(textures);

	// Read vertices (interleaved)

	VertexDecl vertexDecl;

	vertexDecl.push_back(VertexAttrib(VPOSITION, 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VNORMAL  , 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VTCOORD  , 3, GL_FLOAT));

	if (!_tintMap.empty())
		vertexDecl.push_back(VertexAttrib(VTCOORD + 1, 3, GL_FLOAT));

	_mesh->data->rawMesh->getVertexBuffer()->setVertexDeclInterleave(vertexCount, vertexDecl);

	float *v = reinterpret_cast<float *>(_mesh->data->rawMesh->getVertexBuffer()->getData());
	for (uint32_t i = 0; i < vertexCount; i++) {
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
		*v++ = ctx.mdb->readIEEEFloatLE();

		// TintMap TexCoords
		if (!_tintMap.empty()) {
			v[0] = v[-3];
			v[1] = v[-2];
			v[2] = v[-1];
			v += 3;
		}

		ctx.mdb->skip(4); // Bone count
	}


	// Read faces
	_mesh->data->rawMesh->getIndexBuffer()->setSize(facesCount * 3, sizeof(uint16_t), GL_UNSIGNED_SHORT);

	uint16_t *f = reinterpret_cast<uint16_t *>(_mesh->data->rawMesh->getIndexBuffer()->getData());
	for (uint32_t i = 0; i < facesCount * 3; i++)
		f[i] = ctx.mdb->readUint16LE();

	createBound();

	Common::UString meshName = ctx.mdlName;
	meshName += ".";
	if (ctx.state->name.size() != 0) {
		meshName += ctx.state->name;
	} else {
		meshName += "xoreos.default";
	}
	meshName += ".";
	meshName += _name;

	Graphics::Mesh::Mesh *checkMesh = MeshMan.getMesh(meshName);
	if (checkMesh) {
		warning("Warning: probable mesh duplication of: %s, attempting to correct", meshName.c_str());
		delete _mesh->data->rawMesh;
		_mesh->data->rawMesh = checkMesh;
	} else {
		_mesh->data->rawMesh->setName(meshName);
		_mesh->data->rawMesh->init();
		MeshMan.addMesh(_mesh->data->rawMesh);
	}

	return true;
}

void ModelNode_NWN2::setTint(const float tint[3][4]) {
	if (!_mesh || !_mesh->data)
		return;

	lockFrameIfVisible();

	memcpy(_tint, tint, 3 * 4 * sizeof(float));

	removeTint();
	createTint();

	unlockFrameIfVisible();
}

void ModelNode_NWN2::removeTint() {
	if (_tintedMapIndex < 0)
		return;

	_mesh->data->textures.erase(_mesh->data->textures.begin() + _tintedMapIndex);

	_tintedMapIndex = -1;
}

/* Create a tinted texture by combining the tint map with the tint colors.
 *
 * This is currently all done here in software and has several drawbacks:
 * - Slow
 * - We're creating lots of uncompressed RBGA texture, so it
 *   takes up a lot of memory, both on the GPU and in system RAM
 *
 * TODO: We really need to do this in shaders in the future.
 */
void ModelNode_NWN2::createTint() {
	if (_tintMap.empty())
		return;

	ImageDecoder *tintMap   = 0;
	Surface      *tintedMap = 0;
	try {
		// Load and uncompress the texture
		tintMap = Texture::loadImage(_tintMap);
		if (tintMap->isCompressed())
			tintMap->decompress();

		const ImageDecoder::MipMap &tintImg = tintMap->getMipMap(0);

		// Create a new target surface with the same dimensions
		tintedMap = new Surface(tintImg.width, tintImg.height);
		ImageDecoder::MipMap &tintedImg = tintedMap->getMipMap();

		// Iterate over all pixels: read values, mix tints, write pixel to target surface
		for (int n = 0; n < tintImg.width * tintImg.height; n++) {
			float srcColor[4], dstColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			tintImg.getPixel(n, srcColor[0], srcColor[1], srcColor[2], srcColor[3]);

			if (srcColor[3] != 0.0f) {
				// Mix using the value and the alpha components as intensities
				// TODO: Verify how the mixing is actually done in NWN2!
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++)
						dstColor[j] += _tint[i][j] * srcColor[i] * _tint[i][3] * srcColor[3] * _mesh->diffuse[i];
				}
			} else
				// Source alpha is 0.0f: No tinting for this pixel
				for (int i = 0; i < 3; i++)
					dstColor[i] = _mesh->diffuse[i] * _tint[i][3];

			tintedImg.setPixel(n, dstColor[0], dstColor[1], dstColor[2], dstColor[3]);
		}

	} catch (...) {
		delete tintMap;
		delete tintedMap;
		return;
	}

	delete tintMap;

	// And add the new texture to the TextureManager
	TextureHandle tintedTexture = TextureMan.add(Texture::create(tintedMap));

	_mesh->data->textures.push_back(tintedTexture);
	_tintedMapIndex = _mesh->data->textures.size() - 1;
}

} // End of namespace Aurora

} // End of namespace Graphics
