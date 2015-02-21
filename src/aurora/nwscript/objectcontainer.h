/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  An NWScript object container.
 */

#ifndef AURORA_NWSCRIPT_OBJECTCONTAINER_H
#define AURORA_NWSCRIPT_OBJECTCONTAINER_H

#include "src/common/mutex.h"

#include "src/aurora/nwscript/object.h"

namespace Aurora {

namespace NWScript {

class ObjectContainer {
public:
	class SearchContext {
	public:
		SearchContext();
		~SearchContext();

		Object *getObject() const;

	private:
		bool _empty;
		Object *_object;
		Common::UString _tag;
		std::pair<ObjectTagMap::const_iterator, ObjectTagMap::const_iterator> _range;

		friend class ObjectContainer;
	};

	ObjectContainer();
	~ObjectContainer();

	/** Add an object to this container. */
	void addObject(Object &obj);
	/** Remove an object from this container. */
	void removeObject(Object &obj);

	/** Init a search context for finding all objects. */
	bool findObjectInit(SearchContext &ctx) const;
	/** Init a search context for finding all objects with this tag. */
	bool findObjectInit(SearchContext &ctx, const Common::UString &tag) const;

	/** Find the next object. */
	Object *findNextObject(SearchContext &ctx) const;

	/** Find the first best object, disregarding any other matches. */
	Object *findObject() const;
	/** Find the first best object with this tag, disregarding any other matches. */
	Object *findObject(const Common::UString &tag) const;

private:
	Common::Mutex _mutex;

	uint32 _currentID;

	ObjectTagMap _objects;
};

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_OBJECTCONTAINER_H
