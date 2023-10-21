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

/* Based in parts on the binary MDL specs by Torlack
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/torlack>)
 * and the code of Michael_DarkAngel's twMax importer script
 * (<http://www.tbotr.net/modules.php?mod=Downloads&op=download&sid=3&ssid=3&dlid=19>).
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"

#include "src/graphics/aurora/model_witcher.h"

#include "src/graphics/shader/materialman.h"
#include "src/graphics/shader/surfaceman.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/graphics/images/decoder.h"

#include "src/common/uuid.h"

// Disable the "unused variable" warnings while most stuff is still stubbed
IGNORE_UNUSED_VARIABLES

namespace Graphics {

namespace Aurora {

enum NodeType {
	kNodeTypeNode         = 0x00000001,
	kNodeTypeLight        = 0x00000003,
	kNodeTypeEmitter      = 0x00000005,
	kNodeTypeCamera       = 0x00000009,
	kNodeTypeReference    = 0x00000011,
	kNodeTypeTrimesh      = 0x00000021,
	kNodeTypeSkin         = 0x00000061,
	kNodeTypeAABB         = 0x00000221,
	kNodeTypeTrigger      = 0x00000421,
	kNodeTypeSectorInfo   = 0x00001001,
	kNodeTypeWalkmesh     = 0x00002001,
	kNodeTypeDanglyNode   = 0x00004001,
	kNodeTypeTexturePaint = 0x00008001,
	kNodeTypeSpeedTree    = 0x00010001,
	kNodeTypeChain        = 0x00020001,
	kNodeTypeCloth        = 0x00040001
};

enum NodeControllerType {
	kNodeControllerTypePosition    =  84,
	kNodeControllerTypeOrientation =  96,
	kNodeControllerTypeScale       = 184,
	kNodeControllerTypeColor       = 248,  // 0x00F8. Seems a reasonably safe assumption.
	kNodeControllerTypeRadius      = 340,  // 0x0154. A guess, probably a safe one.
};

enum NodeTrimeshControllerType {
	kNodeTrimeshControllerTypeSelfIllumColor = 276,
	kNodeTrimeshControllerTypeAlpha          = 292
};


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
	unsigned int indent = 0;
void Model_Witcher::load(ParserContext &ctx) {
	if (ctx.mdb->readByte() != 0) {
		ctx.mdb->seek(0);

		Common::UString type = Common::readString(*ctx.mdb, Common::kEncodingASCII);
		if (type.beginsWith("binarycompositemodel"))
			throw Common::Exception("TODO: binarycompositemodel");

		throw Common::Exception("Not a The Witcher MDB file");
	}

	ctx.mdb->seek(4);

	ctx.fileVersion = ctx.mdb->readUint32LE() & 0x0FFFFFFF;
	if ((ctx.fileVersion != 136) && (ctx.fileVersion != 133))
		throw Common::Exception("Unknown The Witcher MDB version %d", ctx.fileVersion);

	uint32_t modelCount = ctx.mdb->readUint32LE();
	if (modelCount != 1)
		throw Common::Exception("Unsupported model count %d in The Witcher MDB", modelCount);

	ctx.mdb->skip(4);

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
	ctx.mdlName = _name;
	printf("++++++++++++++++\n");
	printf("loading model %s\n", _name.c_str());

	uint32_t offsetRootNode = ctx.mdb->readUint32LE();

	ctx.mdb->skip(32);

	byte type = ctx.mdb->readByte();
	printf("type: %u\n", type);

	ctx.mdb->skip(3);

	ctx.mdb->skip(48);

	float firstLOD = ctx.mdb->readIEEEFloatLE();
	float lastLOD  = ctx.mdb->readIEEEFloatLE();
	printf("first lod: %f\n", firstLOD);
	printf("last lod: %f\n", lastLOD);
	ctx.mdb->skip(16);

	Common::UString detailMap = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);
	printf("detail map: %s\n", detailMap.c_str());
	ctx.mdb->skip(4);

	float modelScale = ctx.mdb->readIEEEFloatLE();
	printf("scale: %f\n", modelScale);

	Common::UString superModel = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);
	printf("supermodel: %s\n", superModel.c_str());

	ctx.mdb->skip(16);

	newState(ctx);

	indent += 4;
	ModelNode_Witcher *rootNode = new ModelNode_Witcher(*this);
	ctx.nodes.push_back(rootNode);

	ctx.mdb->seek(ctx.offModelData + offsetRootNode);
	rootNode->load(ctx);
	indent -= 4;

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

	void ModelNode_Witcher::load(Model_Witcher::ParserContext &ctx, uint32_t id) {
	ctx.mdb->skip(24); // Function pointers

	uint32_t inheritColor = ctx.mdb->readUint32LE();
	uint32_t nodeNumber   = ctx.mdb->readUint32LE();

	_name = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);
	printf("%*s loading node %s\n", indent, "", _name.c_str());
	printf("%*s inherit colour: %u\n", indent, "", inheritColor);
	printf("%*s node number: %u\n", indent, "", nodeNumber);

	ctx.mdb->skip(8); // Parent pointers

	uint32_t childrenOffset, childrenCount;
	Model::readArrayDef(*ctx.mdb, childrenOffset, childrenCount);

	std::vector<uint32_t> children;
	Model::readArray(*ctx.mdb, ctx.offModelData + childrenOffset, childrenCount, children);

	uint32_t controllerKeyOffset, controllerKeyCount;
	Model::readArrayDef(*ctx.mdb, controllerKeyOffset, controllerKeyCount);

	uint32_t controllerDataOffset, controllerDataCount;
	Model::readArrayDef(*ctx.mdb, controllerDataOffset, controllerDataCount);

	std::vector<float> controllerData;
	Model::readArray(*ctx.mdb, ctx.offModelData + controllerDataOffset,
	                 controllerDataCount, controllerData);

	readNodeControllers(ctx, ctx.offModelData + controllerKeyOffset,
	                    controllerKeyCount, controllerData);

	ctx.mdb->skip(4); // Unknown

	uint32_t imposterGroup = ctx.mdb->readUint32LE();
	uint32_t fixedRot      = ctx.mdb->readUint32LE();
	printf("%*s imposter group: %u\n", indent, "", imposterGroup);
	printf("%*s fixed rot: %u\n", indent, "", fixedRot);

	uint32_t minLOD = ctx.mdb->readUint32LE();
	uint32_t maxLOD = ctx.mdb->readUint32LE();
	printf("%*s min lod: %u\n", indent, "", minLOD);
	printf("%*s max lod: %u\n", indent, "", maxLOD);

	NodeType type = (NodeType) ctx.mdb->readUint32LE();
	printf("%*s node type: 0x%08X\n", indent, "", type);
	switch (type) {
		case kNodeTypeTrimesh:
			printf("%*s has mesh\n", indent, "");
			indent += 4;
			readMesh(ctx);
			indent -= 4;
			break;

		case kNodeTypeTexturePaint:
			printf("%*s has texture paint\n", indent, "");
			readTexturePaint(ctx);
			break;

		default:
			break;
	}

	if (_mesh && _mesh->data && _mesh->data->rawMesh) {
		Common::UString meshName = std::to_string(id);
		meshName += ".";
		meshName += _name;
		ModelNode *hnode = this;
		ModelNode *parent = hnode->getParent();
		while (parent && (parent != hnode)) {
			meshName += ".";
			meshName += parent->getName();
			hnode = parent;
			parent = hnode->getParent();
		}
		meshName += ".";
		if (ctx.state->name.size() != 0) {
			meshName += ctx.state->name;
		} else {
			meshName += "xoreos.default";
		}
		meshName += ".";
		meshName += ctx.mdlName;

		Graphics::Mesh::Mesh *checkMesh = MeshMan.getMesh(meshName);
		if (checkMesh) {
			delete _mesh->data->rawMesh;
			_mesh->data->rawMesh = checkMesh;
		} else {
			_mesh->data->rawMesh->setName(meshName);
			_mesh->data->rawMesh->init();
			MeshMan.addMesh(_mesh->data->rawMesh);
		}
	}

	// Only render the highest LOD (0), or if the node is not LODing (-1)
	if ((minLOD != -1) && (maxLOD != -1) && (minLOD > 0))
		_render = false;
	printf("%*s ________\n", indent, "");

	indent += 4;
	int i = 0;
	for (std::vector<uint32_t>::const_iterator child = children.begin(); child != children.end(); ++child) {
		printf("%*s loading child %d with id %u\n", indent, "", i, children[i]);
		i++;
		ModelNode_Witcher *childNode = new ModelNode_Witcher(*_model);
		ctx.nodes.push_back(childNode);

		childNode->setParent(this);

		ctx.mdb->seek(ctx.offModelData + *child);
		childNode->load(ctx, children[i]);
	}
	indent -= 4;
}

void ModelNode_Witcher::readMesh(Model_Witcher::ParserContext &ctx) {
	ctx.mdb->skip(4); // Function pointer
	ctx.mdb->skip(4); // Unknown

	uint32_t offMeshArrays = ctx.mdb->readUint32LE();

	ctx.mdb->skip(4); // Unknown

	float boundingMin[3], boundingMax[3];

	boundingMin[0] = ctx.mdb->readIEEEFloatLE();
	boundingMin[1] = ctx.mdb->readIEEEFloatLE();
	boundingMin[2] = ctx.mdb->readIEEEFloatLE();

	boundingMax[0] = ctx.mdb->readIEEEFloatLE();
	boundingMax[1] = ctx.mdb->readIEEEFloatLE();
	boundingMax[2] = ctx.mdb->readIEEEFloatLE();
	printf("%*s bounding min: %f, %f, %f\n", indent, "", boundingMin[0], boundingMin[1], boundingMin[2]);
	printf("%*s bounding max: %f, %f, %f\n", indent, "", boundingMax[0], boundingMax[1], boundingMax[2]);

	ctx.mdb->skip(28); // Unknown

	float volFogScale = ctx.mdb->readIEEEFloatLE();
	printf("%*s volumetric fog scale: %f\n", indent, "", volFogScale);

	ctx.mdb->skip(16); // Unknown

	_mesh = new Mesh();

	_mesh->diffuse[0] = ctx.mdb->readIEEEFloatLE();
	_mesh->diffuse[1] = ctx.mdb->readIEEEFloatLE();
	_mesh->diffuse[2] = ctx.mdb->readIEEEFloatLE();
	_mesh->ambient[0] = ctx.mdb->readIEEEFloatLE();
	_mesh->ambient[1] = ctx.mdb->readIEEEFloatLE();
	_mesh->ambient[2] = ctx.mdb->readIEEEFloatLE();
	printf("%*s diffuse: %f, %f, %f\n", indent, "", _mesh->diffuse[0], _mesh->diffuse[1], _mesh->diffuse[2]);
	printf("%*s ambient: %f, %f, %f\n", indent, "", _mesh->ambient[0], _mesh->ambient[1], _mesh->ambient[2]);

	float textureTransRot[3];
	textureTransRot[0] = ctx.mdb->readIEEEFloatLE();
	textureTransRot[1] = ctx.mdb->readIEEEFloatLE();
	textureTransRot[2] = ctx.mdb->readIEEEFloatLE();
	printf("%*s texture trans rot: %f, %f, %f\n", indent, "", textureTransRot[0], textureTransRot[1], textureTransRot[2]);

	_mesh->shininess = ctx.mdb->readIEEEFloatLE();
	printf("%*s shininess: %f\n", indent, "", _mesh->shininess);

	uint32_t shadow = ctx.mdb->readUint32LE();
	uint32_t beaming = ctx.mdb->readUint32LE();
	uint32_t render = ctx.mdb->readUint32LE();
	_mesh->shadow  = shadow == 1;
	_mesh->beaming = beaming == 1;
	_mesh->render  = render == 1;
	printf("%*s shadow: 0x%04X\n", indent, "", shadow);
	printf("%*s beaming: 0x%04X\n", indent, "", beaming);
	printf("%*s render: 0x%04X\n", indent, "", render);

	_mesh->transparencyHint = ctx.mdb->readUint32LE();
	printf("%*s transparency hint: %u\n", indent, "", _mesh->transparencyHint);

	ctx.mdb->skip(4); // Unknown

	Common::UString texture[4];
	for (int t = 0; t < 4; t++) {
		texture[t] = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);
		printf("%*s texture %d: %s\n", indent, "", t, texture[t].c_str());

		if (texture[t] == "NULL")
			texture[t].clear();
	}

	_mesh->tilefade = ctx.mdb->readUint32LE() == 1;
	printf("%*s tilefade: %u\n", indent, "", _mesh->tilefade);

	bool controlFade   = ctx.mdb->readByte() == 1;
	bool lightMapped   = ctx.mdb->readByte() == 1;
	bool rotateTexture = ctx.mdb->readByte() == 1;
	printf("%*s control fade: %u\n", indent, "", controlFade);
	printf("%*s light mapped: %u\n", indent, "", lightMapped);
	printf("%*s rotate texture: %u\n", indent, "", rotateTexture);

	ctx.mdb->skip(1); // Unknown

	float transparencyShift = ctx.mdb->readIEEEFloatLE();
	printf("%*s transparency shift: %f\n", indent, "", transparencyShift);

	uint32_t defaultRenderList = ctx.mdb->readUint32LE();
	uint32_t preserveVColors   = ctx.mdb->readUint32LE();
	printf("%*s default render list: %u\n", indent, "", defaultRenderList);
	printf("%*s preseserve vertex colours: %u\n", indent, "", preserveVColors);

	uint32_t fourCC = ctx.mdb->readUint32BE();
	printf("%*s fourCC: %u\n", indent, "", fourCC);

	ctx.mdb->skip(4); // Unknown

	float depthOffset       = ctx.mdb->readIEEEFloatLE();
	float coronaCenterMult  = ctx.mdb->readIEEEFloatLE();
	float fadeStartDistance = ctx.mdb->readIEEEFloatLE();
	printf("%*s depth offset: %f\n", indent, "", depthOffset);
	printf("%*s corona centre multiplier: %f\n", indent, "", coronaCenterMult);
	printf("%*s fade start distance: %f\n", indent, "", fadeStartDistance);

	bool distFromScreenCenterFace = ctx.mdb->readByte() == 1;
	ctx.mdb->skip(3); // Unknown
	printf("%*s dist from screen centre face: %u\n", indent, "", distFromScreenCenterFace);


	float enlargeStartDistance = ctx.mdb->readIEEEFloatLE();
	printf("%*s enlarge start distance: %f\n", indent, "", enlargeStartDistance);

	bool affectedByWind = ctx.mdb->readByte() == 1;
	ctx.mdb->skip(3); // Unknown
	printf("%*s affected by wind: %u\n", indent, "", affectedByWind);

	float dampFactor = ctx.mdb->readIEEEFloatLE();
	printf("%*s dampening factor: %f\n", indent, "", dampFactor);

	uint32_t blendGroup = ctx.mdb->readUint32LE();
	printf("%*s blend group: %u\n", indent, "", blendGroup);

	bool dayNightLightMaps = ctx.mdb->readByte() == 1;
	printf("%*s day/night light maps: %u\n", indent, "", dayNightLightMaps);

	Common::UString dayNightTransition = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 200);
	printf("%*s day/nnight transition: %s\n", indent, "", dayNightTransition.c_str());

	bool ignoreHitCheck  = ctx.mdb->readByte() == 1;
	bool needsReflection = ctx.mdb->readByte() == 1;
	ctx.mdb->skip(1); // Unknown
	printf("%*s ignore hit check: %u\n", indent, "", ignoreHitCheck);
	printf("%*s needs reflection: %u\n", indent, "", needsReflection);

	float reflectionPlaneNormal[3];
	reflectionPlaneNormal[0] = ctx.mdb->readIEEEFloatLE();
	reflectionPlaneNormal[1] = ctx.mdb->readIEEEFloatLE();
	reflectionPlaneNormal[2] = ctx.mdb->readIEEEFloatLE();
	printf("%*s reflection plane normal: %f, %f, %f\n", indent, "", reflectionPlaneNormal[0], reflectionPlaneNormal[1], reflectionPlaneNormal[2]);
	float reflectionPlaneDistance = ctx.mdb->readIEEEFloatLE();
	printf("%*s reflection plane distance: %f\n", indent, "", reflectionPlaneDistance);

	bool fadeOnCameraCollision = ctx.mdb->readByte() == 1;
	bool noSelfShadow          = ctx.mdb->readByte() == 1;
	bool isReflected           = ctx.mdb->readByte() == 1;
	bool onlyReflected         = ctx.mdb->readByte() == 1;
	printf("%*s fade on camera collision: %u\n", indent, "", fadeOnCameraCollision);
	printf("%*s no self shadow: %u\n", indent, "", noSelfShadow);
	printf("%*s is reflected: %u\n", indent, "", isReflected);
	printf("%*s only reflected: %u\n", indent, "", onlyReflected);

	Common::UString lightMapName = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);
	printf("%*s lightmap name: %s\n", indent, "", lightMapName.c_str());

	bool canDecal            = ctx.mdb->readByte() == 1;
	bool multiBillBoard      = ctx.mdb->readByte() == 1;
	bool ignoreLODReflection = ctx.mdb->readByte() == 1;
	ctx.mdb->skip(1); // Unknown
	printf("%*s can decal: %u\n", indent, "", canDecal);
	printf("%*s multi-billboard: %u\n", indent, "", multiBillBoard);
	printf("%*s ignore lod reflection: %u\n", indent, "", ignoreLODReflection);

	float detailMapScape = ctx.mdb->readIEEEFloatLE();
	printf("%*s detail map scape: %f\n", indent, "", detailMapScape);

	ctx.offTextureInfo = ctx.mdb->readUint32LE();

	uint32_t endPos = ctx.mdb->seek(ctx.offRawData + offMeshArrays);

	ctx.mdb->skip(4);

	uint32_t vertexOffset, vertexCount;
	Model::readArrayDef(*ctx.mdb, vertexOffset, vertexCount);
	printf("%*s vertex count: %u\n", indent, "", vertexCount);

	uint32_t normalsOffset, normalsCount;
	Model::readArrayDef(*ctx.mdb, normalsOffset, normalsCount);

	uint32_t tangentsOffset, tangentsCount;
	Model::readArrayDef(*ctx.mdb, tangentsOffset, tangentsCount);

	uint32_t biNormalsOffset, biNormalsCount;
	Model::readArrayDef(*ctx.mdb, biNormalsOffset, biNormalsCount);

	uint32_t tVertsOffset[4], tVertsCount[4];
	for (uint t = 0; t < 4; t++)
		Model::readArrayDef(*ctx.mdb, tVertsOffset[t], tVertsCount[t]);

	uint32_t unknownOffset, unknownCount;
	Model::readArrayDef(*ctx.mdb, unknownOffset, unknownCount);

	uint32_t facesOffset, facesCount;
	Model::readArrayDef(*ctx.mdb, facesOffset, facesCount);

	if (ctx.fileVersion == 133)
		ctx.offTexData = ctx.mdb->readUint32LE();


	if ((vertexCount == 0) || (facesCount == 0)) {
		ctx.mdb->seek(endPos);
		return;
	}

	_render = _mesh->render;
	_mesh->data = new MeshData();
	_mesh->data->rawMesh = new Graphics::Mesh::Mesh();

	std::vector<Common::UString> textures;
	readTextures(ctx, textures);
	for (int t = 0; t < textures.size(); t++) {
		printf("%*s read texture %d: %s\n", indent, "", t, textures[t].c_str());
	}
	evaluateTextures(4, textures, texture, tVertsCount, dayNightLightMaps, lightMapName);

	loadTextures(textures);

	size_t texCount = textures.size();

	// Read vertices

	VertexDecl vertexDecl;

	vertexDecl.push_back(VertexAttrib(VPOSITION, 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VNORMAL  , 3, GL_FLOAT));

	///< @TODO: This should probably use tVertsCount for a particular VTCOORD
	for (uint t = 0; t < texCount; t++)
		vertexDecl.push_back(VertexAttrib(VTCOORD + t, 2, GL_FLOAT));

	_mesh->data->rawMesh->getVertexBuffer()->setVertexDeclLinear(vertexCount, vertexDecl);

	// Read vertex position
	ctx.mdb->seek(ctx.offRawData + vertexOffset);
	float *v = reinterpret_cast<float *>(_mesh->data->rawMesh->getVertexBuffer()->getData(0));
	for (uint32_t i = 0; i < vertexCount; i++) {
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
	}

	// Read vertex normals
	assert(normalsCount == vertexCount);
	ctx.mdb->seek(ctx.offRawData + normalsOffset);
	v = reinterpret_cast<float *>(_mesh->data->rawMesh->getVertexBuffer()->getData(1));
	for (uint32_t i = 0; i < normalsCount; i++) {
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
	}

	// Read texture coordinates
	for (uint t = 0; t < texCount; t++) {

		ctx.mdb->seek(ctx.offRawData + tVertsOffset[t]);
		v = reinterpret_cast<float *>(_mesh->data->rawMesh->getVertexBuffer()->getData(2 + t));
		for (uint32_t i = 0; i < tVertsCount[t]; i++) {
			if (i < tVertsCount[t]) {
				*v++ = ctx.mdb->readIEEEFloatLE();
				*v++ = ctx.mdb->readIEEEFloatLE();
			} else {
				*v++ = 0.0f;
				*v++ = 0.0f;
			}
		}
	}


	// Read faces

	_mesh->data->rawMesh->getIndexBuffer()->setSize(facesCount * 3, sizeof(uint32_t), GL_UNSIGNED_INT);

	ctx.mdb->seek(ctx.offRawData + facesOffset);
	uint32_t *f = reinterpret_cast<uint32_t *>(_mesh->data->rawMesh->getIndexBuffer()->getData());
	for (uint32_t i = 0; i < facesCount; i++) {
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

	ctx.mdb->seek(endPos);
}

void ModelNode_Witcher::readTexturePaint(Model_Witcher::ParserContext &ctx) {
	uint32_t layersOffset, layersCount;
	Model::readArrayDef(*ctx.mdb, layersOffset, layersCount);
	printf("%*s reading texture paint\n", indent, "");
	printf("%*s layer count: %u\n", indent, "", layersCount);

	ctx.mdb->skip(28); // Unknown

	uint32_t offMeshArrays = ctx.mdb->readUint32LE();

	uint32_t sectorID0 = ctx.mdb->readUint32LE();
	uint32_t sectorID1 = ctx.mdb->readUint32LE();
	uint32_t sectorID2 = ctx.mdb->readUint32LE();
	uint32_t sectorID3 = ctx.mdb->readUint32LE();
	printf("%*s sector ID0: %u\n", indent, "", sectorID0);
	printf("%*s sector ID1: %u\n", indent, "", sectorID1);
	printf("%*s sector ID2: %u\n", indent, "", sectorID2);
	printf("%*s sector ID3: %u\n", indent, "", sectorID3);

	float boundingMin[3], boundingMax[3];

	boundingMin[0] = ctx.mdb->readIEEEFloatLE();
	boundingMin[1] = ctx.mdb->readIEEEFloatLE();
	boundingMin[2] = ctx.mdb->readIEEEFloatLE();

	boundingMax[0] = ctx.mdb->readIEEEFloatLE();
	boundingMax[1] = ctx.mdb->readIEEEFloatLE();
	boundingMax[2] = ctx.mdb->readIEEEFloatLE();
	printf("%*s bounding min: %f, %f, %f\n", indent, "", boundingMin[0], boundingMin[1], boundingMin[2]);
	printf("%*s bounding max: %f, %f, %f\n", indent, "", boundingMax[0], boundingMax[1], boundingMax[2]);

	_mesh = new Mesh();

	_mesh->diffuse[0] = ctx.mdb->readIEEEFloatLE();
	_mesh->diffuse[1] = ctx.mdb->readIEEEFloatLE();
	_mesh->diffuse[2] = ctx.mdb->readIEEEFloatLE();
	_mesh->ambient[0] = ctx.mdb->readIEEEFloatLE();
	_mesh->ambient[1] = ctx.mdb->readIEEEFloatLE();
	_mesh->ambient[2] = ctx.mdb->readIEEEFloatLE();
	printf("%*s diffuse: %f, %f, %f\n", indent, "", _mesh->diffuse[0], _mesh->diffuse[1], _mesh->diffuse[2]);
	printf("%*s ambient: %f, %f, %f\n", indent, "", _mesh->ambient[0], _mesh->ambient[1], _mesh->ambient[2]);

	float textureTransRot[3];
	textureTransRot[0] = ctx.mdb->readIEEEFloatLE();
	textureTransRot[1] = ctx.mdb->readIEEEFloatLE();
	textureTransRot[2] = ctx.mdb->readIEEEFloatLE();
	printf("%*s texture trans rot: %f, %f, %f\n", indent, "", textureTransRot[0], textureTransRot[1], textureTransRot[2]);

	_mesh->shadow = ctx.mdb->readUint32LE() == 1;
	_mesh->render = ctx.mdb->readUint32LE() == 1;
	printf("%*s shadow: %u\n", indent, "", _mesh->shadow);
	printf("%*s render: %u\n", indent, "", _mesh->render);

	bool tileFade = ctx.mdb->readUint32LE() == 1;
	printf("%*s tile fade: %u\n", indent, "", tileFade);

	bool controlFade   = ctx.mdb->readByte() == 1;
	bool lightMapped   = ctx.mdb->readByte() == 1;
	bool rotateTexture = ctx.mdb->readByte() == 1;
	ctx.mdb->skip(1); // Unknown
	printf("%*s control fade: %u\n", indent, "", controlFade);
	printf("%*s light mapped: %u\n", indent, "", lightMapped);
	printf("%*s rotate texture: %u\n", indent, "", rotateTexture);

	float transparencyShift = ctx.mdb->readIEEEFloatLE();
	printf("%*s transparency shift: %f\n", indent, "", transparencyShift);

	uint32_t defaultRenderList = ctx.mdb->readUint32LE();
	uint32_t fourCC            = ctx.mdb->readUint32BE();
	printf("%*s default render list: %u\n", indent, "", defaultRenderList);
	printf("%*s fourCC: %u\n", indent, "", fourCC);

	ctx.mdb->skip(4); // Unknown

	float depthOffset = ctx.mdb->readIEEEFloatLE();
	printf("%*s depth offset: %f\n", indent, "", depthOffset);

	uint32_t blendGroup = ctx.mdb->readUint32LE();
	printf("%*s blend group: %u\n", indent, "", blendGroup);

	bool dayNightLightMaps = ctx.mdb->readByte() == 1;
	printf("%*s day/night light maps: %u\n", indent, "", dayNightLightMaps);

	Common::UString dayNightTransition = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 200);
	printf("%*s day/night transition: %s\n", indent, "", dayNightTransition.c_str());

	bool ignoreHitCheck  = ctx.mdb->readByte() == 1;
	bool needsReflection = ctx.mdb->readByte() == 1;
	ctx.mdb->skip(1); // Unknown
	printf("%*s ignore hit check: %u\n", indent, "", ignoreHitCheck);
	printf("%*s needs reflection: %u\n", indent, "", needsReflection);

	float reflectionPlaneNormal[3];
	reflectionPlaneNormal[0] = ctx.mdb->readIEEEFloatLE();
	reflectionPlaneNormal[1] = ctx.mdb->readIEEEFloatLE();
	reflectionPlaneNormal[2] = ctx.mdb->readIEEEFloatLE();
	printf("%*s reflection plane normal: %f, %f, %f\n", indent, "", reflectionPlaneNormal[0], reflectionPlaneNormal[1], reflectionPlaneNormal[2]);

	float reflectionPlaneDistance = ctx.mdb->readIEEEFloatLE();
	printf("%*s reflection plane distance: %f\n", indent, "", reflectionPlaneDistance);

	bool fadeOnCameraCollision = ctx.mdb->readByte() == 1;
	bool noSelfShadow          = ctx.mdb->readByte() == 1;
	bool isReflected           = ctx.mdb->readByte() == 1;
	ctx.mdb->skip(1); // Unknown
	printf("%*s fade on camera collision: %u\n", indent, "", fadeOnCameraCollision);
	printf("%*s no self shadow: %u\n", indent, "", noSelfShadow);
	printf("%*s is reflected: %u\n", indent, "", isReflected);

	float detailMapScape = ctx.mdb->readIEEEFloatLE();
	printf("%*s detail map scape: %f\n", indent, "", detailMapScape);

	bool onlyReflected = ctx.mdb->readByte() == 1;
	printf("%*s only reflected: %u\n", indent, "", onlyReflected);

	Common::UString lightMapName = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 64);
	printf("%*s lightmap name: %s\n", indent, "", lightMapName.c_str());

	bool canDecal            = ctx.mdb->readByte() == 1;
	bool ignoreLODReflection = ctx.mdb->readByte() == 1;
	bool enableSpecular      = ctx.mdb->readByte() == 1;
	printf("%*s can decal: %u\n", indent, "", canDecal);
	printf("%*s ignore lod reflection: %u\n", indent, "", ignoreLODReflection);
	printf("%*s enable specular: %u\n", indent, "", enableSpecular);


	uint32_t endPos = ctx.mdb->seek(ctx.offRawData + offMeshArrays);

	ctx.mdb->skip(4);

	uint32_t vertexOffset, vertexCount;
	Model::readArrayDef(*ctx.mdb, vertexOffset, vertexCount);
	printf("%*s vertex count: %u\n", indent, "", vertexCount);

	uint32_t normalsOffset, normalsCount;
	Model::readArrayDef(*ctx.mdb, normalsOffset, normalsCount);

	uint32_t tangentsOffset, tangentsCount;
	Model::readArrayDef(*ctx.mdb, tangentsOffset, tangentsCount);

	uint32_t biNormalsOffset, biNormalsCount;
	Model::readArrayDef(*ctx.mdb, biNormalsOffset, biNormalsCount);

	uint32_t tVertsOffset[4], tVertsCount[4];
	for (uint t = 0; t < 4; t++)
		Model::readArrayDef(*ctx.mdb, tVertsOffset[t], tVertsCount[t]);

	uint32_t unknownOffset, unknownCount;
	Model::readArrayDef(*ctx.mdb, unknownOffset, unknownCount);

	uint32_t facesOffset, facesCount;
	Model::readArrayDef(*ctx.mdb, facesOffset, facesCount);

	if ((vertexCount == 0) || (facesCount == 0)) {
		ctx.mdb->seek(endPos);
		return;
	}

	_render = _mesh->render;
	_mesh->data = new MeshData();
	_mesh->data->rawMesh = new Graphics::Mesh::Mesh();

	std::vector<TexturePaintLayer> layers;
	layers.resize(layersCount);

	for (uint32_t l = 0; l < layersCount; l++) {
		ctx.mdb->seek(ctx.offRawData + layersOffset + l * 52);

		layers[l].hasTexture = ctx.mdb->readByte() == 1;
		if (!layers[l].hasTexture)
			continue;

		ctx.mdb->skip(3); // Unknown
		ctx.mdb->skip(4); // Offset to material

		layers[l].texture = Common::readStringFixed(*ctx.mdb, Common::kEncodingASCII, 32);

		uint32_t weightsOffset, weightsCount;
		Model::readArrayDef(*ctx.mdb, weightsOffset, weightsCount);

		ctx.mdb->seek(ctx.offRawData + weightsOffset);
		layers[l].weights.resize(weightsCount);

		for (std::vector<float>::iterator w = layers[l].weights.begin(); w != layers[l].weights.end(); ++w)
			*w = ctx.mdb->readIEEEFloatLE();
	}

	std::vector<Common::UString> textures;
	textures.push_back(lightMapName);

	evaluateTextures(1, textures, 0, tVertsCount, dayNightLightMaps, lightMapName);

	loadTextures(textures);

	size_t texCount = textures.size();

	// Read vertices

	VertexDecl vertexDecl;

	vertexDecl.push_back(VertexAttrib(VPOSITION, 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VNORMAL  , 3, GL_FLOAT));

	///< @TODO: This should probably use tVertsCount to know if a particular VTCOORD exists.
	for (uint t = 0; t < texCount; t++)
		vertexDecl.push_back(VertexAttrib(VTCOORD + t, 2, GL_FLOAT));

	_mesh->data->rawMesh->getVertexBuffer()->setVertexDeclLinear(vertexCount, vertexDecl);

	// Read vertex position
	ctx.mdb->seek(ctx.offRawData + vertexOffset);
	float *v = reinterpret_cast<float *>(_mesh->data->rawMesh->getVertexBuffer()->getData(0));
	for (uint32_t i = 0; i < vertexCount; i++) {
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
	}

	// Read vertex normals
	assert(normalsCount == vertexCount);
	ctx.mdb->seek(ctx.offRawData + normalsOffset);
	v = reinterpret_cast<float *>(_mesh->data->rawMesh->getVertexBuffer()->getData(1));
	for (uint32_t i = 0; i < normalsCount; i++) {
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
		*v++ = ctx.mdb->readIEEEFloatLE();
	}

	// Read texture coordinates
	for (uint t = 0; t < texCount; t++) {

		ctx.mdb->seek(ctx.offRawData + tVertsOffset[t]);
		v = reinterpret_cast<float *>(_mesh->data->rawMesh->getVertexBuffer()->getData(2 + t));
		for (uint32_t i = 0; i < tVertsCount[t]; i++) {
			if (i < tVertsCount[t]) {
				*v++ = ctx.mdb->readIEEEFloatLE();
				*v++ = ctx.mdb->readIEEEFloatLE();
			} else {
				*v++ = 0.0f;
				*v++ = 0.0f;
			}
		}
	}


	// Read faces

	_mesh->data->rawMesh->getIndexBuffer()->setSize(facesCount * 3, sizeof(uint32_t), GL_UNSIGNED_INT);

	ctx.mdb->seek(ctx.offRawData + facesOffset);
	uint32_t *f = reinterpret_cast<uint32_t *>(_mesh->data->rawMesh->getIndexBuffer()->getData());
	for (uint32_t i = 0; i < facesCount; i++) {
		// Vertex indices
		*f++ = ctx.mdb->readUint32LE();
		*f++ = ctx.mdb->readUint32LE();
		*f++ = ctx.mdb->readUint32LE();

		ctx.mdb->skip(68); // Unknown
	}

	_mesh->data->rawMesh->init();

	createBound();

	ctx.mdb->seek(endPos);

	if (GfxMan.isRendererExperimental())
		buildMaterial();
}

void ModelNode_Witcher::readTextures(Model_Witcher::ParserContext &ctx,
                                     std::vector<Common::UString> &textures) {

	uint32_t offset;
	if (ctx.fileVersion == 133)
		offset = ctx.offRawData + ctx.offTexData;
	else
		offset = ctx.offTexData + ctx.offTextureInfo;

	ctx.mdb->seek(offset);

	uint32_t textureCount = ctx.mdb->readUint32LE();
	uint32_t offTexture   = ctx.mdb->readUint32LE();

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
			printf("%*s Texture line indicates shader: %s\n", indent, "", shader.c_str());
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
		const Common::UString *staticTextures, const uint32_t *tVertsCount,
		bool lightMapDayNight, const Common::UString &lightMapName) {

	textures.resize(n);

	for (int t = 0; t < n; t++) {
		if (textures[t].empty() && staticTextures)
			textures[t] = staticTextures[t];

		if (tVertsCount[t] == 0)
			textures[t].clear();

		if (textures[t].empty())
			continue;

		if (lightMapDayNight && (textures[t] == lightMapName)) {
			// Day (dzie&#324;)
			if      (ResMan.hasResource(textures[t] + "!d", ::Aurora::kResourceImage))
				textures[t] += "!d";
			// Morning (rano)
			else if (ResMan.hasResource(textures[t] + "!r", ::Aurora::kResourceImage))
				textures[t] += "!r";
			// Noon (po&#322;udnie)
			else if (ResMan.hasResource(textures[t] + "!p", ::Aurora::kResourceImage))
				textures[t] += "!p";
			// Evening (wiecz&#211;r)
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
		uint32_t offset, uint32_t count, std::vector<float> &data) {

	uint32_t pos = ctx.mdb->seek(offset);

	// TODO: readNodeControllers: Implement this properly :P

	for (uint32_t i = 0; i < count; i++) {
		uint32_t type        = ctx.mdb->readUint32LE();
		uint16_t rowCount    = ctx.mdb->readUint16LE();
		uint16_t timeIndex   = ctx.mdb->readUint16LE();
		uint16_t dataIndex   = ctx.mdb->readUint16LE();
		uint8_t  columnCount = ctx.mdb->readByte();
		ctx.mdb->skip(1);
		printf("%*s Found controller type: 0x%04X\n", indent, "", type);

		if (rowCount == 0xFFFF)
			// TODO: Controller row count = 0xFFFF
			continue;

		printf("%*s columns: %u, ", indent, "", columnCount);
		for (uint32_t c = 0; c < columnCount; ++c) {
			printf("%f ", data[dataIndex + c]);
		}
		printf("\n");
		if        (type == kNodeControllerTypePosition) {
			if (columnCount != 3)
				throw Common::Exception("Position controller with %d values", columnCount);

			_position[0] = data[dataIndex + 0];
			_position[1] = data[dataIndex + 1];
			_position[2] = data[dataIndex + 2];

		} else if (type == kNodeControllerTypeOrientation) {
			if (columnCount != 4)
				throw Common::Exception("Orientation controller with %d values", columnCount);

			_orientation[0] = data[dataIndex + 0];
			_orientation[1] = data[dataIndex + 1];
			_orientation[2] = data[dataIndex + 2];
			_orientation[3] = Common::rad2deg(acos(data[dataIndex + 3]) * 2.0);
		}

	}

	ctx.mdb->seek(pos);
}

void ModelNode_Witcher::buildMaterial() {
	/**
	 * Most of this function has been adapted from ModelNode::buildMaterial, so see there
	 * for more explanations of why everything is the way it is.
	 */

	if (_light) {
		///< @TODO: absolute transforms aren't properly calculated by now apparently.
		//glm::mat4 tform = _model->getAbsoluteTransform() * this->getAbsoluteTransform();
		_light->position = glm::vec3(_renderTransform[3]);
	}

	for (auto &renderable: _renderableArray) {
		delete renderable.getMaterial();
		delete renderable.getSurface();
	}
	_renderableArray.clear();

	_dirtyRender = false;

	ModelNode::Mesh *pmesh  = 0;  // TODO: if anything is changed in here, ensure there's a local copy instead that shares the root data.
	TextureHandle *phandles = 0;  // Take from self first, or root state, if there is one, otherwise.
	TextureHandle *penvmap  = 0;  // Maybe it's only the environment map that's overriden.
	EnvironmentMapMode envmapmode;

	uint32_t textureCount = 0;

	pmesh = getMesh();
	phandles = getTextures(textureCount);
	penvmap = getEnvironmentMap(envmapmode);

	if (textureCount == 0) {
		return;
	}

	if (!_model->getState().empty() && !pmesh) {
		_rootStateNode = _model->getNode("", _name);
		if (_rootStateNode == this) {
			_rootStateNode = 0;
		}
	} else {
		_rootStateNode = 0;
	}

	if (!pmesh) {
		return;
	}

	if (!pmesh->data) {
		return;
	}

	if (textureCount == 0 && pmesh->data->envMap.empty() && !pmesh->data->rawMesh) {
		return;
	}
	/**
	 * To get here, _mesh must exist and have some data. This is required to consider making
	 * a new renderable - otherwise, the renderable of the parent can be used directly. This
	 * may change depending what information the renderable is dependent on during creation.
	 * Important information in this case means texture or environment maps are overidden from
	 * any potential parent.
	 */

	if (!pmesh->data->rawMesh) {
		return;
	}

	Common::UString vertexShaderName;
	Common::UString fragmentShaderName;
	Common::UString materialName = "xoreos.";
	Graphics::Shader::ShaderDescriptor cripter;

	Shader::ShaderMaterial *material;
	Shader::ShaderSurface *surface;

	uint32_t materialFlags = 0;

	_renderableArray.clear();

	const VertexDecl &decl = pmesh->data->rawMesh->getVertexBuffer()->getVertexDecl();
	for (uint32_t i = 0; i < decl.size(); ++i) {
		if (decl[i].index == VertexAttribIdEnum::VPOSITION) {
			cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_POSITION0);
		} else if (decl[i].index == VertexAttribIdEnum::VNORMAL) {
			cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_NORMAL0);
		} else if (decl[i].index == VertexAttribIdEnum::VTCOORD) {
			cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_UV0);
		} else if (decl[i].index == (VertexAttribIdEnum::VTCOORD + 1)) {
			cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_UV1);
		}
	}

	if (penvmap) {
		if (penvmap->getTexture().getImage().isCubeMap()) {
			cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_UV_CUBE);
			cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_7,
			                       Graphics::Shader::ShaderDescriptor::SAMPLER_CUBE);
			cripter.connect(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_7,
			                Graphics::Shader::ShaderDescriptor::INPUT_UV_CUBE,
			                Graphics::Shader::ShaderDescriptor::ENV_CUBE);
		} else {
			cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_UV_SPHERE);
			cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_7,
			                       Graphics::Shader::ShaderDescriptor::SAMPLER_2D);
			cripter.connect(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_7,
			                Graphics::Shader::ShaderDescriptor::INPUT_UV_SPHERE,
			                Graphics::Shader::ShaderDescriptor::ENV_SPHERE);
		}

		if (envmapmode == kModeEnvironmentBlendedUnder) {
			materialName += penvmap->getName();
			// Figure out if a cube or sphere map is used.
			if (penvmap->getTexture().getImage().isCubeMap()) {
				if (!pmesh->isTransparent) {
					materialFlags |= Shader::ShaderMaterial::MATERIAL_OPAQUE;
				}
				cripter.addPass(Graphics::Shader::ShaderDescriptor::ENV_CUBE,
				                Graphics::Shader::ShaderDescriptor::BLEND_ONE);
			} else {
				/**
				 * Seems that, regardless of _isTransparent, anything with shperical env mapping is opaque. This mostly comes from
				 * NWN, where it's seen that things marked as transparent actually shouldn't be. It's assumed this carries over to
				 * other game titles as well.
				 */
				materialFlags |= Shader::ShaderMaterial::MATERIAL_OPAQUE;
				cripter.addPass(Graphics::Shader::ShaderDescriptor::ENV_SPHERE,
				                Graphics::Shader::ShaderDescriptor::BLEND_ONE);
			}
		}
	}

	if (pmesh->isTransparent && !(materialFlags & Shader::ShaderMaterial::MATERIAL_OPAQUE)) {
		materialFlags |= Shader::ShaderMaterial::MATERIAL_TRANSPARENT;
	}

	/**
	 * Sometimes the _textures handler array isn't matched up against what
	 * is properly loaded (missing files from disk). So do some brief sanity
	 * checks on this.
	 */

	if (textureCount == 1) {
		if (!phandles[0].empty()) {
			materialName += ".";
			materialName += phandles[0].getName();
			cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
			                       Graphics::Shader::ShaderDescriptor::SAMPLER_2D);
			cripter.connect(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
			                Graphics::Shader::ShaderDescriptor::INPUT_UV0,
			                Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE);
			cripter.addPass(Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE,
			                Graphics::Shader::ShaderDescriptor::BLEND_ONE);
		}
	} else if (textureCount > 1) {
		if (!phandles[1].empty()) {
			materialName += ".";
			materialName += phandles[1].getName();
			cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_1,
			                       Graphics::Shader::ShaderDescriptor::SAMPLER_2D);
			cripter.connect(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_1,
			                Graphics::Shader::ShaderDescriptor::INPUT_UV1,
			                Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE);
			cripter.addPass(Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE,
			                Graphics::Shader::ShaderDescriptor::BLEND_ONE);
		}

		if (!phandles[0].empty()) {
			materialName += ".";
			materialName += phandles[0].getName();
			cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
			                       Graphics::Shader::ShaderDescriptor::SAMPLER_2D);
			cripter.connect(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
			                Graphics::Shader::ShaderDescriptor::INPUT_UV0,
			                Graphics::Shader::ShaderDescriptor::TEXTURE_LIGHTMAP);
			cripter.addPass(Graphics::Shader::ShaderDescriptor::TEXTURE_LIGHTMAP,
			                Graphics::Shader::ShaderDescriptor::BLEND_MULTIPLY);
		}
