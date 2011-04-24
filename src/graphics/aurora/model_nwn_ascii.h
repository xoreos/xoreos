/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/aurora/model_nwn_ascii.h
 *  Loading ASCII MDL files found in Neverwinter Nights.
 */

#ifndef GRAPHICS_AURORA_NEWMODEL_NWN_ASCII_H
#define GRAPHICS_AURORA_NEWMODEL_NWN_ASCII_H

#include "graphics/aurora/model.h"
#include "graphics/aurora/modelnode.h"

namespace Common {
	class SeekableReadStream;
	class StreamTokenizer;
}

namespace Graphics {

namespace Aurora {

class ModelNode_NWN_ASCII;

/** A 3D model in the NWN ASCII MDL format. */
class Model_NWN_ASCII : public Model {
public:
	Model_NWN_ASCII(Common::SeekableReadStream &mdl, ModelType type = kModelTypeObject);
	~Model_NWN_ASCII();

	static bool isASCII(Common::SeekableReadStream &mdl);

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;

		State *state;

		std::list<ModelNode_NWN_ASCII *> nodes;

		bool hasPosition;
		bool hasOrientation;

		Common::StreamTokenizer *tokenize;

		std::vector<uint32> anims;

		ParserContext(Common::SeekableReadStream &stream);
		~ParserContext();

		void clear();

		bool findNode(const Common::UString &name, ModelNode_NWN_ASCII *&node) const;
	};


	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void load(ParserContext &ctx);

	void skipAnim(ParserContext &ctx);
	void readAnim(ParserContext &ctx);

	friend class ModelNode_NWN_ASCII;
};

class ModelNode_NWN_ASCII : public ModelNode {
public:
	ModelNode_NWN_ASCII(Model &model);
	~ModelNode_NWN_ASCII();

	void load(Model_NWN_ASCII::ParserContext &ctx,
	          const Common::UString &type, const Common::UString &name);

private:
	struct Mesh {
		uint32 vCount;
		uint32 tCount;
		uint32 faceCount;

		std::vector<Common::UString> textures;

		std::vector<float> vX, vY, vZ;
		std::vector<float> tX, tY;

		std::vector<uint32> vIA, vIB, vIC;
		std::vector<uint32> tIA, tIB, tIC;

		std::vector<uint32> smooth, mat;

		Mesh();
	};

	void readConstraints(Model_NWN_ASCII::ParserContext &ctx, uint32 n);
	void readWeights(Model_NWN_ASCII::ParserContext &ctx, uint32 n);

	void readFloats(const std::vector<Common::UString> &strings,
	                float *floats, uint32 n, uint32 start);

	void readVCoords(Model_NWN_ASCII::ParserContext &ctx, Mesh &mesh);
	void readTCoords(Model_NWN_ASCII::ParserContext &ctx, Mesh &mesh);

	void readFaces(Model_NWN_ASCII::ParserContext &ctx, Mesh &mesh);

	void processMesh(Mesh &mesh);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_NEWMODEL_NWN_ASCII_H
