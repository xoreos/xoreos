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

/** @file graphics/aurora/model_nwn2.h
 *  Loading models found in Neverwinter Nights 2.
 */

#ifndef GRAPHICS_AURORA_MODEL_NWN2_H
#define GRAPHICS_AURORA_MODEL_NWN2_H

#include "common/ustring.h"

#include "graphics/materialman.h"

#include "graphics/meshutil.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/model.h"

namespace Ogre {
	class SceneNode;
	class Entity;
}

namespace Graphics {

namespace Aurora {

class Model_NWN2 : public Model {
public:
	Model_NWN2(const Common::UString &name, const Common::UString &scene = "world");
	~Model_NWN2();

private:
	struct PacketKey {
		uint32 signature;
		uint32 offset;
	};

	struct ParserContext {
		Common::SeekableReadStream *mdb;

		uint32 offModelData;
		uint32 offRawData;

		State *state;

		MaterialDeclaration material;
		NodeEntity *nodeEntity;

		ParserContext(const Common::UString &name);
		~ParserContext();

		void newState();
		void newNode();
	};


	void load(ParserContext &ctx);
	void loadNode(ParserContext &ctx, Ogre::SceneNode *parent, uint32 type);

	void loadRigid(ParserContext &ctx, Ogre::SceneNode *parent);
	void loadSkin (ParserContext &ctx, Ogre::SceneNode *parent);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_NWN2_H
