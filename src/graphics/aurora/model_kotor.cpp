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
 *  Loading MDL files found in Star Wars: Knights of the Old Republic.
 */

/* Based on cchargin's KotOR model specs
 * (<https://home.comcast.net/~cchargin/kotor/mdl_info.html>).
 */

#include "external/glm/gtc/type_ptr.hpp"

#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"
#include "src/common/strutil.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"

#include "src/graphics/aurora/model_kotor.h"
#include "src/graphics/aurora/animation.h"
#include "src/graphics/aurora/animnode.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"
#include "src/graphics/aurora/skeletalanimation.h"

#include "src/graphics/shader/materialman.h"
#include "src/graphics/shader/surfaceman.h"

#include "src/graphics/images/decoder.h"

// This is included if a mesh wants a unique name.
#include "src/common/uuid.h"

// Disable the "unused variable" warnings while most stuff is still stubbed
IGNORE_UNUSED_VARIABLES

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

Model_KotOR::ParserContext::ParserContext(const Common::UString &name,
                                          const Common::UString &t, bool k2, bool x) :
	mdl(0), mdx(0), state(0), texture(t), kotor2(k2), xbox(x), mdxStructSize(0), vertexCount(0),
	offNodeData(0) {

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

Model_KotOR::ParserContext::~ParserContext() {
	delete mdl;
	delete mdx;

	clear();
}

void Model_KotOR::ParserContext::clear() {
	for (std::list<ModelNode_KotOR *>::iterator n = nodes.begin(); n != nodes.end(); ++n)
		delete *n;
	nodes.clear();

	delete state;
	state = 0;
}


Model_KotOR::Model_KotOR(const Common::UString &name, bool kotor2, bool xbox, ModelType type,
                         const Common::UString &texture, ModelCache *modelCache) :
	Model(type) {

	_fileName = name;
	_positionRelative = true;

	ParserContext ctx(name, texture, kotor2, xbox);

	load(ctx);

	if (_hasSkinNodes) {
		fillBoneNodeMap();
		computeNodeTransforms();
		reparentHeadNodes();
	}

	loadSuperModel(modelCache, kotor2, xbox);

	finalize();
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

	_name = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32);
	ctx.mdlName = _name;

	uint32 nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32 nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24 + 4); // Unknown + Reference count

	uint8 type = ctx.mdl->readByte();

	ctx.mdl->skip(3 + 2); // Padding + Unknown

	uint8 classification = ctx.mdl->readByte();
	uint8 fogged         = ctx.mdl->readByte();

	ctx.mdl->skip(4); // Unknown

	uint32 animOffset, animCount;
	readArrayDef(*ctx.mdl, animOffset, animCount);

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

	_superModelName = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32);

	ctx.mdl->skip(4); // Root node pointer again

	ctx.mdl->skip(12); // Unknown

	uint32 nameOffset, nameCount;
	readArrayDef(*ctx.mdl, nameOffset, nameCount);

	std::vector<uint32> nameOffsets;
	readArray(*ctx.mdl, ctx.offModelData + nameOffset, nameCount, nameOffsets);

	readStrings(*ctx.mdl, nameOffsets, ctx.offModelData, ctx.names);

	newState(ctx);

	ModelNode_KotOR *rootNode = new ModelNode_KotOR(*this);
	ctx.nodes.push_back(rootNode);

	ctx.mdl->seek(ctx.offModelData + nodeHeadPointer);
	rootNode->load(ctx);

	addState(ctx);

	std::vector<uint32> animOffsets;
	readArray(*ctx.mdl, ctx.offModelData + animOffset, animCount, animOffsets);

	for (std::vector<uint32>::const_iterator offset = animOffsets.begin(); offset != animOffsets.end(); ++offset) {
		newState(ctx);

		if (readAnim(ctx, ctx.offModelData + *offset))
			addState(ctx);

		ctx.clear();
	}
}

