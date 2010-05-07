/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#ifndef GRAPHICS_AURORA_MODEL_WITCHER_H
#define GRAPHICS_AURORA_MODEL_WITCHER_H

#include <vector>

#include "common/ustring.h"

#include "graphics/aurora/model.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class Model_Witcher : public Model {
public:
	Model_Witcher(Common::SeekableReadStream &mdb);
	~Model_Witcher();

private:
	struct ParserContext {
		Common::SeekableReadStream *mdb;

		Node *node;

		uint16 fileVersion;

		uint32 offModelData;
		uint32 modelDataSize;

		uint32 offRawData;
		uint32 rawDataSize;

		uint32 offTexData;
		uint32 texDatasize;

		Common::UString texture;

		std::vector<float>  vertices;
		std::vector<float>  verticesTexture;
		std::vector<uint32> faces;

		ParserContext(Common::SeekableReadStream &mdbStream);
		~ParserContext();
	};

	void load(Common::SeekableReadStream &mdb);

	void readNode(ParserContext &ctx, uint32 offset, Node *parent);

	void readArray(Common::SeekableReadStream &mdb, uint32 &start, uint32 &count);
	void readOffsetArray(Common::SeekableReadStream &mdb, uint32 start, uint32 count,
			std::vector<uint32> &offsets);
	void readFloatsArray(Common::SeekableReadStream &mdb, uint32 start, uint32 count,
			std::vector<float> &floats);

	void readMesh(ParserContext &ctx);
	void processNode(ParserContext &ctx);

	void parseNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_WITCHER_H
