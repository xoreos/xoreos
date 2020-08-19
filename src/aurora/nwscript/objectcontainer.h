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

#include <list>
#include <map>

#include "src/common/mutex.h"

#include "src/aurora/nwscript/object.h"

namespace Aurora {

namespace NWScript {

class ObjectSearch {
public:
	ObjectSearch() { }
	virtual ~ObjectSearch() { }

	/** Return the current object in the search context. */
	virtual Object *get()  = 0;
	/** Move to the next object in the search context and return the previous one. */
	virtual Object *next() = 0;
};

template<typename T> class SearchRange : public ObjectSearch {
public:
	typedef T type;
	typedef typename T::const_iterator iterator;
	typedef std::pair<iterator, iterator> range;

	SearchRange(const range &r) : _range(r) { }
	~SearchRange() { }

	Object *get() {
		if (_range.first == _range.second)
			return 0;

		return getObject(_range.first);
	}

	Object *next() {
		if (_range.first == _range.second)
			return 0;

		Object *object = getObject(_range.first);

		++_range.first;

		return object;
	}

protected:
	virtual Object *getObject(const iterator &t) = 0;

private:
	range _range;
};

class SearchList : public SearchRange< std::list<Object *> > {
public:
	SearchList(const type &l) : SearchRange<type>(std::make_pair(l.begin(), l.end())) { }
	~SearchList() { }

	Object *getObject(const iterator &t) { return *t; }
};

class SearchTagMap : public SearchRange< std::multimap<Common::UString, Object *> > {
public:
	SearchTagMap(const type &m, const Common::UString &tag) : SearchRange<type>(m.equal_range(tag)) { }
	~SearchTagMap() { }

	Object *getObject(const iterator &t) { return t->second; };
};

class ObjectContainer {
public:
	ObjectContainer();
	~ObjectContainer();

	void clearObjects();

	/** Add an object to this container. */
	void addObject(Object &object);
	/** Remove an object from this container. */
	void removeObject(Object &object);

	/** Find a specific object by ID. */
	Object *getObjectByID(uint32_t id) const;

	/** Return the first object. */
	Object *getFirstObject() const;
	/** Return the first object with this tag. */
	Object *getFirstObjectByTag(const Common::UString &tag) const;

	/** Return a search context to iterate over all objects. */
	ObjectSearch *findObjects() const;
	/** Return a search context to iterate over all objects with this tag. */
	ObjectSearch *findObjectsByTag(const Common::UString &tag) const;


protected:
	void lock();
	void unlock();


private:
	typedef std::map<uint32_t, Object *> ObjectIDMap;
	typedef SearchList::type ObjectList;
	typedef SearchTagMap::type ObjectTagMap;

	std::recursive_mutex _mutex;

	ObjectList   _objects;
	ObjectIDMap  _objectsByID;
	ObjectTagMap _objectsByTag;
};

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_OBJECTCONTAINER_H