bool Model_KotOR::readAnim(ParserContext &ctx, uint32 offset) {
	ctx.mdl->seek(offset);

	ctx.mdl->skip(8); // Function pointers

	ctx.state->name = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32);

	if (_stateMap.find(ctx.state->name) != _stateMap.end()) {
		/* TODO: This happens on two models in module 001EBO, the first area of
		 * KotOR2 (the Ebon Hawk drifting in space):
		 * - "f3p1a" in model "S_Female01" (90 and 93 model nodes)
		 * - "walkinj" in model "P_HK47" (53 and 38 model nodes)
		 *
		 * We currently keep the first animation and throw away all subsequent
		 * duplicates. Maybe that's the right way, maybe not.
		 */

		warning("Duplicate animation \"%s\" in model \"%s\"", ctx.state->name.c_str(), _name.c_str());
		return false;
	}

	uint32 nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32 nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24 + 4); // Unknown + Reference count

	uint8 type = ctx.mdl->readByte();

	ctx.mdl->skip(3); // Padding + Unknown

	float animLength = ctx.mdl->readIEEEFloatLE();
	float transTime  = ctx.mdl->readIEEEFloatLE();

	const Common::UString animRoot = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32);

	uint32 eventOffset, eventCount;
	readArrayDef(*ctx.mdl, eventOffset, eventCount);

	ctx.mdl->skip(4); // Padding + Unknown

	ModelNode_KotOR *rootNode = new ModelNode_KotOR(*this);
	ctx.nodes.push_back(rootNode);

	ctx.mdl->seek(ctx.offModelData + nodeHeadPointer);
	rootNode->load(ctx);

	Animation *anim = new SkeletalAnimation(4);

	anim->setName(ctx.state->name);
	anim->setLength(animLength);
	anim->setTransTime(transTime);

	_animationMap.insert(std::make_pair(ctx.state->name, anim));

	for (std::list<ModelNode_KotOR *>::iterator n = ctx.nodes.begin(); n != ctx.nodes.end(); ++n) {
		AnimNode *animnode = new AnimNode(*n);

		anim->addAnimNode(animnode);
	}

	return true;
}

void Model_KotOR::loadSuperModel(ModelCache *modelCache, bool kotor2, bool xbox) {
	if (!_superModelName.empty() && _superModelName != "NULL") {
		bool foundInCache = false;

		if (modelCache) {
			ModelCache::iterator super = modelCache->find(_superModelName);
			if (super != modelCache->end()) {
				_superModel = super->second;

				foundInCache = true;
			}
		}

		if (!_superModel)
			_superModel = new Model_KotOR(_superModelName, kotor2, xbox, _type, "", modelCache);

		if (modelCache && !foundInCache)
			modelCache->insert(std::make_pair(_superModelName, _superModel));
	}
}

void Model_KotOR::readStrings(Common::SeekableReadStream &mdl,
		const std::vector<uint32> &offsets, uint32 offset,
		std::vector<Common::UString> &strings) {

	size_t pos = mdl.pos();

	strings.reserve(offsets.size());
	for (std::vector<uint32>::const_iterator o = offsets.begin(); o != offsets.end(); ++o) {
		mdl.seek(offset + *o);

		strings.push_back(Common::readString(mdl, Common::kEncodingASCII));
	}

	mdl.seek(pos);
}

void Model_KotOR::newState(ParserContext &ctx) {
	ctx.clear();

	ctx.state = new State;
}

