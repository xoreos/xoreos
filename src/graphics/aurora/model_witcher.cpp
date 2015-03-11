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
 *  Loading MDB files found in The Witcher
 */

// Disable the "unused variable" warnings while most stuff is still stubbed
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/stream.h"
#include "src/common/encoding.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"

#include "src/graphics/aurora/model_witcher.h"

namespace Graphics {

namespace Aurora {

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

Model_Witcher::ParserContext::ParserContext(const Common::UString &name) : mdb(0), state(0) {
	mdb = ResMan.getResource(name, ::Aurora::kFileTypeMDB);
	if (!mdb)
		throw Common::Exception("No such MDB \"%s\"", name.c_str());
}

Model_Witcher::ParserContext::~ParserContext() {
	delete mdb;

	clear();
}

void Model_Witcher::ParserContext::clear() {
	for (std::list<ModelNode_Witcher *>::iterator n = nodes.begin(); n != nodes.end(); ++n)
		delete *n;
	nodes.clear();

	delete state;
	state = 0;
}


Model_Witcher::Model_Witcher(const Common::UString &name, ModelType type) : Model(type) {
	_fileName = name;

	ParserContext ctx(name);

	load(ctx);

	finalize();
}

Model_Witcher::~Model_Witcher() {
}

void Model_Witcher::load(ParserContext &ctx) {
	if (ctx.mdb->readByte() != 0) {
		ctx.mdb->seek(0);

		Common::UString type = Common::readString(*ctx.mdb, Common::kEncodingASCII);
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

	_name = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);

	uint32 offsetRootNode = ctx.mdb->readUint32LE();

	ctx.mdb->skip(32);

	byte type = ctx.mdb->readByte();

	ctx.mdb->skip(3);

	ctx.mdb->skip(48);

	float firstLOD = ctx.mdb->readIEEEFloatLE();
	float lastLOD  = ctx.mdb->readIEEEFloatLE();

	ctx.mdb->skip(16);

	Common::UString detailMap = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);

	ctx.mdb->skip(4);

	float scale = ctx.mdb->readIEEEFloatLE();

	Common::UString superModel = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);

	ctx.mdb->skip(16);

	newState(ctx);

	ModelNode_Witcher *rootNode = new ModelNode_Witcher(*this);
	ctx.nodes.push_back(rootNode);

	ctx.mdb->seek(ctx.offModelData + offsetRootNode);
	rootNode->load(ctx);

	addState(ctx);
}

void Model_Witcher::newState(ParserContext &ctx) {
	ctx.clear();

	ctx.state = new State;
}

