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
 *  Loading MDL/MDX files found in Jade Empire
 */

/* Based in parts on cchargin's KotOR model specs
 * (<https://home.comcast.net/~cchargin/kotor/mdl_info.html>).
 */

#include <cstring>

#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"

#include "src/graphics/aurora/model_jade.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/graphics/shader/materialman.h"
#include "src/graphics/shader/surfaceman.h"

#include "src/graphics/render/renderman.h"

#include "src/graphics/images/decoder.h"

// This is included if a mesh wants a unique name.
#include "src/common/uuid.h"

// Disable the "unused variable" warnings while most stuff is still stubbed
IGNORE_UNUSED_VARIABLES

enum NodeType {
	kNodeTypeNode             = 0x00000001,
	kNodeTypeLight            = 0x00000003,
	kNodeTypeEmitter          = 0x00000005,
	kNodeTypeCamera           = 0x00000009,
	kNodeTypeReference        = 0x00000011,
	kNodeTypeTrimesh          = 0x00000021,
	kNodeTypeSkin             = 0x00000061,
	kNodeTypeAABB             = 0x00000221,
	kNodeTypeWeaponTrail      = 0x00000821,
	kNodeTypeGob              = 0x00001001,
	kNodeTypeCloth            = 0x00004021,
	kNodeTypeCollisionSphere  = 0x00006001,
	kNodeTypeCollisionCapsule = 0x0000A001,
	kNodeTypeDanglyBone       = 0x00020001,
	kNodeTypeCollisionLozenge = 0x00022001,
	kNodeTypeUnknown          = 0x00040001
};

enum NodeTypeFeature {
	kNodeTypeHasHeader = 0x00000001,
	kNodeTypeHasMesh   = 0x00000020,
	kNodeTypeHasSkin   = 0x00000040,
	kNodeTypeHasAABB   = 0x00000200
};

enum NodeFlag {
	kNodeFlagsAnimatedUV         = 1 << 0,
	kNodeFlagsLightmapped        = 1 << 1,
	kNodeFlagsBackgroundGeometry = 1 << 2,
	kNodeFlagsBeaming            = 1 << 3,
	kNodeFlagsRender             = 1 << 4
};

namespace Graphics {

namespace Aurora {

Model_Jade::ParserContext::ParserContext(const Common::UString &name,
                                         const Common::UString &t) :
	mdl(0), mdx(0), state(0), texture(t) {

	try {

		if (!(mdl = ResMan.getResource(name, ::Aurora::kFileTypeMDL)))
			throw Common::Exception("No such MDL \"%s\"", name.c_str());
		if (!(mdx = ResMan.getResource(name, ::Aurora::kFileTypeMDX)))
			throw Common::Exception("No such MDX \"%s\"", name.c_str());

	} catch (...) {
		delete mdl;
		delete mdx;
		throw;
	}
}

Model_Jade::ParserContext::~ParserContext() {
	delete mdl;
	delete mdx;

	clear();
}

void Model_Jade::ParserContext::clear() {
	for (std::list<ModelNode_Jade *>::iterator n = nodes.begin(); n != nodes.end(); ++n)
		delete *n;
	nodes.clear();

	delete state;
	state = 0;

	newNode();
}

void Model_Jade::ParserContext::newNode() {
	vertices.clear();
	indices.clear();

	textures.clear();

	texCoords.clear();
}


Model_Jade::Model_Jade(const Common::UString &name, ModelType type, const Common::UString &texture) :
	Model(type) {

	_fileName = name;

	ParserContext ctx(name, texture);

	load(ctx);

	finalize();
}

Model_Jade::~Model_Jade() {
}

void Model_Jade::load(ParserContext &ctx) {
	/* Magic and version number:
	 *
	 * - First byte must be 0x00
	 * - Third byte version:
	 *   - Upper bit PC (1) / Xbox (0)
	 *   - Lower 7 bits version number
	 *
	 * We only support version 7 of the PC version.
	 */
	uint32_t version = ctx.mdl->readUint32BE();
	if (version != 0x00008700)
		throw Common::Exception("Unsupported MDL: 0x%08X", version);

	ctx.offModelData = 20;

	// Size of the MDL file, without the 20 byte header
	ctx.mdlSize = ctx.mdl->readUint32LE();

	// Size of the vertices part of the MDX file
	ctx.mdxSizeVertices = ctx.mdl->readUint32LE();
	// Size of the faces part of the MDX file
	ctx.mdxSizeFaces    = ctx.mdl->readUint32LE();
	// Size of a third part of the MDX file, always 0?
	ctx.mdxSize3        = ctx.mdl->readUint32LE();

	if (ctx.mdxSize3 != 0)
		warning("Model_Jade: Model \"%s\" mdxSize3 == %d", _fileName.c_str(), ctx.mdxSize3);

	ctx.mdl->skip(8); // Function pointers

	_name = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32);
	ctx.mdlName = _name;

