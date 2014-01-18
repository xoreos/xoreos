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

/** @file graphics/aurora/model_witcher.h
 *  Loading models found in The Witcher.
 */

#ifndef GRAPHICS_AURORA_MODEL_WITCHER_H
#define GRAPHICS_AURORA_MODEL_WITCHER_H

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

class Model_Witcher : public Model {
public:
	Model_Witcher(const Common::UString &name);
	~Model_Witcher();

private:
	struct ParserContext {
		Common::SeekableReadStream *mdb;

		uint16 fileVersion;

		uint32 offModelData;
		uint32 modelDataSize;

		uint32 offRawData;
		uint32 rawDataSize;

		uint32 offTextureInfo;

		uint32 offTexData;
		uint32 texDatasize;

		State *state;

		MaterialDeclaration material;
		NodeEntity *nodeEntity;

		ParserContext(const Common::UString &name);
		~ParserContext();

		void newState();
		void newNode();
	};

	Common::UString _fileName;
	Common::UString _name;


	void load(ParserContext &ctx);
	void loadNode(ParserContext &ctx, Ogre::SceneNode *parent);

	void readMesh(ParserContext &ctx);
	void readNodeControllers(ParserContext &ctx, uint32 offset, uint32 count, std::vector<float> &data);

	void readTextures(ParserContext &ctx, const Common::UString &texture,
	                  std::vector<Common::UString> &textures);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_WITCHER_H