void Model_KotOR::addState(ParserContext &ctx) {
	if (!ctx.state || ctx.nodes.empty()) {
		ctx.clear();
		return;
	}

	for (std::list<ModelNode_KotOR *>::iterator n = ctx.nodes.begin();
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

void Model_KotOR::fillBoneNodeMap() {
	for (const auto &n : getNodes()) {
		ModelNode::Mesh *mesh = n->getMesh();
		if (mesh && mesh->skin) {
			ModelNode::Skin *skin = mesh->skin;

			skin->boneNodeMap.resize(skin->boneMappingCount);
			std::fill(skin->boneNodeMap.begin(), skin->boneNodeMap.end(), nullptr);

			const NodeList &nodes = getNodes();

			for (uint16 i = 0; i < skin->boneMappingCount; ++i) {
				int index = static_cast<int>(skin->boneMapping[i]);
				if (index != -1) {
					ModelNode *boneNode = nodes[i];
					skin->boneNodeMap[index] = boneNode;
				}
			}

			if (GfxMan.isRendererExperimental()) {
				std::vector<float> &boneTransforms = mesh->data->rawMesh->getBoneTransforms();
				boneTransforms.resize(16 * skin->boneMappingCount);
				std::fill(boneTransforms.begin(), boneTransforms.end(), 0.0f);
			}
		}
	}
}

void Model_KotOR::reparentHeadNodes() {
	ModelNode *root = _currentState->rootNodes[0];

	ModelNode *head = getNode("head");
	if (head && (head->getParent() != root))
		head->reparentTo(root);

	ModelNode *tongue = getNode("tongue");
	if (tongue && (tongue->getParent() != root))
		tongue->reparentTo(root);
}


ModelNode_KotOR::ModelNode_KotOR(Model &model) :
	ModelNode(model) {
}

ModelNode_KotOR::~ModelNode_KotOR() {
}

void ModelNode_KotOR::load(Model_KotOR::ParserContext &ctx) {
	ctx.flags = ctx.mdl->readUint16LE();
	uint16 superNode = ctx.mdl->readUint16LE();

	_nodeNumber = ctx.mdl->readUint16LE();

	if (_nodeNumber < ctx.names.size())
		_name = ctx.names[_nodeNumber];

	ctx.mdl->skip(6 + 4); // Unknown + parent pointer

	_position   [0] = ctx.mdl->readIEEEFloatLE();
	_position   [1] = ctx.mdl->readIEEEFloatLE();
	_position   [2] = ctx.mdl->readIEEEFloatLE();
	_orientation[3] = Common::rad2deg(acos(ctx.mdl->readIEEEFloatLE()) * 2.0f);
	_orientation[0] = ctx.mdl->readIEEEFloatLE();
	_orientation[1] = ctx.mdl->readIEEEFloatLE();
	_orientation[2] = ctx.mdl->readIEEEFloatLE();

	_positionBuffer[0] = _position[0];
	_positionBuffer[1] = _position[1];
	_positionBuffer[2] = _position[2];

	_orientationBuffer[0] = _orientation[0];
	_orientationBuffer[1] = _orientation[1];
	_orientationBuffer[2] = _orientation[2];
	_orientationBuffer[3] = _orientation[3];

	uint32 childrenOffset, childrenCount;
	Model::readArrayDef(*ctx.mdl, childrenOffset, childrenCount);

	std::vector<uint32> children;
	Model::readArray(*ctx.mdl, ctx.offModelData + childrenOffset, childrenCount, children);

	uint32 controllerKeyOffset, controllerKeyCount;
	Model::readArrayDef(*ctx.mdl, controllerKeyOffset, controllerKeyCount);

	uint32 controllerDataOffset, controllerDataCount;
	Model::readArrayDef(*ctx.mdl, controllerDataOffset, controllerDataCount);

	std::vector<float> controllerDataFloat;
	Model::readArray(*ctx.mdl, ctx.offModelData + controllerDataOffset,
	                 controllerDataCount, controllerDataFloat);

	std::vector<uint32> controllerDataInt;
	Model::readArray(*ctx.mdl, ctx.offModelData + controllerDataOffset,
	                 controllerDataCount, controllerDataInt);

	readNodeControllers(ctx, ctx.offModelData + controllerKeyOffset,
	                    controllerKeyCount, controllerDataFloat, controllerDataInt);

	if ((ctx.flags & 0xFC00) != 0)
		throw Common::Exception("Unknown node flags %04X", ctx.flags);

	if (ctx.flags & kNodeFlagHasLight) {
		// TODO: Light
		ctx.mdl->skip(0x5C);
	}

	if (ctx.flags & kNodeFlagHasEmitter) {
		// TODO: Emitter
		ctx.mdl->skip(0xD8);
	}

	if (ctx.flags & kNodeFlagHasReference) {
		// TODO: Reference
		ctx.mdl->skip(0x44);
	}

	if (ctx.flags & kNodeFlagHasMesh) {
		readMesh(ctx);
	}

	if (ctx.flags & kNodeFlagHasSkin) {
		readSkin(ctx);
		_model->notifyHasSkinNodes();
	}

	if (ctx.flags & kNodeFlagHasAnim) {
		// TODO: Anim
		ctx.mdl->skip(0x38);
	}

	if (ctx.flags & kNodeFlagHasDangly) {
		// TODO: Dangly
		ctx.mdl->skip(0x18);
	}

	if (ctx.flags & kNodeFlagHasAABB) {
		// TODO: AABB
		ctx.mdl->skip(0x4);
	}

	for (std::vector<uint32>::const_iterator child = children.begin(); child != children.end(); ++child) {
		ModelNode_KotOR *childNode = new ModelNode_KotOR(*_model);
		ctx.nodes.push_back(childNode);

		childNode->setParent(this);

		ctx.mdl->seek(ctx.offModelData + *child);
		childNode->load(ctx);
	}

	if (_mesh && _mesh->data) {
		Common::UString meshName = ctx.mdlName;
		meshName += ".";
		if (ctx.state->name.size() != 0) {
			meshName += ctx.state->name;
		} else {
			meshName += "xoreos.default";
		}
		meshName += ".";
		meshName += _name;

		if (GfxMan.isRendererExperimental()) {
			/**
			 * Dirty hack around an issue in KotOR2 where a tile can have multiple meshes
			 * of exactly the same name. This dirty hack will double up on static objects
			 * without state, but hopefully they're relatively few and it won't impact
			 * performance too much.
			 * A future improvement will be to see if an entire model has already been
			 * loaded and to use that directly: that should prevent models with an empty
			 * state from being affected by this dirty hack.
			 */
			Graphics::Mesh::Mesh *mystery_mesh = MeshMan.getMesh(meshName);
			if (ctx.state->name.size() == 0) {
				while (mystery_mesh) {
					meshName += "_";
					mystery_mesh = MeshMan.getMesh(meshName);
				}
			}

			if (!mystery_mesh) {
				Graphics::Mesh::Mesh *checkMesh = MeshMan.getMesh(meshName);
				if (checkMesh) {
					delete _mesh->data->rawMesh;
					_mesh->data->rawMesh = checkMesh;
				} else {
					_mesh->data->rawMesh->setName(meshName);
					_mesh->data->rawMesh->init();
					MeshMan.addMesh(_mesh->data->rawMesh);
				}
			} else {
				delete _mesh->data->rawMesh;
				_mesh->data->rawMesh = mystery_mesh;
			}

			buildMaterial();

		} else {
			/**
			 * Because meshes need to be unique right now, at least until animation can use
			 * vertex shaders instead of vertex data duplication, need to generate a unique
			 * name for the mesh and add it to the mesh manager. This is important; the mesh
			 * manager is responsible for later deleting it.
			 */
			meshName += "#" + Common::generateIDRandomString();
			_mesh->data->rawMesh->setName(meshName);
			_mesh->data->rawMesh->init();

			MeshMan.addMesh(_mesh->data->rawMesh);
		}
	}
}

void ModelNode_KotOR::readNodeControllers(Model_KotOR::ParserContext &ctx,
		uint32 offset, uint32 count, std::vector<float> &dataFloat, std::vector<uint32> &dataInt) {
	uint32 pos = ctx.mdl->seek(offset);
	for (uint32 i = 0; i < count; i++) {
		uint32 type = ctx.mdl->readUint32LE();
		ctx.mdl->skip(2);
		uint16 rowCount = ctx.mdl->readUint16LE();
		uint16 timeIndex = ctx.mdl->readUint16LE();
		uint16 dataIndex = ctx.mdl->readUint16LE();
		uint8 columnCount = ctx.mdl->readByte();
		ctx.mdl->skip(3);
		switch (type) {
			case kControllerTypePosition:
				readPositionController(columnCount, rowCount, timeIndex, dataIndex, dataFloat);
				break;
			case kControllerTypeOrientation:
				readOrientationController(columnCount, rowCount, timeIndex, dataIndex, dataFloat, dataInt);
				break;
		}
	}
	ctx.mdl->seek(pos);
}

void ModelNode_KotOR::buildMaterial() {
	ModelNode::buildMaterial();
}

void ModelNode_KotOR::declareShaderInputs(MaterialConfiguration &config, Shader::ShaderDescriptor &cripter) {
	ModelNode::declareShaderInputs(config, cripter);
	cripter.declareInput(Shader::ShaderDescriptor::INPUT_UV1);

	if (_mesh->skin && _mesh->skin->boneMappingCount) {
		config.materialName += ".skinned";
		cripter.declareUniform(Shader::ShaderDescriptor::UNIFORM_V_BIND_POSE);
		cripter.declareUniform(Shader::ShaderDescriptor::UNIFORM_V_BONE_TRANSFORMS, _mesh->skin->boneMappingCount);
		cripter.declareInput(Shader::ShaderDescriptor::INPUT_BONE_INDICES);
		cripter.declareInput(Shader::ShaderDescriptor::INPUT_BONE_WEIGHTS);
	}
}

void ModelNode_KotOR::setupShaderTexture(MaterialConfiguration &config, int textureIndex, Shader::ShaderDescriptor &cripter) {
	if (textureIndex == 1) {
		if (config.phandles[1].empty())
			return;

		config.materialName += "." + config.phandles[1].getName();

		cripter.declareSampler(Shader::ShaderDescriptor::SAMPLER_TEXTURE_1,
		                       Shader::ShaderDescriptor::SAMPLER_2D);

		cripter.connect(Shader::ShaderDescriptor::SAMPLER_TEXTURE_1,
		                Shader::ShaderDescriptor::INPUT_UV1,
		                Shader::ShaderDescriptor::TEXTURE_LIGHTMAP);

		cripter.addPass(Shader::ShaderDescriptor::TEXTURE_LIGHTMAP,
		                Shader::ShaderDescriptor::BLEND_MULTIPLY);

	} else {
		ModelNode::setupShaderTexture(config, textureIndex, cripter);
	}
}

void ModelNode_KotOR::readPositionController(uint8 columnCount, uint16 rowCount, uint16 timeIndex,
		uint16 dataIndex, std::vector<float> &data) {
	bool bezier = columnCount & 16;
	switch (columnCount) {
		case 3:
		case 19:
			for (int r = 0; r < rowCount; r++) {
				PositionKeyFrame p;
				p.time = data[timeIndex + r];
				int index = dataIndex + (bezier ? 9 : 3) * r;
				p.x = data[index + 0];
				p.y = data[index + 1];
				p.z = data[index + 2];
				_positionFrames.push_back(p);
			}
			break;
		default:
			warning("Position controller with %d values", columnCount);
			break;
	}
}

void ModelNode_KotOR::readOrientationController(uint8 columnCount, uint16 rowCount,
		uint16 timeIndex, uint16 dataIndex, std::vector<float> &dataFloat, std::vector<uint32> &dataInt) {
	switch (columnCount) {
		case 2:
			for (int r = 0; r < rowCount; r++) {
				QuaternionKeyFrame q;
				q.time = dataFloat[timeIndex + r];

				uint32 temp = dataInt[dataIndex + r];
				q.x = 1.0f - static_cast<float>(temp & 0x7ff) / 1023.0f;
				q.y = 1.0f - static_cast<float>((temp >> 11) & 0x7ff) / 1023.0f;
				q.z = 1.0f - static_cast<float>(temp >> 22) / 511.0f;

				float temp2 = q.x * q.x + q.y * q.y + q.z * q.z;
				if (temp2 < 1.0f)
					q.q = -sqrtf(1.0f - temp2);
				else {
					temp2 = sqrtf(temp2);
					q.x = q.x / temp2;
					q.y = q.y / temp2;
					q.z = q.z / temp2;
					q.q = 0.0f;
				}

				_orientationFrames.push_back(q);
			}
			break;
		case 4:
			for (int r = 0; r < rowCount; r++) {
				QuaternionKeyFrame q;
				q.time = dataFloat[timeIndex + r];
				int index = dataIndex + 4 * r;
				q.x = dataFloat[index + 0];
				q.y = dataFloat[index + 1];
				q.z = dataFloat[index + 2];
				q.q = dataFloat[index + 3];
				_orientationFrames.push_back(q);
			}
			break;
		default:
			warning("Orientation controller with %d values", columnCount);
			break;
	}
}

void ModelNode_KotOR::readMesh(Model_KotOR::ParserContext &ctx) {
	size_t P = ctx.mdl->pos();

	ctx.mdl->skip(8); // Function pointers

	uint32 facesOffset, facesCount;
	Model::readArrayDef(*ctx.mdl, facesOffset, facesCount);

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

	_mesh->diffuse[0] = ctx.mdl->readIEEEFloatLE();
	_mesh->diffuse[1] = ctx.mdl->readIEEEFloatLE();
	_mesh->diffuse[2] = ctx.mdl->readIEEEFloatLE();

	_mesh->ambient[0] = ctx.mdl->readIEEEFloatLE();
	_mesh->ambient[1] = ctx.mdl->readIEEEFloatLE();
	_mesh->ambient[2] = ctx.mdl->readIEEEFloatLE();

	_mesh->specular[0] = 0;
	_mesh->specular[1] = 0;
	_mesh->specular[2] = 0;

	uint32 transparencyHint = ctx.mdl->readUint32LE();

	_mesh->hasTransparencyHint = true;
	_mesh->transparencyHint    = (transparencyHint != 0);

	std::vector<Common::UString> textures;

	textures.push_back(Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32));
	textures.push_back(Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32));

	ctx.mdl->skip(24); // Unknown

	ctx.mdl->skip(12); // Vertex indices counts

	uint32 offOffVerts, offOffVertsCount;
	Model::readArrayDef(*ctx.mdl, offOffVerts, offOffVertsCount);

	if (offOffVertsCount > 1)
		throw Common::Exception("Face offsets offsets count wrong (%d)", offOffVertsCount);

	ctx.mdl->skip(12); // Unknown

	ctx.mdl->skip(24 + 16); // Unknown

	ctx.mdxStructSize = ctx.mdl->readUint32LE();

	ctx.mdl->skip(8); // Unknown

	uint32 offNormals = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4); // Unknown

	uint32 offUV[2];
	offUV[0] = ctx.mdl->readUint32LE();
	offUV[1] = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24); // Unknown

	ctx.vertexCount  = ctx.mdl->readUint16LE();
	uint16 textureCount = ctx.mdl->readUint16LE();

	ctx.mdl->skip(2);

	byte unknownFlag1 = ctx.mdl->readByte();
	_mesh->shadow  = ctx.mdl->readByte() == 1;
	byte unknownFlag2 = ctx.mdl->readByte();
	_mesh->render  = ctx.mdl->readByte() == 1;

	ctx.mdl->skip(10);

	if (ctx.kotor2)
		ctx.mdl->skip(8);

	ctx.offNodeData = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4);

	if ((offOffVertsCount < 1) || (ctx.vertexCount == 0) || (facesCount == 0))
		return;

	_render = _mesh->render;
	_mesh->data = new MeshData();
	_mesh->data->envMapMode = kModeEnvironmentBlendedOver;
	_mesh->data->rawMesh = new Graphics::Mesh::Mesh();
	_mesh->data->rawMesh->setBindPosePtr(&_absoluteBaseTransform);

	uint32 endPos = ctx.mdl->pos();

	if (textureCount > 2) {
		warning("Model_KotOR::readMesh(): textureCount > 2 (%d)", textureCount);
		textureCount = 2;
	}

	if ((textureCount > 0) && !ctx.texture.empty())
		textures[0] = ctx.texture;

	textures.resize(textureCount);
	loadTextures(textures);


	// Read vertices (interleaved)

	VertexDecl vertexDecl;

	vertexDecl.push_back(VertexAttrib(VPOSITION, 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VNORMAL  , 3, GL_FLOAT));

	if (ctx.flags & kNodeFlagHasSkin) {
		vertexDecl.push_back(VertexAttrib(VBONEWEIGHTS, 4, GL_FLOAT));
		vertexDecl.push_back(VertexAttrib(VBONEINDICES, 4, GL_FLOAT));
	}

	for (uint t = 0; t < textureCount; t++)
		vertexDecl.push_back(VertexAttrib(VTCOORD + t, 2, GL_FLOAT));

	_mesh->data->rawMesh->getVertexBuffer()->setVertexDeclInterleave(ctx.vertexCount, vertexDecl);
	_mesh->data->initialVertexCoords.resize(3 * ctx.vertexCount);

	float *v = reinterpret_cast<float *>(_mesh->data->rawMesh->getVertexBuffer()->getData());
	float *iv = _mesh->data->initialVertexCoords.data();

	for (uint32 i = 0; i < ctx.vertexCount; i++) {
		// Position
		ctx.mdx->seek(ctx.offNodeData + i * ctx.mdxStructSize);
		iv[0] = ctx.mdx->readIEEEFloatLE();
		iv[1] = ctx.mdx->readIEEEFloatLE();
		iv[2] = ctx.mdx->readIEEEFloatLE();
		*v++ = iv[0];
		*v++ = iv[1];
		*v++ = iv[2];
		iv += 3;

		// Normal
		//ctx.mdx->seek(offNodeData + i * mdxStructSize + offNormals);
		*v++ = ctx.mdx->readIEEEFloatLE();
		*v++ = ctx.mdx->readIEEEFloatLE();
		*v++ = ctx.mdx->readIEEEFloatLE();

		// Bone indices and bone weights are loaded later on
		if (ctx.flags & kNodeFlagHasSkin)
			v += 8;

		// TexCoords
		for (uint16 t = 0; t < textureCount; t++) {
			if (offUV[t] != 0xFFFFFFFF) {
				ctx.mdx->seek(ctx.offNodeData + i * ctx.mdxStructSize + offUV[t]);
				*v++ = ctx.mdx->readIEEEFloatLE();
				*v++ = ctx.mdx->readIEEEFloatLE();
			} else {
				*v++ = 0.0f;
				*v++ = 0.0f;
			}
		}
	}


	// Read faces

	ctx.mdl->seek(ctx.offModelData + offOffVerts);
	uint32 offVerts = ctx.mdl->readUint32LE();

	ctx.mdl->seek(ctx.offModelData + offVerts);

	_mesh->data->rawMesh->getIndexBuffer()->setSize(facesCount * 3, sizeof(uint16), GL_UNSIGNED_SHORT);

	uint16 *f = reinterpret_cast<uint16 *>(_mesh->data->rawMesh->getIndexBuffer()->getData());
	for (uint32 i = 0; i < facesCount * 3; i++)
		f[i] = ctx.mdl->readUint16LE();

	createBound();

	ctx.mdl->seek(endPos);
}

