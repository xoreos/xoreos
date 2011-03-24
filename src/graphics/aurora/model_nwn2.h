/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
	Model_NWN2(Common::SeekableReadStream &mdb, ModelType type = kModelTypeObject);
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

		ParserContext(Common::SeekableReadStream &stream);
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
