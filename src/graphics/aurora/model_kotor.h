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
	Model_KotOR(Common::SeekableReadStream &mdl, Common::SeekableReadStream &mdx, bool kotor2);
	~Model_KotOR();

private:
	struct FaceKotOR {
		int vertices[3];
		int verticesTexture[3];

		int smoothGroup;
		int material;
	};

	struct ParserContext {
		Common::SeekableReadStream *mdl;
		Common::SeekableReadStream *mdx;

		Node *node;

		std::vector<float> vertices;
		std::vector<float> verticesTexture;
		std::vector<FaceKotOR> faces;

		uint32 offModelData;
		uint32 offRawData;

		ParserContext(Common::SeekableReadStream &mdlStream, Common::SeekableReadStream &mdxStream);
		~ParserContext();
	};

	bool _kotor2;

	std::vector<Common::UString> _names;

	void load(Common::SeekableReadStream &mdl, Common::SeekableReadStream &mdx);

	void parseNode(ParserContext &ctx, uint32 offset, Node *parent);
	void parseMesh(ParserContext &ctx);

	void readArray(Common::SeekableReadStream &mdl, uint32 &start, uint32 &count);
	void readOffsetArray(Common::SeekableReadStream &mdl, uint32 start, uint32 count,
			std::vector<uint32> &offsets);
	void readFloatsArray(Common::SeekableReadStream &mdl, uint32 start, uint32 count,
			std::vector<float> &floats);

	void readStrings(Common::SeekableReadStream &mdl, const std::vector<uint32> &offsets,
			uint32 offset, std::vector<Common::UString> &strings);

	void parseNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data);

	void processNode(ParserContext &ctx);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_KOTOR_H
