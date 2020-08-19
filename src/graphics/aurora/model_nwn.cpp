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
 *  Loading MDL files found in Neverwinter Nights.
 */

/* Based on the binary MDL specs by Torlack.
 *
 * Torlack's own site is down, but our docs repository hosts a
 * a mirror (<https://github.com/xoreos/xoreos-docs>).
 */

#include <cassert>

#include <boost/unordered_set.hpp>

#include "external/glm/glm.hpp"

#include "src/common/system.h"
#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/debug.h"
#include "src/common/readstream.h"
#include "src/common/strutil.h"
#include "src/common/encoding.h"
#include "src/common/streamtokenizer.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"

#include "src/graphics/aurora/model_nwn.h"
#include "src/graphics/aurora/animation.h"
#include "src/graphics/aurora/animnode.h"
#include "src/graphics/aurora/animationchannel.h"

#include "src/graphics/shader/materialman.h"
#include "src/graphics/shader/surfaceman.h"

// Disable the "unused variable" warnings while most stuff is still stubbed
IGNORE_UNUSED_VARIABLES

using Common::kDebugGraphics;

static const int kNodeFlagHasHeader    = 0x00000001;
static const int kNodeFlagHasLight     = 0x00000002;
static const int kNodeFlagHasEmitter   = 0x00000004;
static const int kNodeFlagHasReference = 0x00000010;
static const int kNodeFlagHasMesh      = 0x00000020;
static const int kNodeFlagHasSkin      = 0x00000040;
static const int kNodeFlagHasAnim      = 0x00000080;
static const int kNodeFlagHasDangly    = 0x00000100;
static const int kNodeFlagHasAABB      = 0x00000200;

static const uint16_t kControllerTypePosition             = 8;
static const uint16_t kControllerTypeOrientation          = 20;
static const uint16_t kControllerTypeScale                = 36;
static const uint16_t kControllerTypeColor                = 76;
static const uint16_t kControllerTypeRadius               = 88;
static const uint16_t kControllerTypeShadowRadius         = 96;
static const uint16_t kControllerTypeVerticalDisplacement = 100;
static const uint16_t kControllerTypeMultiplier           = 140;
static const uint16_t kControllerTypeAlphaEnd             = 80;
static const uint16_t kControllerTypeAlphaStart           = 84;
static const uint16_t kControllerTypeBirthRate            = 88;
static const uint16_t kControllerTypeBounce_Co            = 92;
static const uint16_t kControllerTypeColorEnd             = 96;
static const uint16_t kControllerTypeColorStart           = 108;
static const uint16_t kControllerTypeCombineTime          = 120;
static const uint16_t kControllerTypeDrag                 = 124;
static const uint16_t kControllerTypeFPS                  = 128;
static const uint16_t kControllerTypeFrameEnd             = 132;
static const uint16_t kControllerTypeFrameStart           = 136;
static const uint16_t kControllerTypeGrav                 = 140;
static const uint16_t kControllerTypeLifeExp              = 144;
static const uint16_t kControllerTypeMass                 = 148;
static const uint16_t kControllerTypeP2P_Bezier2          = 152;
static const uint16_t kControllerTypeP2P_Bezier3          = 156;
static const uint16_t kControllerTypeParticleRot          = 160;
static const uint16_t kControllerTypeRandVel              = 164;
static const uint16_t kControllerTypeSizeStart            = 168;
static const uint16_t kControllerTypeSizeEnd              = 172;
static const uint16_t kControllerTypeSizeStart_Y          = 176;
static const uint16_t kControllerTypeSizeEnd_Y            = 180;
static const uint16_t kControllerTypeSpread               = 184;
static const uint16_t kControllerTypeThreshold            = 188;
static const uint16_t kControllerTypeVelocity             = 192;
static const uint16_t kControllerTypeXSize                = 196;
static const uint16_t kControllerTypeYSize                = 200;
static const uint16_t kControllerTypeBlurLength           = 204;
static const uint16_t kControllerTypeLightningDelay       = 208;
static const uint16_t kControllerTypeLightningRadius      = 212;
static const uint16_t kControllerTypeLightningScale       = 216;
static const uint16_t kControllerTypeDetonate             = 228;
static const uint16_t kControllerTypeAlphaMid             = 464;
static const uint16_t kControllerTypeColorMid             = 468;
static const uint16_t kControllerTypePercentStart         = 480;
static const uint16_t kControllerTypePercentMid           = 481;
static const uint16_t kControllerTypePercentEnd           = 482;
static const uint16_t kControllerTypeSizeMid              = 484;
static const uint16_t kControllerTypeSizeMid_Y            = 488;
static const uint16_t kControllerTypeSelfIllumColor       = 100;
static const uint16_t kControllerTypeAlpha                = 128;