void Model_Witcher::addState(ParserContext &ctx) {
	if (!ctx.state || ctx.nodes.empty()) {
		ctx.clear();
		return;
	}

	for (std::list<ModelNode_Witcher *>::iterator n = ctx.nodes.begin();
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


ModelNode_Witcher::ModelNode_Witcher(Model &model) : ModelNode(model) {
}

ModelNode_Witcher::~ModelNode_Witcher() {
}

void ModelNode_Witcher::load(Model_Witcher::ParserContext &ctx) {
	ctx.mdb->skip(24); // Function pointers

	uint32 inheritColor = ctx.mdb->readUint32LE();
	uint32 nodeNumber   = ctx.mdb->readUint32LE();

	_name = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);

	ctx.mdb->skip(8); // Parent pointers

	uint32 childrenOffset, childrenCount;
	Model::readArrayDef(*ctx.mdb, childrenOffset, childrenCount);

	std::vector<uint32> children;
	Model::readArray(*ctx.mdb, ctx.offModelData + childrenOffset, childrenCount, children);

	uint32 controllerKeyOffset, controllerKeyCount;
	Model::readArrayDef(*ctx.mdb, controllerKeyOffset, controllerKeyCount);

	uint32 controllerDataOffset, controllerDataCount;
	Model::readArrayDef(*ctx.mdb, controllerDataOffset, controllerDataCount);

	std::vector<float> controllerData;
	Model::readArray(*ctx.mdb, ctx.offModelData + controllerDataOffset,
	                 controllerDataCount, controllerData);

	readNodeControllers(ctx, ctx.offModelData + controllerKeyOffset,
	                    controllerKeyCount, controllerData);

	ctx.mdb->skip(4); // Unknown

	uint32 imposterGroup = ctx.mdb->readUint32LE();
	uint32 fixedRot      = ctx.mdb->readUint32LE();
	uint32 minLOD        = ctx.mdb->readUint32LE();
	uint32 maxLOD        = ctx.mdb->readUint32LE();

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


	for (std::vector<uint32>::const_iterator child = children.begin(); child != children.end(); ++child) {
		ModelNode_Witcher *childNode = new ModelNode_Witcher(*_model);
		ctx.nodes.push_back(childNode);

		childNode->setParent(this);

		ctx.mdb->seek(ctx.offModelData + *child);
		childNode->load(ctx);
	}
}

void ModelNode_Witcher::readMesh(Model_Witcher::ParserContext &ctx) {
	ctx.mdb->skip(4); // Function pointer
	ctx.mdb->skip(4); // Unknown

	uint32 offMeshArrays = ctx.mdb->readUint32LE();

	ctx.mdb->skip(4); // Unknown

	float boundingMin[3], boundingMax[3];

	boundingMin[0] = ctx.mdb->readIEEEFloatLE();
	boundingMin[1] = ctx.mdb->readIEEEFloatLE();
	boundingMin[2] = ctx.mdb->readIEEEFloatLE();

	boundingMax[0] = ctx.mdb->readIEEEFloatLE();
	boundingMax[1] = ctx.mdb->readIEEEFloatLE();
	boundingMax[2] = ctx.mdb->readIEEEFloatLE();

	ctx.mdb->skip(28); // Unknown

	float volFogScale = ctx.mdb->readIEEEFloatLE();

	ctx.mdb->skip(16); // Unknown

	_diffuse[0] = ctx.mdb->readIEEEFloatLE();
	_diffuse[1] = ctx.mdb->readIEEEFloatLE();
	_diffuse[2] = ctx.mdb->readIEEEFloatLE();
	_ambient[0] = ctx.mdb->readIEEEFloatLE();
	_ambient[1] = ctx.mdb->readIEEEFloatLE();
	_ambient[2] = ctx.mdb->readIEEEFloatLE();

	float textureTransRot[3];
	textureTransRot[0] = ctx.mdb->readIEEEFloatLE();
	textureTransRot[1] = ctx.mdb->readIEEEFloatLE();
	textureTransRot[2] = ctx.mdb->readIEEEFloatLE();

	_shininess = ctx.mdb->readIEEEFloatLE();

	_shadow  = ctx.mdb->readUint32LE() == 1;
	_beaming = ctx.mdb->readUint32LE() == 1;
	_render  = ctx.mdb->readUint32LE() == 1;

	_transparencyHint = ctx.mdb->readUint32LE() == 1;

	ctx.mdb->skip(4); // Unknown

	Common::UString texture[4];
	for (int t = 0; t < 4; t++) {
		texture[t] = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);

		if (texture[t] == "NULL")
			texture[t].clear();
	}

	bool tileFade = ctx.mdb->readUint32LE() == 1;

	bool controlFade   = ctx.mdb->readByte() == 1;
	bool lightMapped   = ctx.mdb->readByte() == 1;
	bool rotateTexture = ctx.mdb->readByte() == 1;

	ctx.mdb->skip(1); // Unknown

	float transparencyShift = ctx.mdb->readIEEEFloatLE();

	uint32 defaultRenderList = ctx.mdb->readUint32LE();
	uint32 preserveVColors   = ctx.mdb->readUint32LE();

	uint32 fourCC = ctx.mdb->readUint32BE();

	ctx.mdb->skip(4); // Unknown

	float depthOffset       = ctx.mdb->readIEEEFloatLE();
	float coronaCenterMult  = ctx.mdb->readIEEEFloatLE();
	float fadeStartDistance = ctx.mdb->readIEEEFloatLE();

	bool distFromScreenCenterFace = ctx.mdb->readByte() == 1;
	ctx.mdb->skip(3); // Unknown

	float enlargeStartDistance = ctx.mdb->readIEEEFloatLE();

	bool affectedByWind = ctx.mdb->readByte() == 1;
	ctx.mdb->skip(3); // Unknown

	float dampFactor = ctx.mdb->readIEEEFloatLE();

	uint32 blendGroup = ctx.mdb->readUint32LE();

	bool dayNightLightMaps = ctx.mdb->readByte() == 1;

	Common::UString dayNightTransition = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 4);

	ctx.mdb->skip(196); // Unknown

	bool ignoreHitCheck  = ctx.mdb->readByte() == 1;
	bool needsReflection = ctx.mdb->readByte() == 1;
	ctx.mdb->skip(1); // Unknown

	float reflectionPlaneNormal[3];
	reflectionPlaneNormal[0] = ctx.mdb->readIEEEFloatLE();
	reflectionPlaneNormal[1] = ctx.mdb->readIEEEFloatLE();
	reflectionPlaneNormal[2] = ctx.mdb->readIEEEFloatLE();

	float reflectionPlaneDistance = ctx.mdb->readIEEEFloatLE();

	bool fadeOnCameraCollision = ctx.mdb->readByte() == 1;
	bool noSelfShadow          = ctx.mdb->readByte() == 1;
	bool isReflected           = ctx.mdb->readByte() == 1;
	bool onlyReflected         = ctx.mdb->readByte() == 1;

	Common::UString lightMapName = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);

	bool canDecal            = ctx.mdb->readByte() == 1;
	bool multiBillBoard      = ctx.mdb->readByte() == 1;
	bool ignoreLODReflection = ctx.mdb->readByte() == 1;
	ctx.mdb->skip(1); // Unknown

	float detailMapScape = ctx.mdb->readIEEEFloatLE();

	ctx.offTextureInfo = ctx.mdb->readUint32LE();

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

	uint32 tVertsOffset[4], tVertsCount[4];
	for (uint t = 0; t < 4; t++)
		Model::readArrayDef(*ctx.mdb, tVertsOffset[t], tVertsCount[t]);

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
	readTextures(ctx, textures);

	evaluateTextures(4, textures, texture, tVertsCount, dayNightLightMaps, lightMapName);

	loadTextures(textures);

	uint texCount = textures.size();

	// Read vertices

	GLsizei vpsize = 3;
	GLsizei vnsize = 3;
	GLsizei vtsize = 2;
	uint32 vertexSize = (vpsize + vnsize + vtsize * texCount) * sizeof(float);
	_vertexBuffer.setSize(vertexCount, vertexSize);

	float *vertexData = (float *) _vertexBuffer.getData();
	VertexDecl vertexDecl;

	VertexAttrib vp;
	vp.index = VPOSITION;
	vp.size = vpsize;
	vp.type = GL_FLOAT;
	vp.stride = 0;
	vp.pointer = vertexData;
	vertexDecl.push_back(vp);

	VertexAttrib vn;
	vn.index = VNORMAL;
	vn.size = vnsize;
	vn.type = GL_FLOAT;
	vn.stride = 0;
	vn.pointer = vertexData + vpsize * vertexCount;
	vertexDecl.push_back(vn);

	VertexAttrib vt[4];
	for (uint t = 0; t < texCount; t++) {
		vt[t].index = VTCOORD + t;
		vt[t].size = vtsize;
		vt[t].type = GL_FLOAT;
		vt[t].stride = 0;
		vt[t].pointer = vertexData + (vpsize + vnsize + t * vtsize) * vertexCount;
		vertexDecl.push_back(vt[t]);
	}

	_vertexBuffer.setVertexDecl(vertexDecl);

	// Read vertex position
	ctx.mdb->seekTo(ctx.offRawData + vertexOffset);
	float *v = (float *) vp.pointer;
	for (uint32 i = 0; i < vertexCount; i++) {
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
	}

	// Read vertex normals
	assert(normalsCount == vertexCount);
	ctx.mdb->seekTo(ctx.offRawData + normalsOffset);
	v = (float *) vn.pointer;
	for (uint32 i = 0; i < normalsCount; i++) {
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
	}

	// Read texture coordinates
	for (uint t = 0; t < texCount; t++) {

		ctx.mdb->seekTo(ctx.offRawData + tVertsOffset[t]);
		v = (float *) vt[t].pointer;
		for (uint32 i = 0; i < tVertsCount[t]; i++) {
			if (i < tVertsCount[t]) {
				*v++ = ctx.mdb->readIEEEFloatLE();
				*v++ = ctx.mdb->readIEEEFloatLE();
			} else {
				*v++ = 0.0;
				*v++ = 0.0;
			}
		}
	}


	// Read faces

	_indexBuffer.setSize(facesCount * 3, sizeof(uint32), GL_UNSIGNED_INT);

	ctx.mdb->seekTo(ctx.offRawData + facesOffset);
	uint32 *f = (uint32 *) _indexBuffer.getData();
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

	createBound();

	ctx.mdb->seekTo(endPos);
}

