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
 *  A container of Sonic Chronicles: The Dark Brotherhood objects.
 */

#ifndef ENGINES_SONIC_OBJECTCONTAINER_H
#define ENGINES_SONIC_OBJECTCONTAINER_H

#include <list>
#include <map>

#include "src/common/types.h"

#include "src/aurora/nwscript/objectcontainer.h"

#include "src/engines/sonic/types.h"

namespace Engines {

namespace Sonic {

class Object;
class Module;
class Area;
class Placeable;

/** A class able to sort objects by distance to a target object. */
class ObjectDistanceSort {
public:
	ObjectDistanceSort(const Sonic::Object &target);

	bool operator()(Sonic::Object *a, Sonic::Object *b);

private:
	float xt, yt, zt;

	float getDistance(Sonic::Object &a);
};

class ObjectContainer : public ::Aurora::NWScript::ObjectContainer {
public:
	ObjectContainer();
	~ObjectContainer();

	void clearObjects();

	/** Add an object to this container. */
	void addObject(Sonic::Object &object);
	/** Remove an object from this container. */
	void removeObject(Sonic::Object &object);

	/** Return the first object of this type. */
	::Aurora::NWScript::Object *getFirstObjectByType(ObjectType type) const;

	/** Return a search context to iterate over all objects of this type. */
	::Aurora::NWScript::ObjectSearch *findObjectsByType(ObjectType type) const;

	static Sonic::Object *toObject(::Aurora::NWScript::Object *object);

	static Module    *toModule   (Aurora::NWScript::Object *object);
	static Area      *toArea     (Aurora::NWScript::Object *object);
	static Placeable *toPlaceable(Aurora::NWScript::Object *object);

private:
	typedef std::list<Sonic::Object *> ObjectList;
	typedef std::map<ObjectType, ObjectList> ObjectMap;

	ObjectMap _objects;
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_OBJECTCONTAINER_H