	uint32_t nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32_t nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24); // Unknown
	ctx.mdl->skip( 4); // Pointer to the MDL file

	uint8_t type = ctx.mdl->readByte();

	ctx.mdl->skip(3); // Padding
	ctx.mdl->skip(4); // Unknown
	ctx.mdl->skip(4); // Reference count

	ctx.mdl->skip(12); // TODO: Animation Header Pointer Array

	ctx.mdl->skip(4); // Pointer to the super model

	float boundingMin[3], boundingMax[3];

	boundingMin[0] = ctx.mdl->readIEEEFloatLE();
	boundingMin[1] = ctx.mdl->readIEEEFloatLE();
	boundingMin[2] = ctx.mdl->readIEEEFloatLE();

	boundingMax[0] = ctx.mdl->readIEEEFloatLE();
	boundingMax[1] = ctx.mdl->readIEEEFloatLE();
	boundingMax[2] = ctx.mdl->readIEEEFloatLE();

	float radius = ctx.mdl->readIEEEFloatLE();

	ctx.mdl->skip(4); // Unknown

	float modelScale = ctx.mdl->readIEEEFloatLE();

	Common::UString superModelName = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32);

	ctx.mdl->skip( 4); // Pointer to some node
	ctx.mdl->skip(12); // Unknown
	ctx.mdl->skip( 4); // Pointer to the MDX file

	uint32_t nameOffset, nameCount;
	readArrayDef(*ctx.mdl, nameOffset, nameCount);

	std::vector<uint32_t> nameOffsets;
	readArray(*ctx.mdl, ctx.offModelData + nameOffset, nameCount, nameOffsets);

	readStrings(*ctx.mdl, nameOffsets, ctx.offModelData, ctx.names);

	newState(ctx);

	ModelNode_Jade *rootNode = new ModelNode_Jade(*this);
	ctx.nodes.push_back(rootNode);

	ctx.mdl->seek(ctx.offModelData + nodeHeadPointer);
	rootNode->load(ctx);

	addState(ctx);
}

void Model_Jade::readStrings(Common::SeekableReadStream &mdl,
		const std::vector<uint32_t> &offsets, uint32_t offset,
		std::vector<Common::UString> &strings) {

	size_t pos = mdl.pos();

	strings.reserve(offsets.size());
	for (std::vector<uint32_t>::const_iterator o = offsets.begin(); o != offsets.end(); ++o) {
		mdl.seek(offset + *o);

		strings.push_back(Common::readString(mdl, Common::kEncodingASCII));
	}

	mdl.seek(pos);
}

void Model_Jade::newState(ParserContext &ctx) {
	ctx.clear();

	ctx.state = new State;
}

