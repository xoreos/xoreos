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
 *  Loading Nintendo's NSBMD files found in Sonic
 */

/* Based on several bits and pieces figured out by the Nintendo DS
 * modding community. Most prominently:
 * - The NSBMD reader found in the NDS file viewer and editor Tinke
 *   by pleoNeX (<https://github.com/pleonex/tinke>), which is
 *   under the terms of the GPLv3.
 * - lowlines' NSBMD documentation
 *   (<http://llref.emutalk.net/docs/?file=xml/bmd0.xml#xml-doc>)
 * - Output from lowlines' Console Tool
 *   (<http://llref.emutalk.net/projects/ctool/>)
 * - The Nintendo DS technical information GBATEK by Martin Korth
 *   (<http://problemkaputt.de/gbatek.htm>)
 *
 * The original copyright note in Tinke reads as follows:
 *
 * Copyright (C) 2011  pleoNeX
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <cstring>

#include "external/glm/gtc/type_ptr.hpp"
#include "external/glm/gtc/matrix_transform.hpp"
#include "external/glm/gtx/matrix_interpolation.hpp"

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/strutil.h"
#include "src/common/readstream.h"
#include "src/common/error.h"
#include "src/common/encoding.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"

#include "src/graphics/aurora/model_sonic.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

static const uint32_t kBMD0ID = MKTAG('B', 'M', 'D', '0');
static const uint32_t kMDL0ID = MKTAG('M', 'D', 'L', '0');

namespace Graphics {

namespace Aurora {

Model_Sonic::ParserContext::ParserContext(const Common::UString &name) : nsbmd(0), state(0) {
	Common::SeekableReadStream *stream = ResMan.getResource(name, ::Aurora::kFileTypeNSBMD);
	if (!stream)
		throw Common::Exception("No such NSBMD \"%s\"", name.c_str());

	nsbmd = ::Aurora::NitroFile::open(stream);
}

Model_Sonic::ParserContext::~ParserContext() {
	delete nsbmd;

	clear();
}

void Model_Sonic::ParserContext::clear() {
	for (std::list<ModelNode_Sonic *>::iterator n = nodes.begin(); n != nodes.end(); ++n)
		delete *n;
	nodes.clear();

	delete state;
	state = 0;
}


Model_Sonic::Model_Sonic(const Common::UString &name, ModelType type) : Model(type) {
	_fileName = name;

	ParserContext ctx(name);

	load(ctx);

	finalize();

	// Model::createBound() doesn't know about the geometry in Model_Sonic
	createBound();
}

Model_Sonic::~Model_Sonic() {
}

// --- Loader ---

void Model_Sonic::load(ParserContext &ctx) {
	// Read the model data from the file

	readHeader(ctx);
	readModel(ctx);

	// Parse/process the model parts further

	parseBoneCommands(ctx);

	findRootBones(ctx);
	findStackBones(ctx);

	// Create our run-time data

	createModelNodes(ctx);

	findStackMixes(ctx);

	createGeometry(ctx);
	evaluateGeometry();
}

void Model_Sonic::readModel(ParserContext &ctx) {
	readModelHeader(ctx);

	ctx.nsbmd->seek(ctx.offsetModel);

	// Read the model globals

	ctx.nsbmd->skip(4); // Section size

	ctx.offsetBoneCommands = ctx.nsbmd->readUint32() + ctx.offsetModel;
	ctx.offsetMaterials    = ctx.nsbmd->readUint32() + ctx.offsetModel;
	ctx.offsetPolygons     = ctx.nsbmd->readUint32() + ctx.offsetModel;

	ctx.nsbmd->skip(4); // End of the polygons section
	ctx.nsbmd->skip(8); // Unknown

	ctx.defaultScale = readNintendoFixedPoint(ctx.nsbmd->readUint32(), true, 19, 12);

	ctx.nsbmd->skip( 4); // Bound box scaling
	ctx.nsbmd->skip( 8); // Unknown
	ctx.nsbmd->skip(12); // Bounding box. We calculate our own
	ctx.nsbmd->skip( 8); // Unknown

	ctx.offsetBones = ctx.nsbmd->pos();

	// Read the model parts

	readBones(ctx);
	readBoneCommands(ctx);

	readMaterials(ctx);

	readPolygons(ctx);
}

// --- Loader utility methods ---

uint8_t Model_Sonic::readInfoOffset(ParserContext &ctx, Infos &infos, uint32_t offset) {
	/* Contains names and offsets of entries in a section. */

	ctx.nsbmd->skip(1); // Unknown

	const uint8_t count = ctx.nsbmd->readByte();
	infos.resize(count);

	ctx.nsbmd->skip(2); // Section size
	ctx.nsbmd->skip(2 + 2 + 4 + count * (2 + 2)); // Unknown

	ctx.nsbmd->skip(2 + 2); // Header size + section size

	for (uint i = 0; i < count; i++) {
		infos[i].offset = ctx.nsbmd->readUint32() + offset;
		infos[i].count  = 0;
	}

	for (uint i = 0; i < count; i++)
		infos[i].name = Common::readStringFixed(*ctx.nsbmd, Common::kEncodingASCII, 16).toLower();

	return count;
}

uint8_t Model_Sonic::readInfoOffsetCount(ParserContext &ctx, Infos &infos, uint32_t offset) {
	/* Contains names, offsets and counts/sizes of entries in a section. */

	ctx.nsbmd->skip(1); // Unknown

	const uint8_t count = ctx.nsbmd->readByte();
	infos.resize(count);

	ctx.nsbmd->skip(2); // Section size
	ctx.nsbmd->skip(2 + 2 + 4 + count * (2 + 2)); // Unknown

	ctx.nsbmd->skip(2 + 2); // Header size + section size

	for (uint i = 0; i < count; i++) {
		infos[i].offset = ctx.nsbmd->readUint16() + offset;
		infos[i].count  = ctx.nsbmd->readByte();

		ctx.nsbmd->skip(1); // Padding
	}

	for (uint i = 0; i < count; i++)
		infos[i].name = Common::readStringFixed(*ctx.nsbmd, Common::kEncodingASCII, 16).toLower();

	return count;
}

// --- Headers ---