#if 0
		if (!phandles[0].empty()) {
			materialName += phandles[0].getName();
			/**
			 * The Witcher will place diffuse texture as the second if there's more than one.
			 * In these cases, the first texture will be a lightmap. Need to switch things around
			 * so that the first pass is the diffuse colour. The second pass will multiply it.
			 */
			Graphics::Shader::ShaderDescriptor::Sampler sid = Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_1;
			cripter.declareSampler(sid, Graphics::Shader::ShaderDescriptor::SAMPLER_2D);
			cripter.connect(sid,
			                Graphics::Shader::ShaderDescriptor::INPUT_UV1,
			                Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE);

			if (phandles[0].getTexture().getTXI().getFeatures().blending) {
				materialFlags |= Shader::ShaderMaterial::MATERIAL_CUSTOM_BLEND;
			}
			// Check to see if it's actually a decal texture.
			if (phandles[0].getTexture().getTXI().getFeatures().decal) {
				materialFlags |= Shader::ShaderMaterial::MATERIAL_DECAL;
			}
			if (penvmap && envmapmode == kModeEnvironmentBlendedUnder) {
				cripter.addPass(Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE,
				                Graphics::Shader::ShaderDescriptor::BLEND_SRC_ALPHA);
			} else {
				cripter.addPass(Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE,
				                Graphics::Shader::ShaderDescriptor::BLEND_ONE);
			}
		} else {
			/**
			 * Actually this looks like a case where someone has declared multiple textures,
			 * but only filled in one. So the second, which should be a lightmap, is actually
			 * the diffuse. Just to screw with everyone's head a bit.
			 * Just set the colour, which will default to white - a diffuse texture treated
			 * as a lightmap, multiplying white, will end up with the same RGB values.
			 */
			cripter.declareUniform(Graphics::Shader::ShaderDescriptor::UNIFORM_F_COLOUR);
			cripter.addPass(Graphics::Shader::ShaderDescriptor::COLOUR,
			                Graphics::Shader::ShaderDescriptor::BLEND_ONE);
		}