namespace Graphics {

namespace Aurora {

Model_NWN::ParserContext::ParserContext(const Common::UString &name,
                                        const Common::UString &t) :
	mdl(0), state(0), texture(t) {

	mdl = ResMan.getResource(name, ::Aurora::kFileTypeMDL);
	if (!mdl)
		throw Common::Exception("No such MDL \"%s\"", name.c_str());

	mdl->seek(0);
	isASCII = mdl->readUint32LE() != 0;

	if (isASCII) {
		tokenize = new Common::StreamTokenizer(Common::StreamTokenizer::kRuleIgnoreAll);

		tokenize->addSeparator(' ');
		tokenize->addChunkEnd('\n');
		tokenize->addIgnore('\r');
	} else
		tokenize = 0;
}

Model_NWN::ParserContext::~ParserContext() {
	delete tokenize;
	delete mdl;

	clear();
}

void Model_NWN::ParserContext::clear() {
	for (std::list<ModelNode *>::iterator n = nodes.begin(); n != nodes.end(); ++n)
		delete *n;
	nodes.clear();

	delete state;
	state = 0;
}

bool Model_NWN::ParserContext::findNode(const Common::UString &name,
                                        ModelNode *&node) const {

	node = 0;

	if (name.empty() || (name == "NULL"))
		return true;

	for (std::list<ModelNode *>::const_iterator n = nodes.begin();
	     n != nodes.end(); ++n) {

		if ((*n)->getName() == name) {
			node = *n;
			return true;
		}
	}

	return false;
}


Model_NWN::Model_NWN(const Common::UString &name, ModelType type,
                     const Common::UString &texture, ModelCache *modelCache) :
	Model(type) {

	if (_type == kModelTypeGUIFront) {
		// NWN GUI objects use 0.01 units / pixel
		_scale[0] = _scale[1] = 100.0f;
		_scale[2] = 1.0f;
	}

	_fileName = name;

	ParserContext ctx(name, texture);

	if (ctx.isASCII)
		loadASCII(ctx);
	else
		loadBinary(ctx);

	loadSuperModel(modelCache);

	// These are usually inherited from a supermodel
	populateDefaultAnimations();

	finalize();
}

Model_NWN::~Model_NWN() {
}

void Model_NWN::loadBinary(ParserContext &ctx) {
	ctx.mdl->seek(4);

	uint32_t sizeModelData = ctx.mdl->readUint32LE();
	uint32_t sizeRawData   = ctx.mdl->readUint32LE();

	ctx.offModelData = 12;
	ctx.offRawData   = ctx.offModelData + sizeModelData;

	ctx.mdl->skip(8); // Function pointers

	_name = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 64);
	debugC(kDebugGraphics, 4, "Loading NWN binary model \"%s\": \"%s\"", _fileName.c_str(),
	       _name.c_str());
	ctx.mdlName = _name;

	uint32_t nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32_t nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24 + 4); // Unknown + Reference count

	uint8_t type = ctx.mdl->readByte();

	ctx.mdl->skip(3 + 2); // Padding + Unknown

	uint8_t classification = ctx.mdl->readByte();
	uint8_t fogged         = ctx.mdl->readByte();

	ctx.mdl->skip(4); // Unknown

	uint32_t animOffset, animCount;
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

	_animationScale = ctx.mdl->readIEEEFloatLE();

	_superModelName = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 64);

	newState(ctx);

	ModelNode_NWN_Binary *rootNode = new ModelNode_NWN_Binary(*this);
	ctx.nodes.push_back(rootNode);

	ctx.mdl->seek(ctx.offModelData + nodeHeadPointer);
	rootNode->load(ctx);

	addState(ctx);

	std::vector<uint32_t> animOffsets;
	readArray(*ctx.mdl, ctx.offModelData + animOffset, animCount, animOffsets);

	for (std::vector<uint32_t>::const_iterator offset = animOffsets.begin(); offset != animOffsets.end(); ++offset) {
		newState(ctx);

		readAnimBinary(ctx, ctx.offModelData + *offset);

		addState(ctx);
	}
}

void Model_NWN::loadASCII(ParserContext &ctx) {
	ctx.mdl->seek(0);

	newState(ctx);

	while (!ctx.mdl->eos()) {
		std::vector<Common::UString> line;

		size_t count = ctx.tokenize->getTokens(*ctx.mdl, line, 3);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].makeLower();

		if        (line[0] == "newmodel") {
			if (!_name.empty())
				warning("Model_NWN_ASCII::load(): More than one model definition");

			debugC(kDebugGraphics, 4, "Loading NWN ASCII model \"%s\": \"%s\"", _fileName.c_str(),
			       _name.c_str());

			_name = line[1];
			ctx.mdlName = _name;
		} else if (line[0] == "setsupermodel") {
			if (line[1] != _name)
				warning("Model_NWN_ASCII::load(): setsupermodel: \"%s\" != \"%s\"",
				        line[1].c_str(), _name.c_str());

			if (!line[2].empty() && (line[2] != "NULL"))
				_superModelName = line[2];

		} else if (line[0] == "beginmodelgeom") {
			if (line[1] != _name)
				warning("Model_NWN_ASCII::load(): beginmodelgeom: \"%s\" != \"%s\"",
				        line[1].c_str(), _name.c_str());
		} else if (line[0] == "setanimationscale") {
			Common::parseString(line[1], _animationScale);
		} else if (line[0] == "node") {

			ModelNode_NWN_ASCII *newNode = new ModelNode_NWN_ASCII(*this);
			ctx.nodes.push_back(newNode);

			newNode->load(ctx, line[1], line[2]);

		} else if (line[0] == "newanim") {
			ctx.anims.push_back(ctx.mdl->pos());
			skipAnimASCII(ctx);
		} else if (line[0] == "donemodel") {
			break;
		} else {
			// warning("Unknown MDL command \"%s\"", line[0].c_str());
		}
	}

	addState(ctx);

	for (std::vector<uint32_t>::iterator a = ctx.anims.begin(); a != ctx.anims.end(); ++a) {
		ctx.mdl->seek(*a);
		readAnimASCII(ctx);
	}
}
void Model_NWN::newState(ParserContext &ctx) {
	ctx.clear();

	ctx.hasPosition    = false;
	ctx.hasOrientation = false;
	ctx.state          = new State;
}

void Model_NWN::skipAnimASCII(ParserContext &ctx) {
	bool end = false;

	while (!ctx.mdl->eos()) {
		std::vector<Common::UString> line;

		size_t count = ctx.tokenize->getTokens(*ctx.mdl, line, 1);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].makeLower();

		if (line[0] == "doneanim") {
			end = true;
			break;
		}
	}

	if (!end)
		throw Common::Exception("anim without doneanim");
}