void Model_Sonic::readHeader(ParserContext &ctx) {
	/* Global NSBMD header. */

	const uint32_t tag = ctx.nsbmd->readUint32BE();
	if (tag != kBMD0ID)
		throw Common::Exception("Invalid NSBMD file (%s)", Common::debugTag(tag).c_str());

	const uint16_t bom = ctx.nsbmd->readUint16();
	if (bom != 0xFEFF)
		throw Common::Exception("Invalid BOM: %u", bom);

	const uint8_t versionMajor = ctx.nsbmd->readByte();
	const uint8_t versionMinor = ctx.nsbmd->readByte();
	if ((versionMajor != 2) || (versionMinor != 0))
		throw Common::Exception("Unsupported version %u.%u", versionMajor, versionMinor);

	const uint32_t fileSize = ctx.nsbmd->readUint32();
	if (fileSize > ctx.nsbmd->size())
		throw Common::Exception("Size too large (%u > %u)", fileSize, (uint)ctx.nsbmd->size());

	const uint16_t headerSize = ctx.nsbmd->readUint16();
	if (headerSize != 16)
		throw Common::Exception("Invalid header size (%u)", headerSize);

	const uint16_t sectionCount = ctx.nsbmd->readUint16();
	if (sectionCount != 1)
		throw Common::Exception("Invalid number of sections (%u)", sectionCount);

	ctx.offsetMDL0 = ctx.nsbmd->readUint32();
}

void Model_Sonic::readModelHeader(ParserContext &ctx) {
	/* MDL0 section header. */

	ctx.nsbmd->seek(ctx.offsetMDL0);

	const uint32_t tag = ctx.nsbmd->readUint32BE();
	if (tag != kMDL0ID)
		throw Common::Exception("Invalid model section (%s)", Common::debugTag(tag).c_str());

	ctx.nsbmd->skip(4); // Size

	Infos models;
	const uint8_t modelCount = readInfoOffset(ctx, models, ctx.offsetMDL0);

	if (modelCount != 1)
		throw Common::Exception("Unsupported number of models (%u)", modelCount);

	_name = models[0].name;

	ctx.offsetModel = models[0].offset;
}

// --- Bones ---

void Model_Sonic::readBones(ParserContext &ctx) {
	ctx.nsbmd->seek(ctx.offsetBones);

	Infos bones;
	const uint8_t boneCount = readInfoOffset(ctx, bones, ctx.offsetBones);

	ctx.bones.resize(boneCount);
	for (uint i = 0; i < boneCount; i++)
		readBone(ctx, ctx.bones[i], bones[i]);
}

void Model_Sonic::readBone(ParserContext &ctx, Bone &bone, Info &info) {
	ctx.nsbmd->seek(info.offset);

	bone.name = info.name;

	bone.transform = glm::mat4();


	const uint16_t flags = ctx.nsbmd->readUint16();

	const double rotate0 = readNintendoFixedPoint(ctx.nsbmd->readUint16(), true, 3, 12);

	const bool hasTranslate =  (flags & 0x01) == 0;
	const bool hasRotate    = ((flags & 0x02) == 0) && !((flags & 0x08) != 0);
	const bool hasScale     =  (flags & 0x04) == 0;
	const bool hasPivot     =  (flags & 0x08) != 0;

	const uint8_t pivotSelect = (flags >> 4) & 0xF;
	const uint8_t pivotNegate = (flags >> 8) & 0xF;

	// TRS: translate, rotate/pivot, scale

	if (hasTranslate) {
		const float x = readNintendoFixedPoint(ctx.nsbmd->readUint32(), true, 19, 12);
		const float y = readNintendoFixedPoint(ctx.nsbmd->readUint32(), true, 19, 12);
		const float z = readNintendoFixedPoint(ctx.nsbmd->readUint32(), true, 19, 12);

		bone.transform = glm::translate(bone.transform, glm::vec3(x, y, z));
	}

	if (hasPivot) {
		const float pivotA = readNintendoFixedPoint(ctx.nsbmd->readUint16(), true, 3, 12);
		const float pivotB = readNintendoFixedPoint(ctx.nsbmd->readUint16(), true, 3, 12);

		const glm::mat4 pivot = createPivot(pivotA, pivotB, pivotSelect, pivotNegate);

		bone.transform *= pivot;
	}

	if (hasRotate) {
		glm::mat4 rotateMatrix;

		rotateMatrix[0][0] = rotate0;
		rotateMatrix[0][1] = readNintendoFixedPoint(ctx.nsbmd->readUint16(), true, 3, 12);
		rotateMatrix[0][2] = readNintendoFixedPoint(ctx.nsbmd->readUint16(), true, 3, 12);

		rotateMatrix[1][0] = readNintendoFixedPoint(ctx.nsbmd->readUint16(), true, 3, 12);
		rotateMatrix[1][1] = readNintendoFixedPoint(ctx.nsbmd->readUint16(), true, 3, 12);
		rotateMatrix[1][2] = readNintendoFixedPoint(ctx.nsbmd->readUint16(), true, 3, 12);

		rotateMatrix[2][0] = readNintendoFixedPoint(ctx.nsbmd->readUint16(), true, 3, 12);
		rotateMatrix[2][1] = readNintendoFixedPoint(ctx.nsbmd->readUint16(), true, 3, 12);
		rotateMatrix[2][2] = readNintendoFixedPoint(ctx.nsbmd->readUint16(), true, 3, 12);

		bone.transform *= rotateMatrix;
	}

	if (hasScale) {
		const float x = readNintendoFixedPoint(ctx.nsbmd->readUint32(), true, 19, 12);
		const float y = readNintendoFixedPoint(ctx.nsbmd->readUint32(), true, 19, 12);
		const float z = readNintendoFixedPoint(ctx.nsbmd->readUint32(), true, 19, 12);

		bone.transform = glm::scale(bone.transform, glm::vec3(x, y, z));
	}
}

void Model_Sonic::readBoneCommands(ParserContext &ctx) {
	ctx.nsbmd->seek(ctx.offsetBoneCommands);

	BoneCommandID cmd;
	while ((cmd = (BoneCommandID) ctx.nsbmd->readByte()) != kBoneEnd) {
		ctx.boneCommands.push_back(BoneCommand(cmd));
		BoneCommand &boneCommand = ctx.boneCommands.back();

		uint8_t count = 0;
		if (cmd == kBoneLoadStack) {
			size_t pos = ctx.nsbmd->pos();

			ctx.nsbmd->skip(1);
			count = ctx.nsbmd->readByte();

			ctx.nsbmd->seek(pos);
		}

		const uint8_t paramCount = getBoneParameterCount(cmd, count);

		boneCommand.parameters.resize(paramCount);
		for (uint i = 0; i < paramCount; i++)
			boneCommand.parameters[i] = ctx.nsbmd->readByte();
	}
}

