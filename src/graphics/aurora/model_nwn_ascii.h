/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model_nwn_ascii.h
 *  Loading ASCII MDL files found in Neverwinter Nights.
 */

#ifndef GRAPHICS_AURORA_MODEL_NWN_ASCII_H
#define GRAPHICS_AURORA_MODEL_NWN_ASCII_H

#include <vector>

#include "common/ustring.h"

#include "graphics/aurora/model.h"

namespace Common {
	class SeekableReadStream;
	class StreamTokenizer;
}

namespace Graphics {

namespace Aurora {

class Model_NWN_ASCII : public Model {
public:
	Model_NWN_ASCII(Common::SeekableReadStream &mdl, ModelType type = kModelTypeObject);
	~Model_NWN_ASCII();

	static bool isASCII(Common::SeekableReadStream &mdl);

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;

		State *state;
		Node  *node;
		Mesh  *mesh;

		ParserContext(Common::SeekableReadStream &stream);
		~ParserContext();
	};

	typedef std::map<Common::UString, Node *> NodeMap;

	NodeMap _nodeMap;

	Common::StreamTokenizer *_tokenize;

	void load(Common::SeekableReadStream &mdl);

	void readNode(ParserContext &ctx, const Common::UString &type, const Common::UString &name);
	void readAnim(ParserContext &ctx);

	Classification readClassification(Common::UString classification);
	void readFloats(const std::vector<Common::UString> &strings, float *floats, int n, int start);

	void readVertices(ParserContext &ctx, std::vector<float> &vertices, int n);
	void readFaces   (ParserContext &ctx, int n);
	void readWeights (ParserContext &ctx, int n);

	void readConstraints(ParserContext &ctx, std::vector<float> &constraints, int n);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_NWN_ASCII_H
