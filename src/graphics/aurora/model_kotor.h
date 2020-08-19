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

#ifndef GRAPHICS_AURORA_MODEL_KOTOR_H
#define GRAPHICS_AURORA_MODEL_KOTOR_H

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class ModelNode_KotOR;

/** A 3D model in the NWN binary MDL format. */
class Model_KotOR : public Model {
public:
	Model_KotOR(const Common::UString &name, bool kotor2, bool xbox, ModelType type = kModelTypeObject,
	            const Common::UString &texture = "", ModelCache *modelCache = 0);
	~Model_KotOR();

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;
		Common::SeekableReadStream *mdx;

		Common::UString mdlName;

		State *state;

		std::list<ModelNode_KotOR *> nodes;

		Common::UString texture;
		std::vector<Common::UString> textures;

		bool kotor2;
		bool xbox;

		uint32_t offModelData;
		uint32_t offRawData;

		std::vector<Common::UString> names;

		uint32_t mdxStructSize;
		uint16_t vertexCount;
		uint16_t textureCount;
		uint32_t offNodeData;
		uint32_t offVertsCoords;
		uint16_t flags;

		ParserContext(const Common::UString &name, const Common::UString &t, bool k2, bool x);
		~ParserContext();

		void clear();
	};


	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void load(ParserContext &ctx);
	bool readAnim(ParserContext &ctx, uint32_t offset);

	void loadSuperModel(ModelCache *modelCache, bool kotor2, bool xbox);

	void readStrings(Common::SeekableReadStream &mdl,
			const std::vector<uint32_t> &offsets, uint32_t offset,
			std::vector<Common::UString> &strings);

	/** Map bone indices to model nodes for optimization. */
	void fillBoneNodeMap();
	/** Certain head models in KotOR 2 contain nodes that are children to a bone rather than a root node. */
	void reparentHeadNodes();

	friend class ModelNode_KotOR;
};

class ModelNode_KotOR : public ModelNode {
public:
	ModelNode_KotOR(Model &model);
	~ModelNode_KotOR();

	void load(Model_KotOR::ParserContext &ctx);

	void buildMaterial();

	void declareShaderInputs(MaterialConfiguration &config, Shader::ShaderDescriptor &cripter);
	void setupShaderTexture(MaterialConfiguration &config, int textureIndex, Shader::ShaderDescriptor &cripter);

private:
	void readNodeControllers(Model_KotOR::ParserContext &ctx, uint32_t offset,
	                         uint32_t count, std::vector<float> &dataFloat, std::vector<uint32_t> &dataInt);
	void readPositionController(uint8_t columnCount, uint16_t rowCount, uint16_t timeIndex,
	                            uint16_t dataIndex, std::vector<float> &data);
	void readOrientationController(uint8_t columnCount, uint16_t rowCount, uint16_t timeIndex,
	                               uint16_t dataIndex, std::vector<float> &dataFloat, std::vector<uint32_t> &dataInt);
	void readMesh(Model_KotOR::ParserContext &ctx);
	void readSkin(Model_KotOR::ParserContext &ctx);
	void readSaber(Model_KotOR::ParserContext &ctx);
	void readEmitter(Model_KotOR::ParserContext &ctx);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_KOTOR_H