void Model_NWN::readAnimASCII(ParserContext &UNUSED(ctx)) {
	// TODO: Model_NWN_ASCII::readAnimASCII
	//       read in the animation name
	//       there should be a list of animations
	//       each with a bunch of animnodes
	//       each animnode targets a model node
	//       and has a list of position, orientation keyframes
	//       (at time t, position, orientation = x)
}

void Model_NWN::addState(ParserContext &ctx) {
	if (!ctx.state || ctx.nodes.empty()) {
		ctx.clear();
		return;
	}

	for (std::list<ModelNode *>::iterator n = ctx.nodes.begin();
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

void Model_NWN::readAnimBinary(ParserContext &ctx, uint32_t offset) {
	ctx.mdl->seek(offset);

	ctx.mdl->skip(8); // Function pointers

	ctx.state->name = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 64);

	uint32_t nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32_t nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24 + 4); // Unknown + Reference count

	uint8_t type = ctx.mdl->readByte();

	ctx.mdl->skip(3); // Padding

	float animLength = ctx.mdl->readIEEEFloatLE();
	float transTime  = ctx.mdl->readIEEEFloatLE();

	Common::UString animRoot = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 64);

	uint32_t eventOffset, eventCount;
	readArrayDef(*ctx.mdl, eventOffset, eventCount);

	// Associated events
	// TODO: Save in array, then pass to animation class
	ctx.mdl->seek(ctx.offModelData + eventOffset);
	for (uint32_t i = 0; i < eventCount; i++) {
		float after = ctx.mdl->readIEEEFloatLE();

		Common::UString eventName = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32);
	}

	ModelNode_NWN_Binary *rootNode = new ModelNode_NWN_Binary(*this);
	ctx.nodes.push_back(rootNode);

	ctx.mdl->seek(ctx.offModelData + nodeHeadPointer);
	rootNode->load(ctx);

	// We read this in, but what do we do with it??
	// Ah, we call addState, interesting
	// Need to look at interaction with placeable states?
	Animation *anim = new Animation();
	anim->setName(ctx.state->name);
	anim->setLength(animLength);
	anim->setTransTime(transTime);
	_animationMap.insert(std::make_pair(ctx.state->name, anim));
	debugC(kDebugGraphics, 4, "Loaded animation \"%s\" in model \"%s\"", ctx.state->name.c_str(), _name.c_str());

	for (std::list<ModelNode *>::iterator n = ctx.nodes.begin();
	     n != ctx.nodes.end(); ++n) {
		AnimNode *animnode = new AnimNode(*n);
		anim->addAnimNode(animnode);
	}

}

void Model_NWN::loadSuperModel(ModelCache *modelCache) {
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
			_superModel = new Model_NWN(_superModelName, _type, "", modelCache);

		if (modelCache && !foundInCache)
			modelCache->insert(std::make_pair(_superModelName, _superModel));
	}
}

struct DefaultAnim {
	const char *name;
	int probability;
};

static const DefaultAnim kDefaultAnims[] = {
	{"pausesh" ,   5},
	{"pausebrd",   5},
	{"hturnl"  ,   5},
	{"hturnr"  ,   5},
	{"pause1"  , 100},
	{"pause2"  , 100},
	{"chturnl" ,   5},
	{"chturnr" ,   5},
	{"cpause1" , 100}
};

void Model_NWN::populateDefaultAnimations() {
	for (size_t i = 0; i < ARRAYSIZE(kDefaultAnims); i++) {
		Animation *anim = getAnimation(kDefaultAnims[i].name);
		if (!anim)
			continue;

		addDefaultAnimation(kDefaultAnims[i].name, kDefaultAnims[i].probability);
	}
}


ModelNode_NWN_Binary::ModelNode_NWN_Binary(Model &model) : ModelNode(model) {
}

ModelNode_NWN_Binary::~ModelNode_NWN_Binary() {
}

Common::UString ModelNode_NWN_Binary::loadName(Model_NWN::ParserContext &ctx) {
	size_t pos = ctx.mdl->pos();

	ctx.mdl->skip(32); // Function pointers, inherit color flag, part number

	Common::UString name = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32);

	ctx.mdl->seek(pos);

	return name;
}

void ModelNode_NWN_Binary::load(Model_NWN::ParserContext &ctx) {
	ctx.mdl->skip(24); // Function pointers

	uint32_t inheritColorFlag = ctx.mdl->readUint32LE();

	_nodeNumber = ctx.mdl->readUint32LE();

	_name = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32);

	debugC(kDebugGraphics, 5, "Node \"%s\" in state \"%s\"", _name.c_str(),
	       ctx.state->name.c_str());

	ctx.mdl->skip(8); // Parent pointers

	uint32_t childrenOffset, childrenCount;
	Model::readArrayDef(*ctx.mdl, childrenOffset, childrenCount);

	std::vector<uint32_t> children;
	Model::readArray(*ctx.mdl, ctx.offModelData + childrenOffset, childrenCount, children);

	uint32_t controllerKeyOffset, controllerKeyCount;
	Model::readArrayDef(*ctx.mdl, controllerKeyOffset, controllerKeyCount);

	uint32_t controllerDataOffset, controllerDataCount;
	Model::readArrayDef(*ctx.mdl, controllerDataOffset, controllerDataCount);

	std::vector<float> controllerData;
	Model::readArray(*ctx.mdl, ctx.offModelData + controllerDataOffset,
	                 controllerDataCount, controllerData);

	uint32_t flags = ctx.mdl->readUint32LE();

	if ((flags & 0xFFFFFC00) != 0)
		throw Common::Exception("Unknown model node flags %08X", flags);

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
		readAnim(ctx);
	}

	if (flags & kNodeFlagHasDangly) {
		// TODO: Dangly
		ctx.mdl->skip(0x18);
	}

	if (flags & kNodeFlagHasAABB) {
		// TODO: AABB
		ctx.mdl->skip(0x4);
	}

	readNodeControllers(ctx, ctx.offModelData + controllerKeyOffset,
	                    controllerKeyCount, controllerData);

	// If the node has no own position controller, inherit the position from the root state
	if (!ctx.hasPosition) {
		ModelNode *node = _model->getNode(_name);
		if (node)
			node->inheritPosition(*this);
	}

	if (!ctx.hasOrientation) {
		ModelNode *node = _model->getNode(_name);
		if (node)
			node->inheritOrientation(*this);
	}


	for (std::vector<uint32_t>::const_iterator child = children.begin(); child != children.end(); ++child) {
		ctx.mdl->seek(ctx.offModelData + *child);

		ctx.hasPosition    = false;
		ctx.hasOrientation = false;

		ModelNode_NWN_Binary *childNode = new ModelNode_NWN_Binary(*_model);
		ctx.nodes.push_back(childNode);

		childNode->setParent(this);

		checkDuplicateNode(ctx, childNode);

		childNode->load(ctx);
	}

}

