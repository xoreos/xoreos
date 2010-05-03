/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/model_nwn.h
 *  Loading MDL files found in Neverwinter Nights.
 */

#ifndef GRAPHICS_MODEL_NWN_H
#define GRAPHICS_MODEL_NWN_H

#include <vector>

#include "common/ustring.h"

#include "graphics/model.h"

namespace Common {
	class SeekableReadStream;
	class StreamTokenizer;
}

namespace Graphics {

class Model_NWN : public Model {
public:
	Model_NWN(Common::SeekableReadStream &mdl);
	~Model_NWN();

private:
	struct FaceASCII {
		int vertices[3];
		int verticesTexture[3];

		int smoothGroup;
		int material;
	};

	Common::StreamTokenizer *_tokenizeASCII;

	void load(Common::SeekableReadStream &mdl);

	void loadASCII(Common::SeekableReadStream &mdl);
	void loadBinary(Common::SeekableReadStream &mdl);

	void parseNodeASCII(Common::SeekableReadStream &mdl, const Common::UString &type, const Common::UString &name);
	void parseAnimASCII(Common::SeekableReadStream &mdl);

	Classification parseClassification(Common::UString classification);
	void parseFloats(const std::vector<Common::UString> &strings, float *floats, int n, int start);

	void parseVerticesASCII(Common::SeekableReadStream &mdl, std::vector<float> &vertices, int n);
	void parseFacesASCII(Common::SeekableReadStream &mdl, std::vector<FaceASCII> &faces, int n);
	void parseConstraintsASCII(Common::SeekableReadStream &mdl, std::vector<float> &constraints, int n);
	void parseWeightsASCII(Common::SeekableReadStream &mdl, int n);
};

} // End of namespace Graphics

#endif // GRAPHICS_MODEL_NWN_H