void Model_Jade::addState(ParserContext &ctx) {
	if (!ctx.state || ctx.nodes.empty()) {
		ctx.clear();
		return;
	}

	for (std::list<ModelNode_Jade *>::iterator n = ctx.nodes.begin();
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


ModelNode_Jade::ModelNode_Jade(Model &model) : ModelNode(model) {
}

ModelNode_Jade::~ModelNode_Jade() {
}

void ModelNode_Jade::load(Model_Jade::ParserContext &ctx) {
	uint32_t type = ctx.mdl->readUint32LE();

	// Node number in tree order
	uint16_t nodeNumber1 = ctx.mdl->readUint16LE();

	// Sequentially node number as found in the file
	uint16_t nodeNumber2 = ctx.mdl->readUint16LE();

	if (nodeNumber2 < ctx.names.size())
		_name = ctx.names[nodeNumber2];

	ctx.mdl->skip(4); // Pointer to the MDL file
	ctx.mdl->skip(4); // Pointer to the parent Model

	_position   [0] = ctx.mdl->readIEEEFloatLE();
	_position   [1] = ctx.mdl->readIEEEFloatLE();
	_position   [2] = ctx.mdl->readIEEEFloatLE();
	_orientation[3] = Common::rad2deg(acos(ctx.mdl->readIEEEFloatLE()) * 2.0f);
	_orientation[0] = ctx.mdl->readIEEEFloatLE();
	_orientation[1] = ctx.mdl->readIEEEFloatLE();
	_orientation[2] = ctx.mdl->readIEEEFloatLE();

	uint32_t childrenOffset = ctx.mdl->readUint32LE();
	uint32_t childrenCount  = ctx.mdl->readUint32LE();

	float nodeScale       = ctx.mdl->readIEEEFloatLE();
	float maxAnimDistance = ctx.mdl->readIEEEFloatLE();

	std::vector<uint32_t> children;
	Model::readArray(*ctx.mdl, ctx.offModelData + childrenOffset, childrenCount, children);

	if (type & kNodeTypeHasMesh) {
		readMesh(ctx);
		createMesh(ctx);
	}

	for (std::vector<uint32_t>::const_iterator child = children.begin(); child != children.end(); ++child) {
		ModelNode_Jade *childNode = new ModelNode_Jade(*_model);
		ctx.nodes.push_back(childNode);
		ctx.newNode();

		childNode->setParent(this);

		ctx.mdl->seek(ctx.offModelData + *child);
		childNode->load(ctx);
	}

	Common::UString meshName = ctx.mdlName;
	meshName += ".";
	if (ctx.state->name.size() != 0) {
		meshName += ctx.state->name;
	} else {
		meshName += "xoreos.default";
	}
	meshName += ".";
	meshName += _name;

	if (!_mesh) {
		return;
	}

	if (!_mesh->data) {
		return;
	}

	if (!_mesh->data->rawMesh) {
		return;
	}

	_mesh->data->rawMesh->init();
	if (MeshMan.getMesh(meshName)) {
		warning("Warning: probable mesh duplication of: %s", meshName.c_str());

		// TODO: figure out the right thing to handle mesh duplication.
		meshName += "#" + Common::generateIDRandomString();
	}
	_mesh->data->rawMesh->setName(meshName);
	MeshMan.addMesh(_mesh->data->rawMesh);

	if (GfxMan.isRendererExperimental())
		buildMaterial();
}

void ModelNode_Jade::buildMaterial() {
	ModelNode::buildMaterial();
}

void ModelNode_Jade::readMesh(Model_Jade::ParserContext &ctx) {
	ctx.mdl->skip(12); // Unknown

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

	_mesh = new Mesh();

	uint32_t transparencyHint = ctx.mdl->readUint32LE();
	uint16_t flags            = ctx.mdl->readUint16LE();

	_mesh->shadow = ctx.mdl->readUint16LE() != 0;

	_mesh->render  = (flags & kNodeFlagsRender) != 0;
	_mesh->beaming = (flags & kNodeFlagsBeaming) != 0;
	_mesh->isBackgroundGeometry = (flags & kNodeFlagsBackgroundGeometry) != 0;

	_mesh->hasTransparencyHint = true;
	_mesh->transparencyHint    = (transparencyHint == 1);
	_mesh->transparencyHintFull = transparencyHint;

	Common::UString texture = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32);

	uint32_t indexCount = ctx.mdl->readUint32LE();

	// Offset of the face indices into the MDL. If 0, use faceOffsetMDX.
	uint32_t faceOffsetMDL = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4); // Unknown

	// Type of the mesh:
	// - 0: Point list?
	// - 1: Line list?
	// - 2: Line strip?
	// - 3: Triangle list
	// - 4: Triangle strip
	// - 5: Triangle fan
	// - 6: ???
	uint32_t meshType = ctx.mdl->readUint32LE();

	ctx.mdl->skip(12); // Unknown

	uint32_t mdxStructSize = ctx.mdl->readUint32LE();

	ctx.mdl->skip(8); // Unknown

	uint32_t offNormals = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4); // Unknown

	uint32_t offUV[4];
	offUV[0] = ctx.mdl->readUint32LE();
	offUV[1] = ctx.mdl->readUint32LE();
	offUV[2] = ctx.mdl->readUint32LE();
	offUV[3] = ctx.mdl->readUint32LE();

	ctx.mdl->skip(20); // Unknown

	uint16_t vertexCount  = ctx.mdl->readUint16LE();
	uint16_t textureCount = MIN<uint16_t>(ctx.mdl->readUint16LE(), 4);

	uint32_t vertexOffset = ctx.mdl->readUint32LE();
	ctx.mdl->skip(4); // Unknown

	uint32_t materialID = ctx.mdl->readUint32LE();

	// Group id is likely used to select an appropriate shader.
	uint32_t materialGroupID = ctx.mdl->readUint32LE();

	_mesh->selfIllum[0] = ctx.mdl->readIEEEFloatLE();
	_mesh->selfIllum[1] = ctx.mdl->readIEEEFloatLE();
	_mesh->selfIllum[2] = ctx.mdl->readIEEEFloatLE();

	_mesh->alpha = ctx.mdl->readIEEEFloatLE();

	float textureWCoords = ctx.mdl->readIEEEFloatLE();

	ctx.mdl->skip(4); // Unknown

	// Offset of the face indices into the MDX. If 0, use faceOffsetMDL.
	uint32_t faceOffsetMDX = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4); // Unknown


	// Load textures

	/* If we were given a valid materialID, load textures from that material.
	 * Otherwise, try the texture that was given directly.
	 */
	if (materialID != 0xFFFFFFFF)
		readMaterialTextures(materialID, ctx.textures);
	else if (!texture.empty())
		ctx.textures.push_back(texture);

	textureCount = ctx.textures.size();


	// Read vertices

	ctx.vertices.resize(vertexCount * 3);

	ctx.texCoords.resize(textureCount);
	for (uint32_t i = 0; i < textureCount; i++)
		ctx.texCoords[i].resize(vertexCount * 2);

	// TODO: Figure out the correct layout of the vertex struct
	for (uint32_t i = 0; i < vertexCount; i++) {
		ctx.mdx->seek(vertexOffset + i * mdxStructSize);

		ctx.vertices[i * 3 + 0] = ctx.mdx->readIEEEFloatLE();
		ctx.vertices[i * 3 + 1] = ctx.mdx->readIEEEFloatLE();
		ctx.vertices[i * 3 + 2] = ctx.mdx->readIEEEFloatLE();

		for (uint32_t t = 0; t < textureCount; t++) {
			if ((offUV[t] != 0xFFFFFFFF) && ((offUV[t] + 8) <= mdxStructSize)) {
				ctx.mdx->seek(vertexOffset + i * mdxStructSize + offUV[t]);

				ctx.texCoords[t][i * 2 + 0] = ctx.mdx->readIEEEFloatLE();
				ctx.texCoords[t][i * 2 + 1] = ctx.mdx->readIEEEFloatLE();
			} else {
				ctx.texCoords[t][i * 2 + 0] = 0.0f;
				ctx.texCoords[t][i * 2 + 1] = 0.0f;
			}
		}
	}


	// Read face indices

	if      (faceOffsetMDL != 0)
		readPlainIndices  (*ctx.mdl, ctx.indices, faceOffsetMDL + ctx.offModelData, indexCount);
	else if (faceOffsetMDX != 0)
		readChunkedIndices(*ctx.mdx, ctx.indices, faceOffsetMDX, indexCount);

	unfoldFaces(ctx.indices, meshType);
}