void ModelNode_NWN_Binary::checkDuplicateNode(Model_NWN::ParserContext &ctx, ModelNode_NWN_Binary *newNode) {
	// Read the node's name and check if a node with that name already exists
	Common::UString name = ModelNode_NWN_Binary::loadName(ctx);

	ModelNode_NWN_Binary *oldChildNode = 0;
	for (std::list<ModelNode *>::iterator n = ctx.nodes.begin(); n != ctx.nodes.end(); ++n) {
		if ((*n)->getName().equalsIgnoreCase(name)) {
			oldChildNode = dynamic_cast<ModelNode_NWN_Binary *>(*n);
			break;
		}
	}

	// If there isn't, we're done
	if (!oldChildNode)
		return;

	// If it there, reparent its children, remove it from its parent and delete it

	warning("Duplicate node \"%s\" in state \"%s\" in model \"%s\"",
	        name.c_str(), ctx.state->name.c_str(), _model->getName().c_str());

	for (std::list<ModelNode *>::const_iterator c = oldChildNode->getChildren().begin();
	     c != oldChildNode->getChildren().end(); ++c) {

		(*c)->setParent(newNode);
	}

	if (oldChildNode->getParent())
		oldChildNode->getParent()->getChildren().remove(oldChildNode);

	ctx.nodes.remove(oldChildNode);
	delete oldChildNode;
}

struct Face {
	float normal[3];

	uint32_t smooth;

	uint16_t index[3];
};

static bool fuzzyEqual(const float *a, const float *b) {
	return fabs(a[0] - b[0]) < 1E-4 &&
	       fabs(a[1] - b[1]) < 1E-4 &&
	       fabs(a[2] - b[2]) < 1E-4;
}

