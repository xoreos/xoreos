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

#ifndef GRAPHICS_AURORA_MODEL_NWN_H
#define GRAPHICS_AURORA_MODEL_NWN_H

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"

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
	          const Common::UString &texture = "", ModelCache *modelCache = 0);
	~Model_NWN();

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;

		Common::UString mdlName;

		State *state;

		bool isASCII;

		std::list<ModelNode *> nodes;

		Common::UString texture;

		uint32_t offModelData;
		uint32_t offRawData;

		bool hasPosition;
		bool hasOrientation;

		Common::StreamTokenizer *tokenize;
		std::vector<uint32_t> anims;

		ParserContext(const Common::UString &name, const Common::UString &t);
		~ParserContext();

		bool findNode(const Common::UString &name, ModelNode *&node) const;

		void clear();
	};


	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void loadBinary(ParserContext &ctx);
	void readAnimBinary(ParserContext &ctx, uint32_t offset);

	void loadASCII(ParserContext &ctx);
	void readAnimASCII(ParserContext &ctx);
	void skipAnimASCII(ParserContext &ctx);

	void loadSuperModel(ModelCache *modelCache);

	void populateDefaultAnimations();

	friend class ModelNode_NWN_Binary;
	friend class ModelNode_NWN_ASCII;
};

class ModelNode_NWN_Binary : public ModelNode {
public:
	ModelNode_NWN_Binary(Model &model);
	~ModelNode_NWN_Binary();

	void load(Model_NWN::ParserContext &ctx);

	static Common::UString loadName(Model_NWN::ParserContext &ctx);

private:
	void checkDuplicateNode(Model_NWN::ParserContext &ctx, ModelNode_NWN_Binary *newNode);

	void readMesh(Model_NWN::ParserContext &ctx);
	void readAnim(Model_NWN::ParserContext &ctx);

	void readNodeControllers(Model_NWN::ParserContext &ctx, uint32_t offset,
                           uint32_t count, std::vector<float> &data);
};

class ModelNode_NWN_ASCII : public ModelNode {
public:
	ModelNode_NWN_ASCII(Model &model);
	~ModelNode_NWN_ASCII();

	void load(Model_NWN::ParserContext &ctx,
	          const Common::UString &type, const Common::UString &name);

private:
	struct Mesh {
		uint32_t vCount;
		uint32_t tCount;
		uint32_t faceCount;

		std::vector<Common::UString> textures;

		std::vector<float> vX, vY, vZ;
		std::vector<float> tX, tY;

		std::vector<uint32_t> vIA, vIB, vIC;
		std::vector<uint32_t> tIA, tIB, tIC;

		std::vector<uint32_t> smooth, mat;

		Mesh();
	};

	void readConstraints(Model_NWN::ParserContext &ctx, uint32_t n);
	void readWeights(Model_NWN::ParserContext &ctx, uint32_t n);

	void readFloats(const std::vector<Common::UString> &strings,
	                float *floats, uint32_t n, uint32_t start);

	void readVCoords(Model_NWN::ParserContext &ctx, Mesh &mesh);
	void readTCoords(Model_NWN::ParserContext &ctx, Mesh &mesh);

	void readFaces(Model_NWN::ParserContext &ctx, Mesh &mesh);

	void processMesh(ModelNode_NWN_ASCII::Mesh &mesh);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_NWN_BINARY_H
