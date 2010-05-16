/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model_nwn_binary.h
 *  Loading Binary MDL files found in Neverwinter Nights.
 */

#ifndef GRAPHICS_AURORA_MODEL_NWN_BINARY_H
#define GRAPHICS_AURORA_MODEL_NWN_BINARY_H

#include <vector>

#include "common/ustring.h"

#include "graphics/aurora/model.h"

namespace Common {
	class SeekableReadStream;
	class StreamTokenizer;
}

namespace Graphics {

namespace Aurora {

class Model_NWN_Binary : public Model {
public:
	Model_NWN_Binary(Common::SeekableReadStream &mdl, ModelType type = kModelTypeObject);
	~Model_NWN_Binary();

	static bool isBinary(Common::SeekableReadStream &mdl);

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;

		State *state;
		Node  *node;
		Mesh  *mesh;

		uint32 offModelData;
		uint32 offRawData;

		bool hasPosition;
		bool hasOrientation;

		ParserContext(Common::SeekableReadStream &stream);
		~ParserContext();
	};

	typedef std::map<Common::UString, Node *> NodeMap;

	NodeMap _nodeMap;

	void load(Common::SeekableReadStream &mdl);

	void readNode(ParserContext &ctx, uint32 offset, Node *parent, bool rootState);
	void readMesh(ParserContext &ctx);
	void readAnim(ParserContext &ctx);

	void readAnimGeometry(ParserContext &ctx, uint32 offset);

	void readNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_NWN_BINARY_H
