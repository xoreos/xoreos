/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model_witcher.h
 *  Loading MDB files found in The Witcher
 */

#ifndef GRAPHICS_AURORA_NEWMODEL_WITCHER_H
#define GRAPHICS_AURORA_NEWMODEL_WITCHER_H

#include "graphics/aurora/model.h"
#include "graphics/aurora/modelnode.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class ModelNode_Witcher;

/** A 3D model in the The Witcher MDB format. */
class Model_Witcher : public Model {
public:
	Model_Witcher(Common::SeekableReadStream &mdb, ModelType type = kModelTypeObject);
	~Model_Witcher();

private:
	struct ParserContext {
		Common::SeekableReadStream *mdb;

		State *state;

		std::list<ModelNode_Witcher *> nodes;

		uint16 fileVersion;

		uint32 offModelData;
		uint32 modelDataSize;

		uint32 offRawData;
		uint32 rawDataSize;

		uint32 offTextureInfo;

		uint32 offTexData;
		uint32 texDatasize;

		ParserContext(Common::SeekableReadStream &stream);
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
	void readMesh(Model_Witcher::ParserContext &ctx);

	void readTextures(Model_Witcher::ParserContext &ctx,
	                  const Common::UString &texture,
	                  std::vector<Common::UString> &textures);
	void readNodeControllers(Model_Witcher::ParserContext &ctx,
	                         uint32 offset, uint32 count, std::vector<float> &data);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_NEWMODEL_WITCHER_H
