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

#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"

#include "src/graphics/aurora/model_kotor.h"

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
                                          const Common::UString &t, bool k2) :
	mdl(0), mdx(0), state(0), texture(t), kotor2(k2) {

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


Model_KotOR::Model_KotOR(const Common::UString &name,
                         bool kotor2, ModelType type, const Common::UString &texture) :
	Model(type) {

	_fileName = name;

	ParserContext ctx(name, texture, kotor2);

	load(ctx);

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

	uint32 nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32 nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24 + 4); // Unknown + Reference count

	uint8 type = ctx.mdl->readByte();

	ctx.mdl->skip(3 + 2); // Padding + Unknown

	uint8 classification = ctx.mdl->readByte();
	uint8 fogged         = ctx.mdl->readByte();

	ctx.mdl->skip(4); // Unknown

	ctx.mdl->skip(12); // TODO: Animation Header Pointer Array

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

	Common::UString superModelName = Common::readStringFixed(*ctx.mdl, Common::kEncodingASCII, 32);

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


ModelNode_KotOR::ModelNode_KotOR(Model &model) : ModelNode(model) {
}

ModelNode_KotOR::~ModelNode_KotOR() {
}

void ModelNode_KotOR::load(Model_KotOR::ParserContext &ctx) {
	uint16 flags      = ctx.mdl->readUint16LE();
	uint16 superNode  = ctx.mdl->readUint16LE();
	uint16 nodeNumber = ctx.mdl->readUint16LE();

	if (nodeNumber < ctx.names.size())
		_name = ctx.names[nodeNumber];

	ctx.mdl->skip(6 + 4); // Unknown + parent pointer

	_position   [0] = ctx.mdl->readIEEEFloatLE();
	_position   [1] = ctx.mdl->readIEEEFloatLE();
	_position   [2] = ctx.mdl->readIEEEFloatLE();
	_orientation[3] = Common::rad2deg(acos(ctx.mdl->readIEEEFloatLE()) * 2.0f);
	_orientation[0] = ctx.mdl->readIEEEFloatLE();
	_orientation[1] = ctx.mdl->readIEEEFloatLE();
	_orientation[2] = ctx.mdl->readIEEEFloatLE();

	uint32 childrenOffset, childrenCount;
	Model::readArrayDef(*ctx.mdl, childrenOffset, childrenCount);

	std::vector<uint32> children;
	Model::readArray(*ctx.mdl, ctx.offModelData + childrenOffset, childrenCount, children);

	uint32 controllerKeyOffset, controllerKeyCount;
	Model::readArrayDef(*ctx.mdl, controllerKeyOffset, controllerKeyCount);

	uint32 controllerDataOffset, controllerDataCount;
	Model::readArrayDef(*ctx.mdl, controllerDataOffset, controllerDataCount);

	std::vector<float> controllerData;
	Model::readArray(*ctx.mdl, ctx.offModelData + controllerDataOffset,
	                 controllerDataCount, controllerData);

	readNodeControllers(ctx, ctx.offModelData + controllerKeyOffset,
	                    controllerKeyCount, controllerData);

	if ((flags & 0xFC00) != 0)
		throw Common::Exception("Unknown node flags %04X", flags);

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
		// TODO: Anim
		ctx.mdl->skip(0x38);
	}

	if (flags & kNodeFlagHasDangly) {
		// TODO: Dangly
		ctx.mdl->skip(0x18);
	}

	if (flags & kNodeFlagHasAABB) {
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
}

void ModelNode_KotOR::readNodeControllers(Model_KotOR::ParserContext &ctx,
		uint32 offset, uint32 count, std::vector<float> &data) {

	uint32 pos = ctx.mdl->seek(offset);

	// TODO: readNodeControllers: Implement this properly :P

	for (uint32 i = 0; i < count; i++) {
		uint32 type        = ctx.mdl->readUint32LE();
		uint16 rowCount    = ctx.mdl->readUint16LE();
		uint16 timeIndex   = ctx.mdl->readUint16LE();
		uint16 dataIndex   = ctx.mdl->readUint16LE();
		uint8  columnCount = ctx.mdl->readByte();
		ctx.mdl->skip(1);

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
			_orientation[3] = data[dataIndex + 3];
		}

	}

	ctx.mdl->seek(pos);
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

	_diffuse[0] = ctx.mdl->readIEEEFloatLE();
	_diffuse[1] = ctx.mdl->readIEEEFloatLE();
	_diffuse[2] = ctx.mdl->readIEEEFloatLE();

	_ambient[0] = ctx.mdl->readIEEEFloatLE();
	_ambient[1] = ctx.mdl->readIEEEFloatLE();
	_ambient[2] = ctx.mdl->readIEEEFloatLE();

	_specular[0] = 0;
	_specular[1] = 0;
	_specular[2] = 0;

	uint32 transparencyHint = ctx.mdl->readUint32LE();

	_hasTransparencyHint = true;
	_transparencyHint    = (transparencyHint != 0);

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

	uint32 mdxStructSize = ctx.mdl->readUint32LE();

	ctx.mdl->skip(8); // Unknown

	uint32 offNormals = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4); // Unknown

	uint32 offUV[2];
	offUV[0] = ctx.mdl->readUint32LE();
	offUV[1] = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24); // Unknown

	uint16 vertexCount  = ctx.mdl->readUint16LE();
	uint16 textureCount = ctx.mdl->readUint16LE();

	ctx.mdl->skip(2);

	byte unknownFlag1 = ctx.mdl->readByte();
	_shadow  = ctx.mdl->readByte() == 1;
	byte unknownFlag2 = ctx.mdl->readByte();
	_render  = ctx.mdl->readByte() == 1;

	ctx.mdl->skip(10);

	if (ctx.kotor2)
		ctx.mdl->skip(8);

	uint32 offNodeData = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4);

	if ((offOffVertsCount < 1) || (vertexCount == 0) || (facesCount == 0))
		return;

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
	for (uint t = 0; t < textureCount; t++)
		vertexDecl.push_back(VertexAttrib(VTCOORD + t , 2, GL_FLOAT));

	_vertexBuffer.setVertexDeclInterleave(vertexCount, vertexDecl);

	float *v = (float *)_vertexBuffer.getData();
	for (uint32 i = 0; i < vertexCount; i++) {
		// Position
		ctx.mdx->seek(offNodeData + i * mdxStructSize);
		*v++ = ctx.mdx->readIEEEFloatLE();
		*v++ = ctx.mdx->readIEEEFloatLE();
		*v++ = ctx.mdx->readIEEEFloatLE();

		// Normal
		//ctx.mdx->seek(offNodeData + i * mdxStructSize + offNormals);
		*v++ = ctx.mdx->readIEEEFloatLE();
		*v++ = ctx.mdx->readIEEEFloatLE();
		*v++ = ctx.mdx->readIEEEFloatLE();

		// TexCoords
		for (uint16 t = 0; t < textureCount; t++) {
			if (offUV[t] != 0xFFFFFFFF) {
				ctx.mdx->seek(offNodeData + i * mdxStructSize + offUV[t]);
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

	_indexBuffer.setSize(facesCount * 3, sizeof(uint16), GL_UNSIGNED_SHORT);

	uint16 *f = (uint16 *) _indexBuffer.getData();
	for (uint32 i = 0; i < facesCount * 3; i++)
		f[i] = ctx.mdl->readUint16LE();

	createBound();

	ctx.mdl->seek(endPos);
}

} // End of namespace Aurora

} // End of namespace Graphics
