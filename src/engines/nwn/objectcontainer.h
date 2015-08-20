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
 *  A container of Neverwinter Nights objects.
 */

#ifndef ENGINES_NWN_OBJECTCONTAINER_H
#define ENGINES_NWN_OBJECTCONTAINER_H

#include <list>
#include <map>

#include "src/common/types.h"

#include "src/aurora/nwscript/objectcontainer.h"

#include "src/engines/nwn/types.h"

namespace Engines {

namespace NWN {

class Object;
class Module;
class Area;
class Waypoint;
class Situated;
class Placeable;
class Door;
class Creature;
class Location;

/** A class able to sort objects by distance to a target object. */
class ObjectDistanceSort {
public:
	ObjectDistanceSort(const NWN::Object &target);

	bool operator()(NWN::Object *a, NWN::Object *b);

private:
	float xt, yt, zt;

	float getDistance(NWN::Object &a);
};

class ObjectContainer : public ::Aurora::NWScript::ObjectContainer {
public:
	ObjectContainer();
	~ObjectContainer();

	void clearObjects();

	/** Add an object to this container. */
	void addObject(NWN::Object &object);
	/** Remove an object from this container. */
	void removeObject(NWN::Object &object);

	/** Return the first object of this type. */
	::Aurora::NWScript::Object *getFirstObjectByType(ObjectType type) const;

	/** Return a search context to iterate over all objects of this type. */
	::Aurora::NWScript::ObjectSearch *findObjectsByType(ObjectType type) const;

	static NWN::Object *toObject(::Aurora::NWScript::Object *object);

	static Module    *toModule   (Aurora::NWScript::Object *object);
	static Area      *toArea     (Aurora::NWScript::Object *object);
	static Waypoint  *toWaypoint (Aurora::NWScript::Object *object);
	static Situated  *toSituated (Aurora::NWScript::Object *object);
	static Placeable *toPlaceable(Aurora::NWScript::Object *object);
	static Door      *toDoor     (Aurora::NWScript::Object *object);
	static Creature  *toCreature (Aurora::NWScript::Object *object);
	static Creature  *toPC       (Aurora::NWScript::Object *object);

	static Location *toLocation(Aurora::NWScript::EngineType *engineType);

private:
	typedef std::list<NWN::Object *> ObjectList;
	typedef std::map<ObjectType, ObjectList> ObjectMap;

	ObjectMap _objects;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_OBJECTCONTAINER_H
