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

/** @file graphics/aurora/model_nwn_binary.h
 *  Loading binary MDL files found in Neverwinter Nights.
 */

#ifndef GRAPHICS_AURORA_NEWMODEL_NWN_BINARY_H
#define GRAPHICS_AURORA_NEWMODEL_NWN_BINARY_H

#include "graphics/aurora/model.h"
#include "graphics/aurora/modelnode.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class ModelNode_NWN_Binary;

/** A 3D model in the NWN binary MDL format. */
class Model_NWN_Binary : public Model {
public:
	Model_NWN_Binary(Common::SeekableReadStream &mdl, ModelType type = kModelTypeObject);
	~Model_NWN_Binary();

	static bool isBinary(Common::SeekableReadStream &mdl);

private:
	struct ParserContext {
		Common::SeekableReadStream *mdl;

		State *state;

		ModelNode_NWN_Binary *rootNode;
		std::list<ModelNode_NWN_Binary *> nodes;

		uint32 offModelData;
		uint32 offRawData;

		bool hasPosition;
		bool hasOrientation;

		ParserContext(Common::SeekableReadStream &stream);
		~ParserContext();

		void clear();
	};


	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void load(ParserContext &ctx);

	void readAnim(ParserContext &ctx, uint32 offset);

	friend class ModelNode_NWN_Binary;
};

class ModelNode_NWN_Binary : public ModelNode {
public:
	ModelNode_NWN_Binary(Model &model);
	~ModelNode_NWN_Binary();

	void load(Model_NWN_Binary::ParserContext &ctx);

private:
	void readMesh(Model_NWN_Binary::ParserContext &ctx);
	void readAnim(Model_NWN_Binary::ParserContext &ctx);

	void readNodeControllers(Model_NWN_Binary::ParserContext &ctx, uint32 offset,
                           uint32 count, std::vector<float> &data);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_NEWMODEL_NWN_BINARY_H