void ModelNode_Jade::readPlainIndices(Common::SeekableReadStream &stream, std::vector<uint16_t> &indices,
                                      uint32_t offset, uint32_t count) {

	size_t pos = stream.pos();

	stream.seek(offset);

	indices.resize(count);
	for (std::vector<uint16_t>::iterator i = indices.begin(); i != indices.end(); ++i)
		*i = stream.readUint16LE();

	stream.seek(pos);
}

void ModelNode_Jade::readChunkedIndices(Common::SeekableReadStream &stream, std::vector<uint16_t> &indices,
                                        uint32_t offset, uint32_t count) {

	size_t pos = stream.pos();

	stream.seek(offset);

	uint32_t stopValue = stream.readUint32LE();
	stream.skip(4); // Unknown

	indices.reserve(count);

	while (count > 0) {
		uint32_t chunk = stream.readUint32LE();
		if (chunk == stopValue)
			break;

		uint32_t chunkLength = ((chunk >> 16) & 0x1FFF) / 2;
		uint32_t toRead = MIN(chunkLength, count);

		for (uint32_t i = 0; i < toRead; i++)
			indices.push_back(stream.readUint16LE());

		count -= toRead;
	}

	stream.seek(pos);
}

/** Unfolds triangle strips / fans into triangle lists. */
void ModelNode_Jade::unfoldFaces(std::vector<uint16_t> &indices, uint32_t meshType) {
	switch (meshType) {
		case 0: // Point list?
		case 1: // Line list?
		case 2: // Line strip?
		case 6: // ???
		default:
			warning("ModelNode_Jade \"%s\".\"%s\": Unsupported mesh type %d",
			        _model->getName().c_str(), _name.c_str(), meshType);
			indices.clear();
			break;

		case 3: // Triangle list
			break;

		case 4: // Triangle strip
			unfoldTriangleStrip(indices);
			break;

		case 5: // Triangle fan
			unfoldTriangleFan(indices);
			break;
	}
}