void ModelNode_Witcher::readTextures(Model_Witcher::ParserContext &ctx,
                                     std::vector<Common::UString> &textures) {

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
		*line = Common::readStringLine(*ctx.mdb, Common::kEncodingASCII);
		ctx.mdb->skip(1);

		line->trim();
	}

	textures.resize(4);

	int hasShaderTex = false;

	for (std::vector<Common::UString>::const_iterator line = textureLine.begin(); line != textureLine.end(); ++line) {
		int s = -1;
		int n =  0;

		if        (line->beginsWith("texture texture0 ")) {
			s = 17;
			n =  0 + (hasShaderTex ? 1 : 0);
		} else if (line->beginsWith("texture texture1 ")) {
			s = 17;
			n =  1 + (hasShaderTex ? 1 : 0);
		} else if (line->beginsWith("texture texture2 ")) {
			s = 17;
			n =  2 + (hasShaderTex ? 1 : 0);
		} else if (line->beginsWith("texture texture3 ")) {
			s = 17;
			n =  3;
		} else if (line->beginsWith("texture tex ")) {
			s = 12;
			n =  0 + (hasShaderTex ? 1 : 0);
		} else if (line->beginsWith("shader ")) {
			hasShaderTex = true;

			Common::UString shader = line->substr(line->getPosition(7), line->end());
			if ((shader == "dadd_al_mul_alp") ||
			    (shader == "corona") ||
			    (shader == "normalmap") ||
			    (shader == "norm_env_rim_ao") ||
			    (shader == "transparency_2ps") ||
			    (shader == "skin_n_rim_ao"))
				hasShaderTex = false;
		}

		if (s != -1)
			textures[n] = line->substr(line->getPosition(s), line->end());
	}

}

