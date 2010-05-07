/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#ifndef GRAPHICS_AURORA_MODEL_NWN2_H
#define GRAPHICS_AURORA_MODEL_NWN2_H

#include <vector>

#include "common/ustring.h"

#include "graphics/aurora/model.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

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

		Node *node;

		Common::UString texture;

		std::vector<float>  vertices;
		std::vector<float>  verticesTexture;
		std::vector<uint16> faces;

		ParserContext(Common::SeekableReadStream &mdbStream);
		~ParserContext();
	};

	void load(Common::SeekableReadStream &mdb);

	void parseRigid(ParserContext &ctx, uint32 offset);
	void parseSkin (ParserContext &ctx, uint32 offset);

	void processNode(ParserContext &ctx);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_NWN2_H
