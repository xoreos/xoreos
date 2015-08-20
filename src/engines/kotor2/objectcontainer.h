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
 *  A container of Star Wars: Knights of the Old Republic II - The Sith Lords objects.
 */

#ifndef ENGINES_KOTOR2_OBJECTCONTAINER_H
#define ENGINES_KOTOR2_OBJECTCONTAINER_H

#include <list>
#include <map>

#include "src/common/types.h"

#include "src/aurora/nwscript/objectcontainer.h"

#include "src/engines/kotor2/types.h"

namespace Engines {

namespace KotOR2 {

class Object;
class Module;
class Area;
class Waypoint;
class Situated;
class Placeable;
class Door;
class Creature;

/** A class able to sort objects by distance to a target object. */
class ObjectDistanceSort {
public:
	ObjectDistanceSort(const KotOR2::Object &target);

	bool operator()(KotOR2::Object *a, KotOR2::Object *b);

private:
	float xt, yt, zt;

	float getDistance(KotOR2::Object &a);
};

class ObjectContainer : public ::Aurora::NWScript::ObjectContainer {
public:
	ObjectContainer();
	~ObjectContainer();

	void clearObjects();

	/** Add an object to this container. */
	void addObject(KotOR2::Object &object);
	/** Remove an object from this container. */
	void removeObject(KotOR2::Object &object);

	/** Return the first object of this type. */
	::Aurora::NWScript::Object *getFirstObjectByType(ObjectType type) const;

	/** Return a search context to iterate over all objects of this type. */
	::Aurora::NWScript::ObjectSearch *findObjectsByType(ObjectType type) const;

	static KotOR2::Object *toObject(::Aurora::NWScript::Object *object);

	static Module    *toModule   (Aurora::NWScript::Object *object);
	static Area      *toArea     (Aurora::NWScript::Object *object);
	static Waypoint  *toWaypoint (Aurora::NWScript::Object *object);
	static Situated  *toSituated (Aurora::NWScript::Object *object);
	static Placeable *toPlaceable(Aurora::NWScript::Object *object);
	static Door      *toDoor     (Aurora::NWScript::Object *object);
	static Creature  *toCreature (Aurora::NWScript::Object *object);
	static Creature  *toPC       (Aurora::NWScript::Object *object);

private:
	typedef std::list<KotOR2::Object *> ObjectList;
	typedef std::map<ObjectType, ObjectList> ObjectMap;

	ObjectMap _objects;
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_OBJECTCONTAINER_H