void ModelNode_Witcher::evaluateTextures(int n, std::vector<Common::UString> &textures,
		const Common::UString *staticTextures, const uint32 *tVertsCount,
		bool lightMapDayNight, const Common::UString &lightMapName) {

	textures.resize(n);

	for (int t = 0; t < n; t++) {
		if (textures[t].empty())
			textures[t] = staticTextures[t];

		if (tVertsCount[t] == 0)
			textures[t].clear();

		if (textures[t].empty())
			continue;

		if (lightMapDayNight && (textures[t] == lightMapName)) {
			// Day (dzień)
			if      (ResMan.hasResource(textures[t] + "!d", ::Aurora::kResourceImage))
				textures[t] += "!d";
			// Morning (rano)
			else if (ResMan.hasResource(textures[t] + "!r", ::Aurora::kResourceImage))
				textures[t] += "!r";
			// Noon (południe)
			else if (ResMan.hasResource(textures[t] + "!p", ::Aurora::kResourceImage))
				textures[t] += "!p";
			// Evening (wieczór)
			else if (ResMan.hasResource(textures[t] + "!w", ::Aurora::kResourceImage))
				textures[t] += "!w";
			// Night (noc)
			else if (ResMan.hasResource(textures[t] + "!n", ::Aurora::kResourceImage))
				textures[t] += "!n";
			else
				textures[t].clear();
		}
	}

	while (!textures.empty() && textures.back().empty())
		textures.pop_back();
}

void ModelNode_Witcher::readNodeControllers(Model_Witcher::ParserContext &ctx,
		uint32 offset, uint32 count, std::vector<float> &data) {

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

			_position[0] = data[dataIndex + 0];
			_position[1] = data[dataIndex + 1];
			_position[2] = data[dataIndex + 2];

		} else if (type == kControllerTypeOrientation) {
			if (columnCount != 4)
				throw Common::Exception("Orientation controller with %d values", columnCount);

			_orientation[0] = data[dataIndex + 0];
			_orientation[1] = data[dataIndex + 1];
			_orientation[2] = data[dataIndex + 2];
			_orientation[3] = Common::rad2deg(acos(data[dataIndex + 3]) * 2.0);
		}

	}

	ctx.mdb->seekTo(pos);
}

} // End of namespace Aurora

} // End of namespace Graphics
