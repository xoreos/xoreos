/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model_nwn.h
 *  Loading MDL files found in Neverwinter Nights.
 */

#ifndef GRAPHICS_AURORA_MODEL_NWN_H
#define GRAPHICS_AURORA_MODEL_NWN_H

#include <vector>

#include "common/ustring.h"

#include "graphics/aurora/model.h"

namespace Common {
	class SeekableReadStream;
	class StreamTokenizer;
}

namespace Graphics {

namespace Aurora {

class Model_NWN : public Model {
public:
	Model_NWN(Common::SeekableReadStream &mdl);
	~Model_NWN();

private:
	struct FaceNWN {
		int vertices[3];
		int verticesTexture[3];

		int smoothGroup;
		int material;
	};

	struct ParserContext {
		Common::SeekableReadStream *mdl;

		Node *node;

		std::vector<float> vertices;
		std::vector<float> verticesTexture;
		std::vector<FaceNWN> faces;

		uint32 offModelData;
		uint32 offRawData;

		ParserContext(Common::SeekableReadStream &stream);
		~ParserContext();
	};

	Common::StreamTokenizer *_tokenizeASCII;

	void load(Common::SeekableReadStream &mdl);

	void loadASCII(ParserContext &ctx);
	void loadBinary(ParserContext &ctx);

	void parseNodeASCII(ParserContext &ctx, const Common::UString &type, const Common::UString &name);
	void parseAnimASCII(ParserContext &ctx);

	Classification parseClassification(Common::UString classification);
	void parseFloats(const std::vector<Common::UString> &strings, float *floats, int n, int start);

	void parseVerticesASCII(ParserContext &ctx, std::vector<float> &vertices, int n);
	void parseFacesASCII   (ParserContext &ctx, int n);
	void parseWeightsASCII (ParserContext &ctx, int n);

	void parseConstraintsASCII(ParserContext &ctx, std::vector<float> &constraints, int n);

	void parseNodeBinary(ParserContext &ctx, uint32 offset, Node *parent);
	void parseMeshBinary(ParserContext &ctx);

	void readArray(Common::SeekableReadStream &mdl, uint32 &start, uint32 &count);
	void readOffsetArray(Common::SeekableReadStream &mdl, uint32 start, uint32 count,
			std::vector<uint32> &offsets);

	void processNode(ParserContext &ctx);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_NWN_H