void ModelNode_KotOR::readSkin(Model_KotOR::ParserContext &ctx) {
	/* The models found in the Xbox versions store bone indices as int16,
	 * while the Windows/Mac/Linux versions use floats. */

	ctx.mdl->skip(ctx.xbox ? 8 : 12);
	uint32 mdxOffsetBoneWeights = ctx.mdl->readUint32LE();
	uint32 mdxOffsetBoneMappingId = ctx.mdl->readUint32LE();
	uint32 boneMappingOffset = ctx.mdl->readUint32LE();
	uint32 boneMappingCount = ctx.mdl->readUint32LE();
	ctx.mdl->skip(72);

	_mesh->skin = new Skin();
	_mesh->skin->boneMappingCount = boneMappingCount;

	const uint32 pos = ctx.mdl->seek(ctx.offModelData + boneMappingOffset);

	for (uint32 i = 0; i < boneMappingCount; i++) {
		const float index = ctx.xbox ? static_cast<float>(ctx.mdl->readSint16LE()) : ctx.mdl->readIEEEFloatLE();

		_mesh->skin->boneMapping.push_back(index);
	}

	ctx.mdl->seek(pos);

	std::vector<float> &boneWeights = _mesh->skin->boneWeights;
	std::vector<float> &boneMappingId = _mesh->skin->boneMappingId;

	VertexBuffer *vertexBuffer = _mesh->data->rawMesh->getVertexBuffer();
	float *vertexData = static_cast<float *>(vertexBuffer->getData());

	for (int i = 0; i < ctx.vertexCount; i++) {
		// Skip position and normal attributes
		vertexData += 6;

		// Bone weights
		ctx.mdx->seek(ctx.offNodeData + i * ctx.mdxStructSize + mdxOffsetBoneWeights);
		vertexData[0] = ctx.mdx->readIEEEFloatLE();
		vertexData[1] = ctx.mdx->readIEEEFloatLE();
		vertexData[2] = ctx.mdx->readIEEEFloatLE();
		vertexData[3] = ctx.mdx->readIEEEFloatLE();

		boneWeights.push_back(vertexData[0]);
		boneWeights.push_back(vertexData[1]);
		boneWeights.push_back(vertexData[2]);
		boneWeights.push_back(vertexData[3]);

		vertexData += 4;

		// Bone mapping identifiers
		ctx.mdx->seek(ctx.offNodeData + i * ctx.mdxStructSize + mdxOffsetBoneMappingId);
		vertexData[0] = ctx.xbox ? static_cast<float>(ctx.mdx->readSint16LE()) : ctx.mdx->readIEEEFloatLE();
		vertexData[1] = ctx.xbox ? static_cast<float>(ctx.mdx->readSint16LE()) : ctx.mdx->readIEEEFloatLE();
		vertexData[2] = ctx.xbox ? static_cast<float>(ctx.mdx->readSint16LE()) : ctx.mdx->readIEEEFloatLE();
		vertexData[3] = ctx.xbox ? static_cast<float>(ctx.mdx->readSint16LE()) : ctx.mdx->readIEEEFloatLE();

		boneMappingId.push_back(vertexData[0]);
		boneMappingId.push_back(vertexData[1]);
		boneMappingId.push_back(vertexData[2]);
		boneMappingId.push_back(vertexData[3]);

		vertexData += 4;

		// Skip textures coordinates
		vertexData += 2 * _mesh->data->textures.size();
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
