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

/** @file aurora/nwscript/objectcontainer.h
 *  An NWScript object container.
 */

#ifndef AURORA_NWSCRIPT_OBJECTCONTAINER_H
#define AURORA_NWSCRIPT_OBJECTCONTAINER_H

#include "common/mutex.h"

#include "aurora/nwscript/object.h"

namespace Aurora {

namespace NWScript {

class ObjectContainer {
public:
	ObjectContainer();
	~ObjectContainer();

	void add(Object &obj);
	void remove(Object &obj);

	Object *getObject(uint32 id) const;
	const Object *getConstObject(uint32 id) const;

	uint32 findFirstObject(const Common::UString &tag);
	uint32 findNextObject(const Common::UString &tag);

private:
	Common::Mutex _mutex;

	uint32 _currentID;

	ObjectIDMap  _objectsByID;
	ObjectTagMap _objectsByTag;

	Common::UString _currentSearchTag;
	std::pair<ObjectTagMap::const_iterator, ObjectTagMap::const_iterator> _currentSearchRange;
};

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_OBJECTCONTAINER_H
