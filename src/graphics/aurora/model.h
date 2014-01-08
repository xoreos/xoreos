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

/** @file graphics/aurora/model.h
 *  Base class for models found in Aurora games.
 */

#ifndef GRAPHICS_AURORA_MODEL_H
#define GRAPHICS_AURORA_MODEL_H

#include <vector>
#include <map>

#include "common/types.h"
#include "common/ustring.h"

#include "graphics/aurora/renderable.h"

namespace Ogre {
	class SceneNode;
	class Entity;
}

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class Model : public Renderable {
public:
	Model();
	~Model();

protected:
	struct NodeEntity {
		Ogre::SceneNode *node;
		Ogre::Entity *entity;

		bool dontRender;

		NodeEntity();
	};

	typedef std::list<Ogre::Entity *> EntityList;
	typedef std::map<Common::UString, NodeEntity> NodeEntities;

	struct State {
		Common::UString name;

		NodeEntities nodeEntities;

		State(const Common::UString &n = "");
	};

	typedef std::map<Common::UString, State *> StateMap;


	StateMap   _states;
	EntityList _entities;

	State *_currentState;


	void setState(State *state);


	// General loading helpers

	static void readValue(Common::SeekableReadStream &stream, uint32 &value);
	static void readValue(Common::SeekableReadStream &stream, float  &value);

	static void readArrayDef(Common::SeekableReadStream &stream,
	                         uint32 &offset, uint32 &count);

	template<typename T>
	static void readArray(Common::SeekableReadStream &stream,
	                      uint32 offset, uint32 count, std::vector<T> &values);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_H