void ModelNode_NWN_Binary::readMesh(Model_NWN::ParserContext &ctx) {
	ctx.mdl->skip(8); // Function pointers

	uint32_t facesOffset, facesCount;
	Model::readArrayDef(*ctx.mdl, facesOffset, facesCount);

	_mesh = new Mesh();

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

	_mesh->ambient[0] = ctx.mdl->readIEEEFloatLE();
	_mesh->ambient[1] = ctx.mdl->readIEEEFloatLE();
	_mesh->ambient[2] = ctx.mdl->readIEEEFloatLE();

	_mesh->diffuse[0] = ctx.mdl->readIEEEFloatLE();
	_mesh->diffuse[1] = ctx.mdl->readIEEEFloatLE();
	_mesh->diffuse[2] = ctx.mdl->readIEEEFloatLE();

	_mesh->specular[0] = ctx.mdl->readIEEEFloatLE();
	_mesh->specular[1] = ctx.mdl->readIEEEFloatLE();
	_mesh->specular[2] = ctx.mdl->readIEEEFloatLE();

	_mesh->shininess = ctx.mdl->readIEEEFloatLE();

	_mesh->shadow  = ctx.mdl->readUint32LE() == 1;
	_mesh->beaming = ctx.mdl->readUint32LE() == 1;
	_mesh->render  = ctx.mdl->readUint32LE() == 1;

	_mesh->hasTransparencyHint = true;
	_mesh->transparencyHint = ctx.mdl->readUint32LE() == 1;

	ctx.mdl->skip(4); // Unknown

	std::vector<Common::UString> textures;
	textures.push_back(Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 64));
	textures.push_back(Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 64));
	textures.push_back(Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 64));
	textures.push_back(Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 64));

	_mesh->tilefade = ctx.mdl->readUint32LE();

	ctx.mdl->skip(12); // Vertex indices
	ctx.mdl->skip(12); // Left over faces

	ctx.mdl->skip(12); // Vertex indices counts
	ctx.mdl->skip(12); // Vertex indices offsets

	ctx.mdl->skip(8); // Unknown

	byte triangleMode = ctx.mdl->readByte(); // 3 - Triangle, 4 - TriStrip

	ctx.mdl->skip(3 + 4); // Padding + Unknown

	uint32_t vertexOffset = ctx.mdl->readUint32LE();
	uint16_t vertexCount  = ctx.mdl->readUint16LE();
	uint16_t textureCount = ctx.mdl->readUint16LE();

	uint32_t textureVertexOffset[4];
	textureVertexOffset[0] = ctx.mdl->readUint32LE();
	textureVertexOffset[1] = ctx.mdl->readUint32LE();
	textureVertexOffset[2] = ctx.mdl->readUint32LE();
	textureVertexOffset[3] = ctx.mdl->readUint32LE();

	uint32_t normalOffset = ctx.mdl->readUint32LE(); // Vertex normals
	uint32_t colorOffset = ctx.mdl->readUint32LE(); // Vertex RGBA colors

	uint32_t textureAnimOffset[6]; // Texture animation data
	for (uint32_t i = 0; i < 6; i++)
		textureAnimOffset[i] = ctx.mdl->readUint32LE();

	bool lightMapped = ctx.mdl->readByte() == 1;

	_mesh->rotatetexture = ctx.mdl->readByte() == 1;

	ctx.mdl->skip(2); // Padding

	ctx.mdl->skip(4); // Normal sum / 2

	ctx.mdl->skip(4); // Unknown

	if ((vertexCount == 0) || (facesCount == 0) || (facesOffset == 0))
		return;

	if (textureCount > 4) {
		warning("ModelNode_NWN_Binary::readMesh(): textureCount > 4 (%d)", textureCount);
		textureCount = 4;
	}

	if ((textureCount > 0) && !ctx.texture.empty())
		textures[0] = ctx.texture;

	_render = _mesh->render;
	_mesh->data = new MeshData();
	_mesh->data->rawMesh = new Graphics::Mesh::Mesh();

	textures.resize(textureCount);
	loadTextures(textures);

	size_t endPos = ctx.mdl->pos();


	// Read vertices

	std::vector<float> vertices;
	vertices.resize(vertexCount * 3);

	assert (vertexOffset != 0xFFFFFFFF);
	ctx.mdl->seek(ctx.offRawData + vertexOffset);
	for (std::vector<float>::iterator v = vertices.begin(); v != vertices.end(); ++v)
		*v = ctx.mdl->readIEEEFloatLE();

	// Read faces

	std::vector<Face> faces;
	faces.resize(facesCount);

	std::vector< std::list<Face *> > vFaces;
	vFaces.resize(vertexCount);

	assert (facesOffset != 0xFFFFFFFF);
	ctx.mdl->seek(ctx.offModelData + facesOffset);
	for (std::vector<Face>::iterator f = faces.begin(); f != faces.end(); ++f) {
		f->normal[0] = ctx.mdl->readIEEEFloatLE();
		f->normal[1] = ctx.mdl->readIEEEFloatLE();
		f->normal[2] = ctx.mdl->readIEEEFloatLE();

		ctx.mdl->skip(4); // Plane distance

		f->smooth = ctx.mdl->readUint32LE();

		ctx.mdl->skip(3 * 2); // Adjacent face number or -1

		f->index[0] = ctx.mdl->readUint16LE();
		f->index[1] = ctx.mdl->readUint16LE();
		f->index[2] = ctx.mdl->readUint16LE();

		// Assign this face to all vertices belonging to this face
		for (int i = 0; i < 3; i++) {
			assert((size_t)(f->index[i] * 3) < vertices.size());

			const float *fV = &vertices[f->index[i] * 3];

			for (uint32_t j = 0; j < vertices.size() / 3; j++)
				if (fuzzyEqual(fV, &vertices[j * 3]))
					vFaces[j].push_back(&*f);
		}
	}

	// Read texture coordinates

	std::vector<float> texCoords;
	texCoords.resize(textureCount * vertexCount * 2);

	for (uint16_t t = 0; t < textureCount; t++) {
		const bool hasTexture = textureVertexOffset[t] != 0xFFFFFFFF;
		if (hasTexture)
			ctx.mdl->seek(ctx.offRawData + textureVertexOffset[t]);

		float *v = &texCoords[t * vertexCount * 2];
		for (uint32_t i = 0; i < vertexCount; i++) {
			*v++ = hasTexture ? ctx.mdl->readIEEEFloatLE() : 0.0f;
			*v++ = hasTexture ? ctx.mdl->readIEEEFloatLE() : 0.0f;
		}
	}

	// Create vertex buffer

	VertexDecl vertexDecl;

	vertexDecl.push_back(VertexAttrib(VPOSITION, 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VNORMAL  , 3, GL_FLOAT));
	for (uint t = 0; t < textureCount; t++)
		vertexDecl.push_back(VertexAttrib(VTCOORD + t, 2, GL_FLOAT));

	_mesh->data->rawMesh->getVertexBuffer()->setVertexDeclInterleave(facesCount * 3, vertexDecl);

	float *v = reinterpret_cast<float *>(_mesh->data->rawMesh->getVertexBuffer()->getData());
	for (uint32_t i = 0; i < facesCount; i++) {
		const Face &face = faces[i];

		for (uint32_t j = 0; j < 3; j++) {
			const uint16_t index = face.index[j];
			assert((size_t)(index * 3) < vertices.size());

			// Vertices
			*v++ = vertices[index * 3 + 0];
			*v++ = vertices[index * 3 + 1];
			*v++ = vertices[index * 3 + 2];

			// Normals, smoothed
			glm::vec3 normal(0.0f, 0.0f, 0.0f);
			uint32_t n = 0;

			for (std::list<Face *>::const_iterator vF = vFaces[index].begin(); vF != vFaces[index].end(); ++vF) {
				if (face.smooth != (*vF)->smooth)
					continue;

				normal += glm::vec3((*vF)->normal[0], (*vF)->normal[1], (*vF)->normal[2]);
				n++;
			}

			if (n > 0) {
				normal /= (float) n;
				normal = glm::normalize(normal);
			}

			*v++ = normal[0];
			*v++ = normal[1];
			*v++ = normal[2];

			// Texture coordinates
			for (uint32_t t = 0; t < textureCount; t++) {
				*v++ = texCoords[t * vertexCount * 2 + index * 2 + 0];
				*v++ = texCoords[t * vertexCount * 2 + index * 2 + 1];
			}
		}
	}

	// Create index buffer

	_mesh->data->rawMesh->getIndexBuffer()->setSize(facesCount * 3, sizeof(uint16_t), GL_UNSIGNED_SHORT);

	uint16_t *f = reinterpret_cast<uint16_t *>(_mesh->data->rawMesh->getIndexBuffer()->getData());
	for (uint16_t i = 0; i < facesCount * 3; i++)
		*f++ = i;

	createBound();

	ctx.mdl->seek(endPos);

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
		delete _mesh->data->rawMesh;
		_mesh->data->rawMesh = checkMesh;
	} else {
		_mesh->data->rawMesh->setName(meshName);
		_mesh->data->rawMesh->init();
		MeshMan.addMesh(_mesh->data->rawMesh);
	}

	if (GfxMan.isRendererExperimental())
		buildMaterial();
}