// --- Materials ---

void Model_Sonic::readMaterials(ParserContext &ctx) {
	ctx.nsbmd->seek(ctx.offsetMaterials);

	ctx.offsetTextures = ctx.nsbmd->readUint16() + ctx.offsetMaterials;
	ctx.offsetPalettes = ctx.nsbmd->readUint16() + ctx.offsetMaterials;

	readMaterialDefinitions(ctx);

	// Attach the texture and palette names to the materials
	// Note: we don't actually care about the palette name

	ctx.nsbmd->seek(ctx.offsetTextures);
	readMaterialResource(ctx, 0);

	ctx.nsbmd->seek(ctx.offsetPalettes);
	readMaterialResource(ctx, 1);
}

void Model_Sonic::readMaterialDefinitions(ParserContext &ctx) {
	Infos materials;
	const uint8_t materialCount = readInfoOffset(ctx, materials, ctx.offsetMaterials);

	ctx.materials.resize(materialCount);
	for (uint i = 0; i < materialCount; i++)
		readMaterialDefinition(ctx, ctx.materials[i], materials[i]);
}

void Model_Sonic::readMaterialDefinition(ParserContext &ctx, Material &material, Info &info) {
	ctx.nsbmd->seek(info.offset);

	material.name = info.name;

	ctx.nsbmd->skip(2 + 2 + 18); // Unknown + section size + unknown

	const uint16_t flags = ctx.nsbmd->readUint16();

	// TODO: Are the different from the ones inside the NSBTX?
	// TODO: Actually use those parameters

	material.wrapX = (flags & 0x0001) != 0;
	material.wrapY = (flags & 0x0002) != 0;
	material.flipX = (flags & 0x0004) != 0;
	material.flipY = (flags & 0x0008) != 0;

	const bool doubleX = (flags & 0x1000) != 0;
	const bool doubleY = (flags & 0x4000) != 0;

	material.scaleX = 1 << (doubleX ? 1 : 0);
	material.scaleY = 1 << (doubleY ? 1 : 0);

	ctx.nsbmd->skip(8); // Unknown

	material.width  = ctx.nsbmd->readUint16();
	material.height = ctx.nsbmd->readUint16();
}

void Model_Sonic::readMaterialResource(ParserContext &ctx, uint textureOrPalette) {
	/* Each texture/palette has a list of materials it belongs to. */

	Infos resources;
	const uint8_t resCount = readInfoOffsetCount(ctx, resources, ctx.offsetMaterials);

	for (uint i = 0; i < resCount; i++) {
		ctx.nsbmd->seek(resources[i].offset);

		for (uint j = 0; j < resources[i].count; j++) {
			const uint8_t materialID = ctx.nsbmd->readByte();
			if (materialID >= ctx.materials.size())
				continue;

			if      (textureOrPalette == 0)
				ctx.materials[materialID].texture = resources[i].name;
			else if (textureOrPalette == 1)
				ctx.materials[materialID].palette = resources[i].name;
		}
	}
}

// --- Polygons ---

void Model_Sonic::readPolygons(ParserContext &ctx) {
	ctx.nsbmd->seek(ctx.offsetPolygons);

	Infos polygons;
	const uint8_t polygonCount = readInfoOffset(ctx, polygons, ctx.offsetPolygons);

	ctx.polygons.resize(polygonCount);
	for (uint i = 0; i < polygonCount; i++)
		readPolygon(ctx, ctx.polygons[i], polygons[i]);
}

void Model_Sonic::readPolygon(ParserContext &ctx, Polygon &polygon, Info &info) {
	ctx.nsbmd->seek(info.offset);

	polygon.name = info.name;

	ctx.nsbmd->skip(8); // Unknown

	const uint32_t listOffset = ctx.nsbmd->readUint32() + info.offset;
	const uint32_t listSize   = ctx.nsbmd->readUint32();

	ctx.nsbmd->seek(listOffset);

	readPolygonCommands(ctx, polygon, listSize);
}

void Model_Sonic::readPolygonCommands(ParserContext &ctx, Polygon &polygon, uint32_t listSize) {
	/* Read the commands for a polygon.
	*
	 * For some reason (alignment?), the layout is as follows:
	 * - 4 command IDs (uint8_t each)
	 * - parameters for each of those 4 command IDs (uint32_t each)
	 * - 4 command IDs
	 * - [...]
	 *
	 * While we're reading the list, we also count the number of primitives
	 * that will be created and how long they'll be.
	 */

	uint8_t buffer[4];

	uint32_t primitiveSize = 0;

	polygon.commands.reserve(listSize / 4);
	while (listSize >= 4) {
		ctx.nsbmd->read(buffer, 4);
		listSize -= 4;

		for (uint i = 0; i < 4; i++) {
			polygon.commands.push_back(PolygonCommand((PolygonCommandID) buffer[i]));
			PolygonCommand &cmd = polygon.commands.back();

			const uint8_t paramCount = getPolygonParameterCount(cmd.command);
			cmd.parameters.resize(paramCount);

			for (uint j = 0; (j < paramCount) && (listSize >= 4); j++, listSize -= 4)
				cmd.parameters[j] = ctx.nsbmd->readUint32();

			if ((cmd.command >= kPolygonVertex16) && (cmd.command <= kPolygonVertexDiff))
				primitiveSize++;

			if (cmd.command == kPolygonBeginVertices) {
				polygon.primitiveCount++;

				polygon.primitiveSize = MAX(polygon.primitiveSize, primitiveSize);
				primitiveSize = 0;
			}
		}
	}

	polygon.primitiveSize = MAX(polygon.primitiveSize, primitiveSize);
}

// --- Processing the model parts ---

