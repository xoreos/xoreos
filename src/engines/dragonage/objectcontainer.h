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
 *  A container of Dragon Age: Origins objects.
 */

#ifndef ENGINES_DRAGONAGE_OBJECTCONTAINER_H
#define ENGINES_DRAGONAGE_OBJECTCONTAINER_H

#include <list>
#include <map>

#include "src/common/types.h"

#include "src/aurora/nwscript/objectcontainer.h"

#include "src/engines/dragonage/types.h"

namespace Aurora {
	namespace NWScript {
		class EngineType;
	}
}

namespace Engines {

namespace DragonAge {

class Object;
class Area;
class Waypoint;
class Placeable;
class Creature;

class Event;

/** A class able to sort objects by distance to a target object. */
class ObjectDistanceSort {
public:
	ObjectDistanceSort(const DragonAge::Object &target);

	bool operator()(DragonAge::Object *a, DragonAge::Object *b);

private:
	float xt, yt, zt;

	float getDistance(DragonAge::Object &a);
};

class ObjectContainer : public ::Aurora::NWScript::ObjectContainer {
public:
	ObjectContainer();
	~ObjectContainer();

	void clearObjects();

	/** Add an object to this container. */
	void addObject(DragonAge::Object &object);
	/** Remove an object from this container. */
	void removeObject(DragonAge::Object &object);

	/** Return the first object of this type. */
	::Aurora::NWScript::Object *getFirstObjectByType(ObjectType type) const;

	/** Return a search context to iterate over all objects of this type. */
	::Aurora::NWScript::ObjectSearch *findObjectsByType(ObjectType type) const;

	static DragonAge::Object *toObject(::Aurora::NWScript::Object *object);

	static Area      *toArea     (Aurora::NWScript::Object *object);
	static Waypoint  *toWaypoint (Aurora::NWScript::Object *object);
	static Placeable *toPlaceable(Aurora::NWScript::Object *object);
	static Creature  *toCreature (Aurora::NWScript::Object *object);

	static Event *toEvent(Aurora::NWScript::EngineType *engineType);

private:
	typedef std::list<DragonAge::Object *> ObjectList;
	typedef std::map<ObjectType, ObjectList> ObjectMap;

	ObjectMap _objects;
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_OBJECTCONTAINER_H