void ModelNode_NWN_Binary::readAnim(Model_NWN::ParserContext &ctx) {
	float samplePeriod = ctx.mdl->readIEEEFloatLE();

	uint32_t a0S, a0C;
	Model::readArrayDef(*ctx.mdl, a0S, a0C);

	uint32_t a1S, a1C;
	Model::readArrayDef(*ctx.mdl, a1S, a1C);

	uint32_t a2S, a2C;
	Model::readArrayDef(*ctx.mdl, a2S, a2C);

	uint32_t offAnimVertices        = ctx.mdl->readUint32LE();
	uint32_t offAnimTextureVertices = ctx.mdl->readUint32LE();

	uint32_t verticesCount        = ctx.mdl->readUint32LE();
	uint32_t textureVerticesCount = ctx.mdl->readUint32LE();
}

void ModelNode_NWN_Binary::readNodeControllers(Model_NWN::ParserContext &ctx,
		uint32_t offset, uint32_t count, std::vector<float> &data) {

	uint32_t pos = ctx.mdl->seek(offset);

	// TODO: readNodeControllers: Implement this properly :P

	for (uint32_t i = 0; i < count; i++) {
		uint32_t type        = ctx.mdl->readUint32LE();
		uint16_t rowCount    = ctx.mdl->readUint16LE();
		uint16_t timeIndex   = ctx.mdl->readUint16LE();
		uint16_t dataIndex   = ctx.mdl->readUint16LE();
		uint8_t  columnCount = ctx.mdl->readByte();
		ctx.mdl->skip(1);

		if (rowCount == 0xFFFF)
			// TODO: Controller row count = 0xFFFF
			continue;

		if (type == kControllerTypePosition) {
			if (columnCount != 3)
				throw Common::Exception("Position controller with %d values", columnCount);
			for (int r = 0; r < rowCount; r++) {
				PositionKeyFrame p;
				p.time = data[timeIndex + r];
				p.x = data[dataIndex + (r * columnCount) + 0];
				p.y = data[dataIndex + (r * columnCount) + 1];
				p.z = data[dataIndex + (r * columnCount) + 2];
				_positionFrames.push_back(p);

				// Starting position
				if (p.time == 0.0f) {
					_position[0] = p.x;
					_position[1] = p.y;
					_position[2] = p.z;
					_positionBuffer[0] = _position[0];
					_positionBuffer[1] = _position[1];
					_positionBuffer[2] = _position[2];
					ctx.hasPosition = true;
				}
			}

		} else if (type == kControllerTypeOrientation) {
			if (columnCount != 4)
				throw Common::Exception("Orientation controller with %d values", columnCount);

			for (int r = 0; r < rowCount; r++) {
				QuaternionKeyFrame q;
				q.time = data[timeIndex + r];
				q.x = data[dataIndex + (r * columnCount) + 0];
				q.y = data[dataIndex + (r * columnCount) + 1];
				q.z = data[dataIndex + (r * columnCount) + 2];
				q.q = data[dataIndex + (r * columnCount) + 3];
				_orientationFrames.push_back(q);
				// Starting orientation
				// TODO: Handle animation orientation correctly
				if (data[timeIndex + 0] == 0.0f) {
					_orientation[0] = data[dataIndex + 0];
					_orientation[1] = data[dataIndex + 1];
					_orientation[2] = data[dataIndex + 2];
					_orientation[3] = Common::rad2deg(acos(data[dataIndex + 3]) * 2.0);
					_orientationBuffer[0] = _orientation[0];
					_orientationBuffer[1] = _orientation[1];
					_orientationBuffer[2] = _orientation[2];
					_orientationBuffer[3] = _orientation[3];

					ctx.hasOrientation = true;
				}
			}

		} else if (type == kControllerTypeAlpha) {
			if (columnCount != 1)
				throw Common::Exception("Alpha controller with %d values", columnCount);

			// Starting alpha
			if (data[timeIndex + 0] == 0.0f)
				if (data[dataIndex + 0] == 0.0f)
					// TODO: Just disabled rendering if alpha == 0.0 for now
					_render = false;
		}

	}

	ctx.mdl->seek(pos);
}


ModelNode_NWN_ASCII::Mesh::Mesh() : vCount(0), tCount(0), faceCount(0) {
}


ModelNode_NWN_ASCII::ModelNode_NWN_ASCII(Model &model) : ModelNode(model) {
}

ModelNode_NWN_ASCII::~ModelNode_NWN_ASCII() {
}