void Model_Sonic::parseBoneCommands(ParserContext &ctx) {
	/* Go through and evaluate all bone commands. */

	uint16_t polygon      = 0xFFFF;
	uint16_t polygonStack = 0xFFFF;
	uint16_t material     = 0xFFFF;

	uint16_t nodeStack   = 0;
	uint16_t parentStack = 0xFFFF;

	uint16_t nodeID   = 0xFFFF;
	uint16_t parentID = 0xFFFF;

	Bone *node   = 0;
	Bone *parent = 0;


	for (BoneCommands::const_iterator c = ctx.boneCommands.begin(); c != ctx.boneCommands.end(); ++c) {
		switch (c->command) {
			case kBoneNOP:
			case kBoneEnd:
			case kBoneBeginPair:
			case kBoneEndPair:
			case kBoneUnknown1:
			case kBoneUnknown2:
			case kBoneUnknown3:
				break;

			case kBoneSetInvisible:
				ctx.boneInvisible[c->parameters[0]] = c->parameters[1] == 1;
				break;

			case kBoneSetPolygonStack:
				polygonStack = c->parameters[0];
				break;

			case kBoneLoadStack:
				polygonStack = c->parameters[0];

				ctx.stackMix[polygonStack] = StackMixes();
				ctx.stackMix[polygonStack].resize(c->parameters[1]);

				for (uint32_t i = 0; i < c->parameters[1]; i++) {
					ctx.stackMix[polygonStack][i].nodeID    = c->parameters[2 + i * 3 + 0];
					ctx.stackMix[polygonStack][i].nodeStack = c->parameters[2 + i * 3 + 1];
					ctx.stackMix[polygonStack][i].ratio     = c->parameters[2 + i * 3 + 2] / 256.0f;
				}

				warning("TODO: Bone command LoadStack: %u (\"%s\")", c->parameters[0], _name.c_str());
				break;

			case kBoneSetPolygon:
				polygon = c->parameters[0];
				if (polygon >= ctx.polygons.size())
					break;

				if (material < ctx.materials.size())
					ctx.polygons[polygon].material = &ctx.materials[material];

				ctx.polygons[polygon].defaultStack = polygonStack;
				break;

			case kBoneSetMaterial1:
			case kBoneSetMaterial2:
			case kBoneSetMaterial3:
				material = c->parameters[0];
				break;

			case kBoneConnect1:
			case kBoneConnect2:
			case kBoneConnect3:
			case kBoneConnect4:
				nodeID   = c->parameters[0];
				parentID = c->parameters[1];

				node   = (nodeID   < ctx.bones.size()) ? &ctx.bones[nodeID]   : 0;
				parent = (parentID < ctx.bones.size()) ? &ctx.bones[parentID] : 0;

				if (!node) {
					warning("No such node %u in bone command connect 0x%02X (\"%s\")",
					        c->parameters[0], c->parameters[1], _name.c_str());
					break;
				}

				if (node == parent)
					parent = 0;

				parentStack = parent ? parent->nodeStack : 0xFFFF;

				switch (c->command) {
					case kBoneConnect1:
						nodeStack++;
						break;

					case kBoneConnect2:
						nodeStack   = c->parameters[3];
						break;

					case kBoneConnect3:
						parentStack = c->parameters[3];
						break;

					case kBoneConnect4:
						nodeStack   = c->parameters[3];
						parentStack = c->parameters[4];
						break;

					default:
						break;
				}

				polygonStack = nodeStack;

				// If this ever happens, the node doesn't inherit its matrix from its parent
				if (parent && (parentStack != parent->nodeStack))
					warning("Parent stack != node parent stack (%u:%u, %u:%u) (\"%s\")",
					        parentID, parent->nodeStack, nodeID, parentStack, _name.c_str());

				node->nodeID      = nodeID;
				node->parentID    = parentID;
				node->nodeStack   = nodeStack;
				node->parentStack = parentStack;
				node->parent      = parent;

				if (parent)
					parent->children.push_back(node);

				break;

			default:
				throw Common::Exception("Invalid bone command: 0x%02X", (uint) c->command);
		}
	}
}

void Model_Sonic::findRootBones(ParserContext &ctx) {
	/* Collect all bones without a parent in our root bone list. */

	for (Bones::iterator j = ctx.bones.begin(); j != ctx.bones.end(); ++j)
		if (!j->parent)
			ctx.rootBones.push_back(&*j);

	// There *should* only be one
	if (ctx.rootBones.size() != 1)
		throw Common::Exception("Invalid number of root bones (%u)", (uint) ctx.rootBones.size());
}

void Model_Sonic::findStackBones(ParserContext &ctx) {
	/* Collect the stack positions for each of our bone. */

	for (Bones::iterator j = ctx.bones.begin(); j != ctx.bones.end(); ++j) {
		if (j->nodeStack == 0xFFFF)
			continue;

		std::pair<StackBoneMap::iterator, bool> result;
		result = ctx.stackBones.insert(std::make_pair(j->nodeStack, &*j));

		if (!result.second)
			warning("Stack collision: %u for node %u and %u (\"%s\")", j->nodeStack,
			        result.first->second->nodeID, j->nodeID, _name.c_str());
	}
}

// --- Create our run-time data ---

void Model_Sonic::createModelNodes(ParserContext &ctx) {
	/* Recursively add the model nodes according to how the bones connect. */

	newState(ctx);

	ModelNode_Sonic *rootNode = new ModelNode_Sonic(*this);
	ctx.nodes.push_back(rootNode);

	rootNode->load(ctx, *ctx.rootBones.front());
	rootNode->createAbsoluteBound();

	addState(ctx);

	// Evaluate the map of invisible bones
	for (BoneInvisible::const_iterator v = ctx.boneInvisible.begin(); v != ctx.boneInvisible.end(); ++v) {
		StackBoneMap::iterator b = ctx.stackBones.find(v->first);
		if (b != ctx.stackBones.end())
			b->second->modelNode->setInvisible(v->second);
	}
}

void Model_Sonic::findStackMixes(ParserContext &ctx) {
	/* Go through all the stack mixes and fill in the nodes corresponding to the stack positions. */

	for (StackMixMap::iterator s = ctx.stackMix.begin(); s != ctx.stackMix.end(); ++s) {
		for (StackMixes::iterator m = s->second.begin(); m != s->second.end(); ++m) {
			StackBoneMap::iterator b = ctx.stackBones.find(m->nodeStack);
			if (b == ctx.stackBones.end())
				continue;

			m->node = b->second->modelNode;
		}
	}
}