void ModelNode_Jade::unfoldTriangleStrip(std::vector<uint16_t> &indices) {
	if (indices.size() < 3) {
		indices.clear();
		return;
	}

	std::vector<uint16_t> unfolded;
	unfolded.reserve((indices.size() - 2) * 3);

	for (size_t i = 0; i < indices.size() - 2; i++) {
		if (i & 1) {
			unfolded.push_back(indices[i]);
			unfolded.push_back(indices[i + 2]);
			unfolded.push_back(indices[i + 1]);
		} else {
			unfolded.push_back(indices[i]);
			unfolded.push_back(indices[i + 1]);
			unfolded.push_back(indices[i + 2]);
		}
	}

	indices.swap(unfolded);
}

void ModelNode_Jade::unfoldTriangleFan(std::vector<uint16_t> &indices) {
	if (indices.size() < 3) {
		indices.clear();
		return;
	}

	std::vector<uint16_t> unfolded;
	unfolded.reserve((indices.size() - 2) * 3);

	for (size_t i = 1; i < indices.size() - 1; i++) {
		unfolded.push_back(indices[0]);
		unfolded.push_back(indices[i]);
		unfolded.push_back(indices[i + 1]);
	}

	indices.swap(unfolded);
}

void ModelNode_Jade::createMesh(Model_Jade::ParserContext &ctx) {
	const uint32_t vertexCount  = ctx.vertices.size() / 3;
	const uint32_t indexCount   = ctx.indices.size();
	const uint32_t textureCount = ctx.texCoords.size();
	if ((vertexCount == 0) || (indexCount == 0))
		return;

	_render = _mesh->render;
	_mesh->data = new MeshData();
	_mesh->data->rawMesh = new Graphics::Mesh::Mesh();

	loadTextures(ctx.textures);

	// Create the VertexBuffer / IndexBuffer

	VertexDecl vertexDecl;

	vertexDecl.push_back(VertexAttrib(VPOSITION, 3, GL_FLOAT));
	for (uint t = 0; t < textureCount; t++)
		vertexDecl.push_back(VertexAttrib(VTCOORD + t , 2, GL_FLOAT));

	_mesh->data->rawMesh->getVertexBuffer()->setVertexDeclInterleave(vertexCount, vertexDecl);

	float *v = reinterpret_cast<float *>(_mesh->data->rawMesh->getVertexBuffer()->getData());
	for (uint32_t i = 0; i < vertexCount; i++) {
		// Position
		*v++ = ctx.vertices[i * 3 + 0];
		*v++ = ctx.vertices[i * 3 + 1];
		*v++ = ctx.vertices[i * 3 + 2];

		// Texture coordinates
		for (uint t = 0; t < textureCount; t++) {
			*v++ = ctx.texCoords[t][i * 2 + 0];
			*v++ = ctx.texCoords[t][i * 2 + 1];
		}
	}

	_mesh->data->rawMesh->getIndexBuffer()->setSize(indexCount, sizeof(uint16_t), GL_UNSIGNED_SHORT);

	uint16_t *f = reinterpret_cast<uint16_t *>(_mesh->data->rawMesh->getIndexBuffer()->getData());
	memcpy(f, &ctx.indices[0], indexCount * sizeof(uint16_t));

	createBound();
}

