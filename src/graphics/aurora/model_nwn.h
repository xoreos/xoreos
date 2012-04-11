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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/aurora/model_nwn.h
 *  Loading MDL files found in Neverwinter Nights.
 */

#ifndef GRAPHICS_AURORA_NEWMODEL_NWN_H
#define GRAPHICS_AURORA_NEWMODEL_NWN_H

#include "graphics/aurora/model.h"
#include "graphics/aurora/modelnode.h"

namespace Common {
	class SeekableReadStream;
	class StreamTokenizer;
}

namespace Graphics {

namespace Aurora {

class ModelNode_NWN_Binary;
class ModelNode_NWN_ASCII;

/** A 3D model in the NWN MDL format. */
class Model_NWN : public Model {
public:
	Model_NWN(const Common::UString &name, ModelType type = kModelTypeObject,
	          const Common::UString &texture = "", std::map<Common::UString, Model*, Common::UString::iless> *modelCache = 0);
	~Model_NWN();

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;

		State *state;

		bool isASCII;

		std::list<ModelNode *> nodes;

		Common::UString texture;

		uint32 offModelData;
		uint32 offRawData;

		bool hasPosition;
		bool hasOrientation;

		Common::StreamTokenizer *tokenize;
		std::vector<uint32> anims;

		ParserContext(const Common::UString &name, const Common::UString &t);
		~ParserContext();

		bool findNode(const Common::UString &name, ModelNode *&node) const;

		void clear();
	};


	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void loadBinary(ParserContext &ctx);
	void readAnimBinary(ParserContext &ctx, uint32 offset);

	void loadASCII(ParserContext &ctx);
	void readAnimASCII(ParserContext &ctx);
	void skipAnimASCII(ParserContext &ctx);

	friend class ModelNode_NWN_Binary;
	friend class ModelNode_NWN_ASCII;
};

class ModelNode_NWN_Binary : public ModelNode {
public:
	ModelNode_NWN_Binary(Model &model);
	~ModelNode_NWN_Binary();

	void load(Model_NWN::ParserContext &ctx);

private:
	void readMesh(Model_NWN::ParserContext &ctx);
	void readAnim(Model_NWN::ParserContext &ctx);

	void readNodeControllers(Model_NWN::ParserContext &ctx, uint32 offset,
                           uint32 count, std::vector<float> &data);
};

class ModelNode_NWN_ASCII : public ModelNode {
public:
	ModelNode_NWN_ASCII(Model &model);
	~ModelNode_NWN_ASCII();

	void load(Model_NWN::ParserContext &ctx,
	          const Common::UString &type, const Common::UString &name);

private:
	struct Mesh {
		uint32 vCount;
		uint32 tCount;
		uint32 faceCount;

		std::vector<Common::UString> textures;

		std::vector<float> vX, vY, vZ;
		std::vector<float> tX, tY;

		std::vector<uint32> vIA, vIB, vIC;
		std::vector<uint32> tIA, tIB, tIC;

		std::vector<uint32> smooth, mat;

		Mesh();
	};

	void readConstraints(Model_NWN::ParserContext &ctx, uint32 n);
	void readWeights(Model_NWN::ParserContext &ctx, uint32 n);

	void readFloats(const std::vector<Common::UString> &strings,
	                float *floats, uint32 n, uint32 start);

	void readVCoords(Model_NWN::ParserContext &ctx, Mesh &mesh);
	void readTCoords(Model_NWN::ParserContext &ctx, Mesh &mesh);

	void readFaces(Model_NWN::ParserContext &ctx, Mesh &mesh);

	void processMesh(Mesh &mesh);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_NEWMODEL_NWN_BINARY_H