void Model_Sonic::createGeometry(ParserContext &ctx) {
	_geometries.resize(ctx.polygons.size());
	for (size_t i = 0; i < ctx.polygons.size(); i++) {
		Polygon  &polygon  = ctx.polygons[i];
		Geometry &geometry = _geometries[i];

		// Load texture
		try {
			if (polygon.material && !polygon.material->texture.empty())
				geometry.texture = TextureMan.get(polygon.material->texture);
		} catch (...) {
			Common::exceptionDispatcherWarning();
		}

		// Create the primitives and their vertices
		createPrimitives(ctx, geometry, polygon);

		// And fill in the indices
		for (Primitives::iterator p = geometry.primitives.begin(); p != geometry.primitives.end(); ++p)
			createIndices(*p);
	}
}

void Model_Sonic::createPrimitives(ParserContext &ctx, Geometry &geometry, Polygon &polygon) {
	/* Go through all polygon commands in this polygon and evaluate them, creating
	 * an intermediate Primitive for each vertex segment between BeginVertices
	 * and EndVertices commands. */

	// We already counted the number of primitives while reading the command list
	geometry.primitives.reserve(polygon.primitiveCount);

	// Running vertex buffer

	PrimitiveVertex vertex;

	bool hasVertex = false;
	Primitive *primitive = 0;

	StackMixMap::const_iterator stackMix = ctx.stackMix.end();

	// Default values

	StackBoneMap::const_iterator stackBone = ctx.stackBones.find(polygon.defaultStack);
	ModelNode_Sonic *defaultNode = (stackBone != ctx.stackBones.end()) ? stackBone->second->modelNode : 0;

	if (defaultNode)
		vertex.nodes.push_back(PrimitiveNode(defaultNode, 1.0f));

	glm::vec3 primScale(ctx.defaultScale, ctx.defaultScale, ctx.defaultScale);

	// Texture dimensions, to convert the texture coordinates to OpenGL notation

	const double tWidth  = !geometry.texture.empty() ? geometry.texture.getTexture().getWidth()  : 1.0f;
	const double tHeight = !geometry.texture.empty() ? geometry.texture.getTexture().getHeight() : 1.0f;


	for (PolygonCommands::const_iterator c = polygon.commands.begin(); c != polygon.commands.end(); ++c) {
		switch (c->command) {
			case kPolygonNOP:
				break;

			case kPolygonBeginVertices:
				geometry.primitives.push_back(Primitive((PrimitiveType) c->parameters[0]));
				primitive = &geometry.primitives.back();

				// We calculated the maximum length of a primitive earlier
				primitive->vertices.reserve(polygon.primitiveSize);
				break;

			case kPolygonEndVertices:
				// We don't strictly need to observe the EndVertices command.
				// In fact, according to some people, the Nintendo DS itself doesn't either.
				break;


			case kPolygonColor:
				// BGR555
				vertex.color[0] = ( c->parameters[0]        & 0x1F) / 31.0f;
				vertex.color[1] = ((c->parameters[0] >>  5) & 0x1F) / 31.0f;
				vertex.color[2] = ((c->parameters[0] >> 10) & 0x1F) / 31.0f;
				break;

			case kPolygonTexCoord:
				// One unit ^= one texel!
				vertex.texCoord[0] = readNintendoFixedPoint( c->parameters[0]        & 0xFFFF, true, 11, 4) / tWidth;
				vertex.texCoord[1] = readNintendoFixedPoint((c->parameters[0] >> 16) & 0xFFFF, true, 11, 4) / tHeight;
				break;

			case kPolygonNormal:
				vertex.normal[0] = readNintendoFixedPoint( c->parameters[0]        & 0x03FF, true, 0, 9);
				vertex.normal[1] = readNintendoFixedPoint((c->parameters[0] >> 10) & 0x03FF, true, 0, 9);
				vertex.normal[2] = readNintendoFixedPoint((c->parameters[0] >> 20) & 0x03FF, true, 0, 9);
				break;


			case kPolygonVertex16:
				vertex.vertex[0] = readNintendoFixedPoint( c->parameters[0]        & 0xFFFF, true, 3, 12);
				vertex.vertex[1] = readNintendoFixedPoint((c->parameters[0] >> 16) & 0xFFFF, true, 3, 12);
				vertex.vertex[2] = readNintendoFixedPoint( c->parameters[1]        & 0xFFFF, true, 3, 12);

				hasVertex = true;
				break;

			case kPolygonVertex10:
				vertex.vertex[0] = readNintendoFixedPoint( c->parameters[0]        & 0x03FF, true, 3, 6);
				vertex.vertex[1] = readNintendoFixedPoint((c->parameters[0] >> 10) & 0x03FF, true, 3, 6);
				vertex.vertex[2] = readNintendoFixedPoint((c->parameters[0] >> 20) & 0x03FF, true, 3, 6);

				hasVertex = true;
				break;

			case kPolygonVertexXY:
				vertex.vertex[0] = readNintendoFixedPoint( c->parameters[0]        & 0xFFFF, true, 3, 12);
				vertex.vertex[1] = readNintendoFixedPoint((c->parameters[0] >> 16) & 0xFFFF, true, 3, 12);

				hasVertex = true;
				break;

			case kPolygonVertexXZ:
				vertex.vertex[0] = readNintendoFixedPoint( c->parameters[0]        & 0xFFFF, true, 3, 12);
				vertex.vertex[2] = readNintendoFixedPoint((c->parameters[0] >> 16) & 0xFFFF, true, 3, 12);

				hasVertex = true;
				break;

			case kPolygonVertexYZ:
				vertex.vertex[1] = readNintendoFixedPoint( c->parameters[0]        & 0xFFFF, true, 3, 12);
				vertex.vertex[2] = readNintendoFixedPoint((c->parameters[0] >> 16) & 0xFFFF, true, 3, 12);

				hasVertex = true;
				break;

			case kPolygonVertexDiff:
				vertex.vertex[0] += readNintendoFixedPoint( c->parameters[0]        & 0x03FF, true, 0, 9) / 8;
				vertex.vertex[1] += readNintendoFixedPoint((c->parameters[0] >> 10) & 0x03FF, true, 0, 9) / 8;
				vertex.vertex[2] += readNintendoFixedPoint((c->parameters[0] >> 20) & 0x03FF, true, 0, 9) / 8;

				hasVertex = true;
				break;


			case kPolygonMatrixRestore:
				if ((stackMix = ctx.stackMix.find(c->parameters[0])) != ctx.stackMix.end()) {
					// Was this stack position filled with kBoneLoadStack?

					vertex.nodes.clear();
					vertex.nodes.reserve(stackMix->second.size());
					for (StackMixes::const_iterator m = stackMix->second.begin(); m != stackMix->second.end(); ++m)
						vertex.nodes.push_back(PrimitiveNode(*m));
				} else if ((stackBone = ctx.stackBones.find(c->parameters[0])) != ctx.stackBones.end()) {
					// Is this a regular bone position?

					vertex.nodes.clear();
					vertex.nodes.push_back(PrimitiveNode(stackBone->second->modelNode, 1.0f));
				}

				// Reset the scale
				primScale[0] = 1.0f;
				primScale[1] = 1.0f;
				primScale[2] = 1.0f;
				break;

			case kPolygonMatrixScale:
				// The NDS probably directly scales the current matrix?
				primScale[0] *= readNintendoFixedPoint(c->parameters[0], true, 19, 12);
				primScale[1] *= readNintendoFixedPoint(c->parameters[1], true, 19, 12);
				primScale[2] *= readNintendoFixedPoint(c->parameters[2], true, 19, 12);
				break;

			// Commands we hopefully won't need
			case kPolygonMatrixMode:
			case kPolygonMatrixPush:
			case kPolygonMatrixPop:
			case kPolygonMatrixStore:
			case kPolygonMatrixIdentity:
			case kPolygonMatrixLoad4x4:
			case kPolygonMatrixLoad4x3:
			case kPolygonMatrixMult4x4:
			case kPolygonMatrixMult4x3:
			case kPolygonMatrixMult3x3:
			case kPolygonMatrixTranslate:
			case kPolygonPolygonAttrib:
			case kPolygonTexImageParam:
			case kPolygonPaletteBase:
			case kPolygonDiffuseAmbient:
			case kPolygonSpecularEmit:
			case kPolygonLightVector:
			case kPolygonLightColor:
			case kPolygonShininess:
			case kPolygonSwapBuffers:
			case kPolygonViewport:
			case kPolygonBoxTest:
			case kPolygonPosTest:
			case kPolygonVecTest:
				throw Common::Exception("Unsupported polygon command: 0x%02X", (uint) c->command);

			default:
				throw Common::Exception("Invalid polygon command: 0x%02X", (uint) c->command);
		}

		if (hasVertex && primitive) {
			/* TODO: We're disabling primitives with mixed stack positions here.
			 *       To support this, we need a way to properly average several
			 *       matrices in evaluatePrimitive(). */
			if (vertex.nodes.size() > 1)
				primitive->invalid = true;

			primitive->vertices.push_back(vertex);
			primitive->vertices.back().vertex *= primScale;

			hasVertex = false;
		}
	}

}