#endif
	}
	/*
	if (textureCount >= 2) {
		if (!phandles[1].empty()) {
			materialName += ".";
			materialName += phandles[1].getName();
			cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
			                       Graphics::Shader::ShaderDescriptor::SAMPLER_2D);
			cripter.connect(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
			                Graphics::Shader::ShaderDescriptor::INPUT_UV1,
			                Graphics::Shader::ShaderDescriptor::TEXTURE_LIGHTMAP);
			cripter.addPass(Graphics::Shader::ShaderDescriptor::TEXTURE_LIGHTMAP,
			                Graphics::Shader::ShaderDescriptor::BLEND_MULTIPLY);
		}
	}
	*/

	if (textureCount >= 3) {
		if (!phandles[2].empty()) {
			materialName += ".";
			materialName += phandles[2].getName();
			cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_2,
			                       Graphics::Shader::ShaderDescriptor::SAMPLER_2D);
		}
	}

	if (textureCount >= 4) {
		// Don't know yet what this extra texture is supposed to be.
		if (!phandles[3].empty()) {
			materialName += ".";
			materialName += phandles[3].getName();
			cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_3,
			                       Graphics::Shader::ShaderDescriptor::SAMPLER_2D);
		}
	}

	if (penvmap) {
		if (envmapmode == kModeEnvironmentBlendedOver) {
			materialName += penvmap->getName();
			// Figure out if a cube or sphere map is used.
			if (penvmap->getTexture().getImage().isCubeMap()) {
				cripter.addPass(Graphics::Shader::ShaderDescriptor::ENV_CUBE,
				                Graphics::Shader::ShaderDescriptor::BLEND_DST_ALPHA);
			} else {
				cripter.addPass(Graphics::Shader::ShaderDescriptor::ENV_SPHERE,
				                Graphics::Shader::ShaderDescriptor::BLEND_DST_ALPHA);
			}
		}
	}

	if (_mesh->alpha < 1.0f) {
		materialFlags &= ~Shader::ShaderMaterial::MATERIAL_OPAQUE;  // Make sure it's not actually opaque.
		materialFlags |= Shader::ShaderMaterial::MATERIAL_TRANSPARENT;
	}

	cripter.genName(vertexShaderName);
	fragmentShaderName = vertexShaderName + ".frag";
	vertexShaderName += ".vert";

	// Ok, material doesn't exist. Check on the shaders.
	Shader::ShaderObject *vertexObject = ShaderMan.getShaderObject(vertexShaderName, Shader::SHADER_VERTEX);
	Shader::ShaderObject *fragmentObject = ShaderMan.getShaderObject(fragmentShaderName, Shader::SHADER_FRAGMENT);

	// Should be checking vert and frag shader separately, but they really should exist together anyway.
	if (!vertexObject) {
		// No object found. Generate a shader then.
		bool isGL3 = GfxMan.isGL3();

		Common::UString vertexStringFinal;
		Common::UString fragmentStringFinal;

		cripter.build(isGL3, vertexStringFinal, fragmentStringFinal);

		vertexObject = ShaderMan.getShaderObject(vertexShaderName, vertexStringFinal, Shader::SHADER_VERTEX);
		fragmentObject = ShaderMan.getShaderObject(fragmentShaderName, fragmentStringFinal, Shader::SHADER_FRAGMENT);
	}

	// Shader objects should now exist, so go ahead and make the material and surface.
	surface = new Shader::ShaderSurface(vertexObject, materialName);
	material = new Shader::ShaderMaterial(fragmentObject, materialName);
	material->setFlags(materialFlags);
	if (materialFlags & Shader::ShaderMaterial::MATERIAL_CUSTOM_BLEND) {
		material->setBlendSrcRGB(GL_ZERO);
		material->setBlendSrcAlpha(GL_ZERO);
		material->setBlendDstRGB(GL_ONE_MINUS_SRC_COLOR);
		material->setBlendDstAlpha(GL_ONE_MINUS_SRC_ALPHA);
	}

	if (penvmap) {
		material->setTexture("sampler_7_id", *penvmap);
	}

	if (textureCount >= 1) {
		if (!phandles[0].empty()) {
			material->setTexture("sampler_0_id", phandles[0]);
		}
	}

	if (textureCount >= 2) {
		if (!phandles[1].empty()) {
			material->setTexture("sampler_1_id", phandles[1]);
		}
	}

	surface->setVariable("_objectModelviewMatrix", &_renderTransform);
	surface->setVariable("_bindPose", &_absoluteBaseTransform);
	surface->setVariable("_boneTransforms", pmesh->data->rawMesh->getBoneTransforms().data());
	material->setVariable("_alpha", &_alpha);
	material->setVariable("_ambient", pmesh->ambient);

	_renderableArray.push_back(Shader::ShaderRenderable(surface, material, pmesh->data->rawMesh));
}

} // End of namespace Aurora

} // End of namespace Graphics
