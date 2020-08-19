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
 *  Loading MDL/MDX files found in Jade Empire
 */

#ifndef GRAPHICS_AURORA_MODEL_JADE_H
#define GRAPHICS_AURORA_MODEL_JADE_H

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class ModelNode_Jade;

/** A 3D model in the Jade Empire MDL/MDX format. */
class Model_Jade : public Model {
public:
	Model_Jade(const Common::UString &name, ModelType type = kModelTypeObject,
	           const Common::UString &texture = "");
	~Model_Jade();

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;
		Common::SeekableReadStream *mdx;

		Common::UString mdlName;

		State *state;

		std::list<ModelNode_Jade *> nodes;

		Common::UString texture;

		uint32_t offModelData;

		uint32_t mdlSize;

		uint32_t mdxSizeVertices;
		uint32_t mdxSizeFaces;
		uint32_t mdxSize3;

		std::vector<Common::UString> names;

		std::vector<float>  vertices;
		std::vector<uint16_t> indices;

		std::vector<Common::UString> textures;

		std::vector<std::vector<float> > texCoords;

		ParserContext(const Common::UString &name, const Common::UString &t);
		~ParserContext();

		void clear();
		void newNode();
	};


	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void load(ParserContext &ctx);

	void readStrings(Common::SeekableReadStream &mdl,
			const std::vector<uint32_t> &offsets, uint32_t offset,
			std::vector<Common::UString> &strings);

	friend class ModelNode_Jade;
};

class ModelNode_Jade : public ModelNode {
public:
	ModelNode_Jade(Model &model);
	~ModelNode_Jade();

	void load(Model_Jade::ParserContext &ctx);

	void buildMaterial();

private:

	struct JadeMaterialData {
		uint32_t renderPathID;
		uint32_t opacity1;
		uint32_t opacity2;
		float cubeMultiplier;
		float bumpCoordMultiplier;
		float terrainCoordMultiplier;
		float falloff;
		float waterAlpha;
		uint8_t bumpMapIsSpecular;
		uint8_t doubleSided;
		// 2 bytes unknown (padding?)
		float diffuseColor[3];
		float ambientColor[3];
		// 24 bytes unknown.
		uint32_t blending1;
		uint32_t blending2;
		// 4 bytes unknown.
	};

	void readMesh(Model_Jade::ParserContext &ctx);

	void readPlainIndices  (Common::SeekableReadStream &stream, std::vector<uint16_t> &indices,
	                        uint32_t offset, uint32_t count);
	void readChunkedIndices(Common::SeekableReadStream &stream, std::vector<uint16_t> &indices,
	                        uint32_t offset, uint32_t count);

	void unfoldFaces        (std::vector<uint16_t> &indices, uint32_t meshType);
	void unfoldTriangleStrip(std::vector<uint16_t> &indices);
	void unfoldTriangleFan  (std::vector<uint16_t> &indices);

	void createMesh(Model_Jade::ParserContext &ctx);

	void readMaterialTextures(uint32_t materialID, std::vector<Common::UString> &textures);

	JadeMaterialData _jadeMaterialData;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_JADE_H