void Model_Sonic::createIndices(Primitive &primitive) {
	/* Create index lists for the vertices, according to the type. */

	switch (primitive.type) {
		case kPrimitiveTypeTriangles:
			createIndicesTriangles(primitive);
			return;

		case kPrimitiveTypeQuads:
			createIndicesQuads(primitive);
			break;

		case kPrimitiveTypeTriangleStrip:
			createIndicesTriangleStrip(primitive);
			break;

		case kPrimitiveTypeQuadStrip:
			createIndicesQuadStrip(primitive);
			break;

		default:
			throw Common::Exception("Invalid primitive type %u", (uint) primitive.type);
	}
}

void Model_Sonic::createIndicesTriangles(Primitive &primitive) {
	/* Just plain old triangles. */

	primitive.indices.resize(primitive.vertices.size());

	for (size_t i = 0; i < primitive.vertices.size(); i++)
		primitive.indices[i] = i;
}

void Model_Sonic::createIndicesTriangleStrip(Primitive &primitive) {
	/* Triangle strip. Create indices to unfold the strip into a triangle list. */

	if (primitive.vertices.size() < 3)
		return;

	primitive.indices.reserve((primitive.vertices.size() - 2) * 3);

	for (size_t i = 0; i < primitive.vertices.size() - 2; i++) {
		if (i & 1) {
			primitive.indices.push_back(i);
			primitive.indices.push_back(i + 2);
			primitive.indices.push_back(i + 1);
		} else {
			primitive.indices.push_back(i);
			primitive.indices.push_back(i + 1);
			primitive.indices.push_back(i + 2);
		}
	}
}

void Model_Sonic::createIndicesQuads(Primitive &primitive) {
	/* Quads. Create indices to unfold the quads into a triangle list. */

	if (primitive.vertices.size() < 4)
		return;

	primitive.indices.reserve((primitive.vertices.size() / 4) * 6);

	for (size_t i = 0; i < primitive.vertices.size() - 3; i += 4) {
		primitive.indices.push_back(i);
		primitive.indices.push_back(i + 1);
		primitive.indices.push_back(i + 2);

		primitive.indices.push_back(i + 2);
		primitive.indices.push_back(i + 3);
		primitive.indices.push_back(i);
	}
}

void Model_Sonic::createIndicesQuadStrip(Primitive &primitive) {
	/* Quad strip. Create indices to unfold the quads into a triangle list. */

	if (primitive.vertices.size() < 4)
		return;

	primitive.indices.reserve(((primitive.vertices.size() - 2) / 2) * 6);

	for (size_t i = 0; i < primitive.vertices.size() - 2; i += 2) {
		primitive.indices.push_back(i);
		primitive.indices.push_back(i + 1);
		primitive.indices.push_back(i + 2);

		primitive.indices.push_back(i + 1);
		primitive.indices.push_back(i + 3);
		primitive.indices.push_back(i + 2);
	}
}

