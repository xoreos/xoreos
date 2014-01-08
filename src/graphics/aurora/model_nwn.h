/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/aurora/model_nwn.h
 *  Loading models found in Neverwinter Nights.
 */

#ifndef GRAPHICS_AURORA_MODEL_NWN_H
#define GRAPHICS_AURORA_MODEL_NWN_H

#include "common/ustring.h"

#include "graphics/materialman.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/model.h"
#include "graphics/aurora/meshutil.h"

namespace Ogre {
	class SceneNode;
	class Entity;
}

namespace Common {
	class StreamTokenizer;
}

namespace Graphics {

namespace Aurora {

class Model_NWN : public Model {
public:
	Model_NWN(const Common::UString &name, const Common::UString &texture = "");
	~Model_NWN();

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;

		bool isASCII;

		uint32 offModelData;
		uint32 offRawData;

		Common::UString texture;

		Common::StreamTokenizer *tokenize;

		State *state;

		MaterialDeclaration material;
		NodeEntity *nodeEntity;

		ParserContext(const Common::UString &name, const Common::UString &t);
		~ParserContext();

		void newState();
		void newNode();
	};

	struct MeshASCII {
		uint32 vCount;
		uint32 tCount;
		uint32 faceCount;

		std::vector<float> vX, vY, vZ;
		std::vector<float> tX, tY;

		std::vector<uint32> vIA, vIB, vIC;
		std::vector<uint32> tIA, tIB, tIC;

		std::vector<uint32> smooth, mat;

		MeshASCII();
	};

	Common::UString _fileName;
	Common::UString _name;

	Common::UString _superModelName;

	float _animationScale;


	void loadBinary(ParserContext &ctx);
	void loadBinaryNode(ParserContext &ctx, Ogre::SceneNode *parent);
	void readBinaryMesh(ParserContext &ctx);
	void readBinaryAnim(ParserContext &ctx);
	void readBinaryNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data);

	void loadASCII(ParserContext &ctx);
	void loadASCIINode(ParserContext &ctx, Ogre::SceneNode *parent,
	                   const Common::UString &type, const Common::UString &name);
	void skipASCIIAnim(ParserContext &ctx);
	void readASCIIConstraints(ParserContext &ctx, uint32 n);
	void readASCIIWeights(ParserContext &ctx, uint32 n);
	void readASCIIFloats(const std::vector<Common::UString> &strings, float *floats, uint32 n, uint32 start);
	void readASCIIVCoords(ParserContext &ctx, MeshASCII &mesh);
	void readASCIITCoords(ParserContext &ctx, MeshASCII &mesh);
	void readASCIIFaces(ParserContext &ctx, MeshASCII &mesh);
	void processASCIIMesh(ParserContext &ctx, MeshASCII &mesh);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_NWN_H