void ModelNode_NWN_ASCII::load(Model_NWN::ParserContext &ctx,
                               const Common::UString &type, const Common::UString &name) {

	bool end      = false;
	bool skipNode = false;

	_name = name;

	debugC(kDebugGraphics, 5, "Node \"%s\" in state \"%s\"", _name.c_str(),
	       ctx.state->name.c_str());

	if ((type == "trimesh") || (type == "danglymesh") || (type == "skin")) {
		_mesh = new ModelNode::Mesh();
		_mesh->hasTransparencyHint = true;
		_mesh->render = true;
		if (type == "danglymesh")
			_mesh->dangly = new Dangly();
	}

	if ((type == "emitter") || (type == "reference") || (type == "aabb")) {
		// warning("TODO: Node type %s", type.c_str());
		skipNode = true;
	}

	ModelNode_NWN_ASCII::Mesh mesh;

	while (!ctx.mdl->eos()) {
		std::vector<Common::UString> line;

		size_t count = ctx.tokenize->getTokens(*ctx.mdl, line, 5);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].makeLower();

		if        (line[0] == "endnode") {
			end = true;
			break;
		} else if (skipNode) {
			continue;
		} else if (line[0] == "parent") {
			ModelNode *parent = 0;

			if (!ctx.findNode(line[1], parent))
				warning("ModelNode_NWN_ASCII::load(): Non-existent parent node \"%s\"",
				        line[1].c_str());

			setParent(parent);

		} else if (line[0] == "position") {
			readFloats(line, _position, 3, 1);
		} else if (line[0] == "orientation") {
			readFloats(line, _orientation, 4, 1);

			_orientation[3] = Common::rad2deg(_orientation[3]);
		} else if (line[0] == "render") {
			Common::parseString(line[1], _mesh->render);
		} else if (line[0] == "transparencyhint") {
			Common::parseString(line[1], _mesh->transparencyHint);
		} else if (line[0] == "danglymesh") {
		} else if (line[0] == "constraints") {
			uint32_t n;

			Common::parseString(line[1], n);
			readConstraints(ctx, n);
		} else if (line[0] == "weights") {
			uint32_t n;

			Common::parseString(line[1], n);
			readWeights(ctx, n);
		} else if (line[0] == "bitmap") {
			mesh.textures.push_back(line[1]);
		} else if (line[0] == "verts") {
			Common::parseString(line[1], mesh.vCount);

			readVCoords(ctx, mesh);
		} else if (line[0] == "tverts") {
			if (mesh.tCount != 0)
				warning("ModelNode_NWN_ASCII::load(): Multiple texture coordinates!");

			Common::parseString(line[1], mesh.tCount);

			readTCoords(ctx, mesh);
		} else if (line[0] == "faces") {
			Common::parseString(line[1], mesh.faceCount);

			readFaces(ctx, mesh);
		} else {
			// warning("Unknown MDL node command \"%s\"", line[0].c_str());
		}
	}

	if (!end)
		throw Common::Exception("ModelNode_NWN_ASCII::load(): node without endnode");

	if (!mesh.textures.empty() && !ctx.texture.empty())
		mesh.textures[0] = ctx.texture;

	processMesh(mesh);

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

	_mesh->data->rawMesh->setName(meshName);
	_mesh->data->rawMesh->init();
	if (MeshMan.getMesh(meshName)) {
		warning("Warning: probable mesh duplication of: %s", meshName.c_str());
	}
	MeshMan.addMesh(_mesh->data->rawMesh);

	if (GfxMan.isRendererExperimental())
		buildMaterial();
}

void ModelNode_NWN_ASCII::readConstraints(Model_NWN::ParserContext &ctx, uint32_t n) {
	for (uint32_t i = 0; i < n; ) {
		std::vector<Common::UString> line;

		size_t count = ctx.tokenize->getTokens(*ctx.mdl, line, 1);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		i++;
	}
}

void ModelNode_NWN_ASCII::readWeights(Model_NWN::ParserContext &ctx, uint32_t n) {
	for (uint32_t i = 0; i < n; ) {
		std::vector<Common::UString> line;

		size_t count = ctx.tokenize->getTokens(*ctx.mdl, line, 1);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		i++;
	}
}

void ModelNode_NWN_ASCII::readFloats(const std::vector<Common::UString> &strings,
                                     float *floats, uint32_t n, uint32_t start) {

	if (strings.size() < (start + n))
		throw Common::Exception("Missing tokens");

	for (uint32_t i = 0; i < n; i++)
		Common::parseString(strings[start + i], floats[i]);
}

void ModelNode_NWN_ASCII::readVCoords(Model_NWN::ParserContext &ctx, Mesh &mesh) {
	mesh.vX.resize(mesh.vCount);
	mesh.vY.resize(mesh.vCount);
	mesh.vZ.resize(mesh.vCount);

	for (uint32_t i = 0; i < mesh.vCount; ) {
		std::vector<Common::UString> line;

		size_t count = ctx.tokenize->getTokens(*ctx.mdl, line, 3);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		Common::parseString(line[0], mesh.vX[i]);
		Common::parseString(line[1], mesh.vY[i]);
		Common::parseString(line[2], mesh.vZ[i]);

		i++;
	}
}

void ModelNode_NWN_ASCII::readTCoords(Model_NWN::ParserContext &ctx, Mesh &mesh) {
	mesh.tX.resize(mesh.tCount);
	mesh.tY.resize(mesh.tCount);

	for (uint32_t i = 0; i < mesh.tCount; ) {
		std::vector<Common::UString> line;

		size_t count = ctx.tokenize->getTokens(*ctx.mdl, line, 2);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		Common::parseString(line[0], mesh.tX[i]);
		Common::parseString(line[1], mesh.tY[i]);

		i++;
	}
}