/** Opens the resource for the materialID and parses it to return the 4 normal textures.
 *
 *  TODO: Proper material support.
 */
void ModelNode_Jade::readMaterialTextures(uint32_t materialID, std::vector<Common::UString> &textures) {
	if (materialID == 0xFFFFFFFF) {
		textures.clear();
		return;
	}

	Common::UString mabFile = Common::UString::format("%d", materialID);
	Common::SeekableReadStream *mab = ResMan.getResource(mabFile, ::Aurora::kFileTypeMAB);
	if (!mab) {
		textures.clear();
		return;
	}

	textures.reserve(4);

	try {
		uint32_t size = mab->readUint32LE();
		if (size != 292)
			throw Common::Exception("Invalid size in binary material %s.mab", mabFile.c_str());

		_jadeMaterialData.renderPathID = mab->readUint32LE();

		_jadeMaterialData.opacity1 = mab->readUint32LE();
		_jadeMaterialData.opacity2 = mab->readUint32LE();

		_jadeMaterialData.cubeMultiplier         = mab->readIEEEFloatLE();
		_jadeMaterialData.bumpCoordMultiplier    = mab->readIEEEFloatLE();
		_jadeMaterialData.terrainCoordMultiplier = mab->readIEEEFloatLE();

		_jadeMaterialData.falloff = mab->readIEEEFloatLE();

		_jadeMaterialData.waterAlpha = mab->readIEEEFloatLE();

		_jadeMaterialData.bumpMapIsSpecular = mab->readByte();
		_jadeMaterialData.doubleSided       = mab->readByte();

		mab->skip(2); // Unknown, padding?

		_jadeMaterialData.diffuseColor[0] = mab->readIEEEFloatLE();
		_jadeMaterialData.diffuseColor[1] = mab->readIEEEFloatLE();
		_jadeMaterialData.diffuseColor[2] = mab->readIEEEFloatLE();
		_jadeMaterialData.ambientColor[0] = mab->readIEEEFloatLE();
		_jadeMaterialData.ambientColor[1] = mab->readIEEEFloatLE();
		_jadeMaterialData.ambientColor[2] = mab->readIEEEFloatLE();

		mab->skip(24); // Unknown

		_jadeMaterialData.blending1 = mab->readUint32LE();
		_jadeMaterialData.blending2 = mab->readUint32LE();

		mab->skip(4); // Unknown

		for (int i = 0; i < 4; i++) {
			textures.push_back(Common::readStringFixed(*mab, Common::kEncodingASCII, 32));

			if (textures.back() == "NULL")
				textures.back().clear();
		}

	} catch (...) {
		delete mab;
		textures.clear();

		Common::exceptionDispatcherWarning();
		return;
	}

	delete mab;

	while (!textures.empty() && textures.back().empty())
		textures.pop_back();
}

} // End of namespace Aurora

} // End of namespace Graphics
