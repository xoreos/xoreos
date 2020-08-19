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

#ifndef GRAPHICS_AURORA_MODEL_WITCHER_H
#define GRAPHICS_AURORA_MODEL_WITCHER_H

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class ModelNode_Witcher;

/** A 3D model in the The Witcher MDB format. */
class Model_Witcher : public Model {
public:
	Model_Witcher(const Common::UString &name, ModelType type = kModelTypeObject);
	~Model_Witcher();

private:
	struct ParserContext {
		Common::SeekableReadStream *mdb;

		Common::UString mdlName;

		State *state;

		std::list<ModelNode_Witcher *> nodes;

		uint16_t fileVersion;

		uint32_t offModelData;
		uint32_t modelDataSize;

		uint32_t offRawData;
		uint32_t rawDataSize;

		uint32_t offTextureInfo;

		uint32_t offTexData;
		uint32_t texDatasize;

		ParserContext(const Common::UString &name);
		~ParserContext();

		void clear();
	};


	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void load(ParserContext &ctx);

	friend class ModelNode_Witcher;
};

class ModelNode_Witcher : public ModelNode {
public:
	ModelNode_Witcher(Model &model);
	~ModelNode_Witcher();

	void load(Model_Witcher::ParserContext &ctx);

private:
	struct TexturePaintLayer {
		bool hasTexture;

		Common::UString texture;

		std::vector<float> weights;
	};

	void readMesh(Model_Witcher::ParserContext &ctx);
	void readTexturePaint(Model_Witcher::ParserContext &ctx);

	void readTextures(Model_Witcher::ParserContext &ctx,
	                  std::vector<Common::UString> &textures);
	void readNodeControllers(Model_Witcher::ParserContext &ctx,
	                         uint32_t offset, uint32_t count, std::vector<float> &data);

	void evaluateTextures(int n, std::vector<Common::UString> &textures,
	                      const Common::UString *staticTextures, const uint32_t *tVertsCount,
	                      bool lightMapDayNight, const Common::UString &lightMapName);

protected:
	void buildMaterial();
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_WITCHER_H