void Model_Sonic::createBound() {
	/* Manually recreate the bounding box by inspecting the vertex buffers. */

	_boundBox.clear();

	for (Geometries::const_iterator g = _geometries.begin(); g != _geometries.end(); ++g) {
		for (Primitives::const_iterator p = g->primitives.begin(); p != g->primitives.end(); ++p) {
			if (p->vertexBuffer.getVertexDecl().empty())
				continue;

			const VertexAttrib &vpos = p->vertexBuffer.getVertexDecl()[0];
			assert(vpos.index == VPOSITION);
			assert(vpos.type == GL_FLOAT);

			uint32_t stride = MAX<uint32_t>(vpos.size, vpos.stride / sizeof(float));

			const float *vertexData = reinterpret_cast<const float *>(vpos.pointer);

			const float *vX = vertexData + 0;
			const float *vY = vertexData + 1;
			const float *vZ = vertexData + 2;

			for (uint32_t v = 0; v < p->vertexBuffer.getCount(); v++)
				_boundBox.add(vX[v * stride], vY[v * stride], vZ[v * stride]);
		}
	}

	float minX, minY, minZ, maxX, maxY, maxZ;
	_boundBox.getMin(minX, minY, minZ);
	_boundBox.getMax(maxX, maxY, maxZ);

	_center[0] = minX + ((maxX - minX) / 2.0f);
	_center[1] = minY + ((maxY - minY) / 2.0f);
	_center[2] = minZ + ((maxZ - minZ) / 2.0f);

	_absoluteBoundBox = _boundBox;
	_absoluteBoundBox.transform(_absolutePosition);
	_absoluteBoundBox.absolutize();
}

void Model_Sonic::newState(ParserContext &ctx) {
	ctx.clear();

	ctx.state = new State;
}

