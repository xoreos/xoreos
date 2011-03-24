/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model_nwn_binary.h
 *  Loading binary MDL files found in Neverwinter Nights.
 */

#ifndef GRAPHICS_AURORA_NEWMODEL_NWN_BINARY_H
#define GRAPHICS_AURORA_NEWMODEL_NWN_BINARY_H

#include "graphics/aurora/model.h"
#include "graphics/aurora/modelnode.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class ModelNode_NWN_Binary;

/** A 3D model in the NWN binary MDL format. */
class Model_NWN_Binary : public Model {
public:
	Model_NWN_Binary(Common::SeekableReadStream &mdl, ModelType type = kModelTypeObject);
	~Model_NWN_Binary();

	static bool isBinary(Common::SeekableReadStream &mdl);

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;

		State *state;

		ModelNode_NWN_Binary *rootNode;
		std::list<ModelNode_NWN_Binary *> nodes;

		uint32 offModelData;
		uint32 offRawData;

		bool hasPosition;
		bool hasOrientation;

		ParserContext(Common::SeekableReadStream &stream);
		~ParserContext();

		void clear();
	};


	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void load(ParserContext &ctx);

	void readAnim(ParserContext &ctx, uint32 offset);

	friend class ModelNode_NWN_Binary;
};

class ModelNode_NWN_Binary : public ModelNode {
public:
	ModelNode_NWN_Binary(Model &model);
	~ModelNode_NWN_Binary();

	void load(Model_NWN_Binary::ParserContext &ctx);

private:
	void readMesh(Model_NWN_Binary::ParserContext &ctx);
	void readAnim(Model_NWN_Binary::ParserContext &ctx);

	void readNodeControllers(Model_NWN_Binary::ParserContext &ctx, uint32 offset,
                           uint32 count, std::vector<float> &data);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_NEWMODEL_NWN_BINARY_H
