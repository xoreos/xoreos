/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#ifndef GRAPHICS_AURORA_MODEL_KOTOR_H
#define GRAPHICS_AURORA_MODEL_KOTOR_H

#include <vector>

#include "common/ustring.h"

#include "graphics/aurora/model.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class Model_KotOR : public Model {
public:
	Model_KotOR(Common::SeekableReadStream &mdl, Common::SeekableReadStream &mdx, bool kotor2,
			ModelType type = kModelTypeObject, const Common::UString &texture = "");
	~Model_KotOR();

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;
		Common::SeekableReadStream *mdx;

		Common::UString texture;

		State *state;
		Node  *node;
		Mesh  *mesh;

		uint32 offModelData;
		uint32 offRawData;

		ParserContext(Common::SeekableReadStream &mdlStream, Common::SeekableReadStream &mdxStream,
				const Common::UString &text);
		~ParserContext();
	};

	bool _kotor2;

	std::vector<Common::UString> _names;

	void load(Common::SeekableReadStream &mdl, Common::SeekableReadStream &mdx, const Common::UString &texture);

	void readNode(ParserContext &ctx, uint32 offset, Node *parent);
	void readMesh(ParserContext &ctx);

	void readStrings(Common::SeekableReadStream &mdl, const std::vector<uint32> &offsets,
			uint32 offset, std::vector<Common::UString> &strings);

	void readNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_KOTOR_H