void Model_Sonic::addState(ParserContext &ctx) {
	if (!ctx.state || ctx.nodes.empty()) {
		ctx.clear();
		return;
	}

	for (std::list<ModelNode_Sonic *>::iterator n = ctx.nodes.begin(); n != ctx.nodes.end(); ++n) {
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

// --- Run-time methods ---

void Model_Sonic::evaluateGeometry() {
	for (Geometries::iterator g = _geometries.begin(); g != _geometries.end(); ++g)
		for (Primitives::iterator p = g->primitives.begin(); p != g->primitives.end(); ++p)
			evaluatePrimitive(*p);
}

void Model_Sonic::evaluatePrimitive(Primitive &primitive) {
	/* Create the actual IBO and VBO structures. */

	if (primitive.invalid || primitive.indices.empty() || primitive.vertices.empty())
		return;

	// Create the index buffer

	primitive.indexBuffer.setSize(primitive.indices.size(), sizeof(uint16_t), GL_UNSIGNED_SHORT);

	uint16_t *indices = reinterpret_cast<uint16_t *>(primitive.indexBuffer.getData());
	memcpy(indices, &primitive.indices[0], primitive.indices.size() * sizeof(uint16_t));

	// Create vertex buffer

	VertexDecl vertexDecl;

	vertexDecl.push_back(VertexAttrib(VPOSITION, 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VNORMAL  , 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VCOLOR   , 4, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VTCOORD  , 2, GL_FLOAT));

	primitive.vertexBuffer.setVertexDeclInterleave(primitive.vertices.size(), vertexDecl);

	float *vData = reinterpret_cast<float *>(primitive.vertexBuffer.getData());
	for (PrimitiveVertices::const_iterator v = primitive.vertices.begin(); v != primitive.vertices.end(); ++v) {
		/* To get the absolute position of the vertex, transform it by the absolute
		 * position of its base node. Use an identity matrix as a fallback. */

		// TODO: For some primitives, we need to calculate the weighted average of several matrices
		glm::mat4 matrix;
		if (!v->nodes.empty() && v->nodes[0].node)
			matrix = v->nodes[0].node->getAbsolutePosition();

		const glm::vec4 pos = matrix * glm::vec4(v->vertex.x, v->vertex.y, v->vertex.z, 1);

		*vData++ = pos[0];
		*vData++ = pos[1];
		*vData++ = pos[2];

		*vData++ = v->normal[0];
		*vData++ = v->normal[1];
		*vData++ = v->normal[2];

		*vData++ = v->color[0];
		*vData++ = v->color[1];
		*vData++ = v->color[2];
		*vData++ = v->color[3];

		*vData++ = v->texCoord[0];
		*vData++ = v->texCoord[1];
	}

}

// --- Utility methods ---

uint8_t Model_Sonic::getBoneParameterCount(BoneCommandID cmd, uint8_t count) {
	switch (cmd) {
		case kBoneNOP:             return 0;
		case kBoneEnd:             return 0;
		case kBoneSetInvisible:    return 2;
		case kBoneSetPolygonStack: return 1;
		case kBoneSetMaterial1:    return 1;
		case kBoneSetPolygon:      return 1;
		case kBoneConnect1:        return 3;
		case kBoneUnknown1:        return 1;
		case kBoneLoadStack:       return count * 3 + 2;
		case kBoneBeginPair:       return 0;
		case kBoneUnknown2:        return 2;
		case kBoneSetMaterial2:    return 1;
		case kBoneConnect2:        return 4;
		case kBoneEndPair:         return 0;
		case kBoneUnknown3:        return 6;
		case kBoneSetMaterial3:    return 1;
		case kBoneConnect3:        return 4;
		case kBoneConnect4:        return 5;

		default:
			throw Common::Exception("Invalid bone command: 0x%02X", (uint) cmd);
	}
}

uint8_t Model_Sonic::getPolygonParameterCount(PolygonCommandID cmd) {
	switch (cmd) {
		case kPolygonNOP:             return  0;

		// Matrix commands
		case kPolygonMatrixMode:      return  1;
		case kPolygonMatrixPush:      return  0;
		case kPolygonMatrixPop:       return  1;
		case kPolygonMatrixStore:     return  1;
		case kPolygonMatrixRestore:   return  1;
		case kPolygonMatrixIdentity:  return  0;
		case kPolygonMatrixLoad4x4:   return 16;
		case kPolygonMatrixLoad4x3:   return 12;
		case kPolygonMatrixMult4x4:   return 16;
		case kPolygonMatrixMult4x3:   return 12;
		case kPolygonMatrixMult3x3:   return  9;
		case kPolygonMatrixScale:     return  3;
		case kPolygonMatrixTranslate: return  3;

		// Vertex attributes
		case kPolygonColor:           return  1;
		case kPolygonNormal:          return  1;
		case kPolygonTexCoord:        return  1;

		// Vertex coordinates
		case kPolygonVertex16:        return  2;
		case kPolygonVertex10:        return  1;
		case kPolygonVertexXY:        return  1;
		case kPolygonVertexXZ:        return  1;
		case kPolygonVertexYZ:        return  1;
		case kPolygonVertexDiff:      return  1;

		case kPolygonPolygonAttrib:   return  1;
		case kPolygonTexImageParam:   return  1;

		case kPolygonPaletteBase:     return  1;

		// Lighting
		case kPolygonDiffuseAmbient:  return  1;
		case kPolygonSpecularEmit:    return  1;
		case kPolygonLightVector:     return  1;
		case kPolygonLightColor:      return  1;
		case kPolygonShininess:       return  32;

		case kPolygonBeginVertices:   return  1;
		case kPolygonEndVertices:     return  0;

		case kPolygonSwapBuffers:     return  1;

		case kPolygonViewport:        return  1;

		case kPolygonBoxTest:         return  3;
		case kPolygonPosTest:         return  2;
		case kPolygonVecTest:         return  1;

		default:
			throw Common::Exception("Invalid polygon command: 0x%02X", (uint) cmd);
	}
}

glm::mat4 Model_Sonic::createPivot(double a, double b, uint8_t select, uint8_t negate) {
	float pivot[16] = {
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	float one = 1.0f;
	float a2  = a;
	float b2  = b;

	if ((negate ==  1) || (negate ==  3) || (negate ==  5) || (negate ==  7) ||
	    (negate ==  9) || (negate == 11) || (negate == 13) || (negate == 15))
		one = -one;

	if ((negate ==  2) || (negate ==  3) || (negate ==  6) || (negate ==  7) ||
	    (negate == 10) || (negate == 11) || (negate == 14) || (negate == 15))
		b2 = -b2;

	if ((negate ==  4) || (negate ==  5) || (negate ==  6) || (negate ==  7) ||
	    (negate == 12) || (negate == 13) || (negate == 14) || (negate == 15))
		a2 = -a2;

	switch (select) {
		case 0:
			pivot[ 0] = one; pivot[5] = a; pivot[6] = b; pivot[9] = b2; pivot[10] = a2; break;
		case 1:
			pivot[ 1] = one; pivot[4] = a; pivot[6] = b; pivot[8] = b2; pivot[10] = a2; break;
		case 2:
			pivot[ 2] = one; pivot[4] = a; pivot[5] = b; pivot[8] = b2; pivot[ 9] = a2; break;
		case 3:
			pivot[ 4] = one; pivot[1] = a; pivot[2] = b; pivot[9] = b2; pivot[10] = a2; break;
		case 4:
			pivot[ 5] = one; pivot[0] = a; pivot[2] = b; pivot[8] = b2; pivot[10] = a2; break;
		case 5:
			pivot[ 6] = one; pivot[0] = a; pivot[1] = b; pivot[8] = b2; pivot[ 9] = a2; break;
		case 6:
			pivot[ 8] = one; pivot[1] = a; pivot[2] = b; pivot[5] = b2; pivot[ 6] = a2; break;
		case 7:
			pivot[ 9] = one; pivot[0] = a; pivot[2] = b; pivot[4] = b2; pivot[ 6] = a2; break;
		case 8:
			pivot[10] = one; pivot[0] = a; pivot[1] = b; pivot[4] = b2; pivot[ 5] = a2; break;
		case 9:
			pivot[ 0] = -a;  break;

		default:
			break;
	}

	return glm::make_mat4(pivot);
}

void Model_Sonic::render(RenderPass pass) {
	/* We're overriding Model::render() here, because Model_Sonic keeps the geometry,
	 * while in other Model classes, the geometry is inside the ModelNodes.
	 *
	 * TODO: Find a way to merge this back into Model?
	 */

	if (!_currentState || (pass > kRenderPassAll))
		return;

	if (pass == kRenderPassAll) {
		Model_Sonic::render(kRenderPassOpaque);
		Model_Sonic::render(kRenderPassTransparent);
		return;
	}

	// Apply our global model transformation
	glTranslatef(_position[0], _position[1], _position[2]);
	glRotatef(_orientation[3], _orientation[0], _orientation[1], _orientation[2]);
	glScalef(_scale[0], _scale[1], _scale[2]);

	// Draw the bounding box, if requested
	doDrawBound();

	TextureMan.reset();

	for (Geometries::const_iterator g = _geometries.begin(); g != _geometries.end(); ++g) {
		TextureMan.set(g->texture);

		for (Primitives::const_iterator p = g->primitives.begin(); p != g->primitives.end(); ++p)
			p->vertexBuffer.draw(GL_TRIANGLES, p->indexBuffer);
	}

	TextureMan.reset();

	// Draw the skeleton, if requested
	doDrawSkeleton();
}


ModelNode_Sonic::ModelNode_Sonic(Model &model) : ModelNode(model) {
}

ModelNode_Sonic::~ModelNode_Sonic() {
}

void ModelNode_Sonic::load(Model_Sonic::ParserContext &ctx, Model_Sonic::Bone &bone) {
	bone.modelNode = this;

	_name      = bone.name;
	_render    = true;

	// Decompose the bone's transformation matrix into TRS

	const glm::mat4 &m = bone.transform;

	_position[0] = m[3][0];
	_position[1] = m[3][1];
	_position[2] = m[3][2];

	glm::vec3 axis;
	glm::axisAngle(m, axis, _orientation[3]);
	_orientation[3] = Common::rad2deg(_orientation[3]);
	_orientation[0] = axis.x;
	_orientation[1] = axis.y;
	_orientation[2] = axis.z;

	_scale[0] = sqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
	_scale[1] = sqrtf(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);
	_scale[2] = sqrtf(m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]);

	if (bone.parent)
		setParent(bone.parent->modelNode);

	for (std::list<Model_Sonic::Bone *>::iterator c = bone.children.begin(); c != bone.children.end(); ++c) {
		ModelNode_Sonic *childNode = new ModelNode_Sonic(*_model);
		ctx.nodes.push_back(childNode);

		childNode->load(ctx, **c);
	}
}

void ModelNode_Sonic::createAbsoluteBound() {
	ModelNode::createAbsoluteBound();
}

} // End of namespace Aurora

} // End of namespace Graphics
