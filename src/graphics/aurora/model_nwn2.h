/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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

/** @file graphics/aurora/model_nwn2.h
 *  Loading MDB files found in Neverwinter Nights 2
 */

#ifndef GRAPHICS_AURORA_NEWMODEL_NWN2_H
#define GRAPHICS_AURORA_NEWMODEL_NWN2_H

#include "graphics/aurora/model.h"
#include "graphics/aurora/modelnode.h"

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

private:
	struct PacketKey {
		uint32 signature;
		uint32 offset;
	};

	struct ParserContext {
		Common::SeekableReadStream *mdb;

		State *state;

		std::list<ModelNode_NWN2 *> nodes;

		uint32 offModelData;
		uint32 offRawData;

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
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_NEWMODEL_NWN2_H
