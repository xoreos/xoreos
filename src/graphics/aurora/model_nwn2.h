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
 *  Loading MDB files found in Neverwinter Nights 2
 */

#ifndef GRAPHICS_AURORA_MODEL_NWN2_H
#define GRAPHICS_AURORA_MODEL_NWN2_H

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class ModelNode_NWN2;

/** A 3D model in the NWN2 MDB format. */
class Model_NWN2 : public Model {
public:
	Model_NWN2(const Common::UString &name, ModelType type = kModelTypeObject);
	~Model_NWN2();

	/** Tint the whole model with these tint colors. */
	void setTint(const float tint[3][4]);

	/** Tint all floor nodes of the model with these tint colors. */
	void setTintFloor(const float tint[3][4]);
	/** Tint all wall nodes of the model with these tint colors. */
	void setTintWalls(const float tint[3][4]);

private:
	struct PacketKey {
		uint32_t signature;
		uint32_t offset;
	};

	struct ParserContext {
		Common::SeekableReadStream *mdb;

		Common::UString mdlName;

		State *state;

		std::list<ModelNode_NWN2 *> nodes;

		uint32_t offModelData;
		uint32_t offRawData;

		ParserContext(const Common::UString &name);
		~ParserContext();

		void clear();
	};


	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void load(ParserContext &ctx);

	friend class ModelNode_NWN2;
};

class ModelNode_NWN2 : public ModelNode {
public:
	ModelNode_NWN2(Model &model);
	~ModelNode_NWN2();

	bool loadRigid(Model_NWN2::ParserContext &ctx);
	bool loadSkin (Model_NWN2::ParserContext &ctx);

	void setTint(const float tint[3][4]);

private:
	Common::UString _tintMap;

	int _tintedMapIndex;

	float _tint[3][4];

	void removeTint();
	void createTint();
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_NWN2_H
