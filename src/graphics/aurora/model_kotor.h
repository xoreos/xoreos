/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model_kotor.h
 *  Loading MDL files found in Star Wars: Knights of the Old Republic.
 */

#ifndef GRAPHICS_AURORA_NEWMODEL_KOTOR_H
#define GRAPHICS_AURORA_NEWMODEL_KOTOR_H

#include "graphics/aurora/model.h"
#include "graphics/aurora/modelnode.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class ModelNode_KotOR;

/** A 3D model in the NWN binary MDL format. */
class Model_KotOR : public Model {
public:
	Model_KotOR(Common::SeekableReadStream &mdl, Common::SeekableReadStream &mdx,
	            bool kotor2, ModelType type = kModelTypeObject,
	            const Common::UString &texture = "");
	~Model_KotOR();

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;
		Common::SeekableReadStream *mdx;

		State *state;

		std::list<ModelNode_KotOR *> nodes;

		Common::UString texture;

		bool kotor2;

		uint32 offModelData;
		uint32 offRawData;

		std::vector<Common::UString> names;

		ParserContext(Common::SeekableReadStream &mdlStream,
		              Common::SeekableReadStream &mdxStream,
		              const Common::UString &t, bool k2);
		~ParserContext();

		void clear();
	};


	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void load(ParserContext &ctx);

	void readStrings(Common::SeekableReadStream &mdl,
			const std::vector<uint32> &offsets, uint32 offset,
			std::vector<Common::UString> &strings);

	friend class ModelNode_KotOR;
};

class ModelNode_KotOR : public ModelNode {
public:
	ModelNode_KotOR(Model &model);
	~ModelNode_KotOR();

	void load(Model_KotOR::ParserContext &ctx);

private:
	void readNodeControllers(Model_KotOR::ParserContext &ctx, uint32 offset,
	                         uint32 count, std::vector<float> &data);
	void readMesh(Model_KotOR::ParserContext &ctx);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_NEWMODEL_KOTOR_H