void ModelNode_NWN_ASCII::readFaces(Model_NWN::ParserContext &ctx, Mesh &mesh) {
	mesh.vIA.resize(mesh.faceCount);
	mesh.vIB.resize(mesh.faceCount);
	mesh.vIC.resize(mesh.faceCount);

	mesh.tIA.resize(mesh.faceCount);
	mesh.tIB.resize(mesh.faceCount);
	mesh.tIC.resize(mesh.faceCount);

	mesh.smooth.resize(mesh.faceCount);
	mesh.mat.resize(mesh.faceCount);

	for (uint32_t i = 0; i < mesh.faceCount; ) {
		std::vector<Common::UString> line;

		size_t count = ctx.tokenize->getTokens(*ctx.mdl, line, 8);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		Common::parseString(line[0], mesh.vIA[i]);
		Common::parseString(line[1], mesh.vIB[i]);
		Common::parseString(line[2], mesh.vIC[i]);

		Common::parseString(line[3], mesh.smooth[i]);

		Common::parseString(line[4], mesh.tIA[i]);
		Common::parseString(line[5], mesh.tIB[i]);
		Common::parseString(line[6], mesh.tIC[i]);

		Common::parseString(line[7], mesh.mat[i]);

		i++;
	}
}

typedef glm::vec3 Vec3;

struct FaceVert {
	uint32_t p, t; // position, texture coord indices
	uint32_t i;    // unique vertex id
	Vec3 n;      // normal vector
};

bool operator == (const FaceVert &a, const FaceVert &b) {
	return (a.p == b.p) && (a.t == b.t) && fuzzyEqual(&a.n[0], &b.n[0]);
}

std::size_t hash_value(const FaceVert &b) {
	std::size_t seed = 0;
	boost::hash_combine(seed, b.p);
	boost::hash_combine(seed, b.t);
	boost::hash_combine(seed, uint32_t(b.n[0] * 1E4));
	boost::hash_combine(seed, uint32_t(b.n[1] * 1E4));
	boost::hash_combine(seed, uint32_t(b.n[2] * 1E4));
	return seed;
}

void ModelNode_NWN_ASCII::processMesh(ModelNode_NWN_ASCII::Mesh &mesh) {
	if ((mesh.vCount == 0) || (mesh.tCount == 0) || (mesh.faceCount == 0))
		return;

	_render = _mesh->render;
	_mesh->data = new MeshData();
	_mesh->data->rawMesh = new Graphics::Mesh::Mesh();

	loadTextures(mesh.textures);

	const size_t textureCount = mesh.textures.size();
	if (textureCount > 1)
		warning("ModelNode_NWN_ASCII::processMesh(): textureCount == %u", (uint)textureCount);


	// Read faces

	uint32_t facesCount = mesh.faceCount;
	_mesh->data->rawMesh->getIndexBuffer()->setSize(facesCount * 3, sizeof(uint32_t), GL_UNSIGNED_INT);

	boost::unordered_set<FaceVert> verts;
	typedef boost::unordered_set<FaceVert>::iterator verts_set_it;

	uint32_t vertexCount = 0;
	uint32_t *f = reinterpret_cast<uint32_t *>(_mesh->data->rawMesh->getIndexBuffer()->getData());
	for (uint32_t i = 0; i < facesCount; i++) {
		const uint32_t v[3] = {mesh.vIA[i], mesh.vIB[i], mesh.vIC[i]};
		const uint32_t t[3] = {mesh.tIA[i], mesh.tIB[i], mesh.tIC[i]};

		// Face normal
		const Vec3 p1(mesh.vX[v[0]], mesh.vY[v[0]], mesh.vZ[v[0]]);
		const Vec3 p2(mesh.vX[v[1]], mesh.vY[v[1]], mesh.vZ[v[1]]);
		const Vec3 p3(mesh.vX[v[2]], mesh.vY[v[2]], mesh.vZ[v[2]]);
		const Vec3 n = glm::normalize(glm::cross(p2 - p1, p3 - p2));

		for (uint32_t j = 0; j < 3; j++) {
			FaceVert fv;
			fv.i = vertexCount;
			fv.p = v[j];
			fv.t = t[j];
			fv.n = n;

			std::pair<verts_set_it, bool> it = verts.insert(fv);
			if (it.second)
				vertexCount++;

			*f++ = it.first->i;
		}
	}


	// Read vertices (interleaved)

	VertexDecl vertexDecl;

	vertexDecl.push_back(VertexAttrib(VPOSITION, 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VNORMAL  , 3, GL_FLOAT));
	for (uint t = 0; t < textureCount; t++)
		vertexDecl.push_back(VertexAttrib(VTCOORD + t, 2, GL_FLOAT));

	_mesh->data->rawMesh->getVertexBuffer()->setVertexDeclInterleave(vertexCount, vertexDecl);

	for (verts_set_it i = verts.begin(); i != verts.end(); ++i) {
		byte  *vData = reinterpret_cast<byte  *>(_mesh->data->rawMesh->getVertexBuffer()->getData()) + i->i * _mesh->data->rawMesh->getVertexBuffer()->getSize();
		float *v     = reinterpret_cast<float *>(vData);

		// Position
		*v++ = mesh.vX[i->p];
		*v++ = mesh.vY[i->p];
		*v++ = mesh.vZ[i->p];

		// Normal
		*v++ = i->n[0];
		*v++ = i->n[1];
		*v++ = i->n[2];

		// TexCoord
		if (i->t < mesh.tCount) {
			*v++ = mesh.tX[i->t];
			*v++ = mesh.tY[i->t];
		} else {
			*v++ = 0.0f;
			*v++ = 0.0f;
		}
		for (uint16_t t = 1; t < textureCount; t++) {
			*v++ = 0.0f;
			*v++ = 0.0f;
		}
	}

	createBound();
}

} // End of namespace Aurora

} // End of namespace Graphics
