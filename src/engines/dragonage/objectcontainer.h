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

#include "src/common/types.h"

#include "src/aurora/nwscript/objectcontainer.h"

#include "src/engines/dragonage/types.h"

namespace Engines {

namespace DragonAge {

class ScriptObject;

class Area;
class Object;
class Waypoint;
class Placeable;
class Creature;

class ObjectContainer : public ::Aurora::NWScript::ObjectContainer {
public:
	ObjectContainer();
	~ObjectContainer();

	void clearObjects();

	/** Add an object to this container. */
	void addObject(ScriptObject &object);
	/** Remove an object from this container. */
	void removeObject(ScriptObject &object);

	/** Return the first object of this type. */
	::Aurora::NWScript::Object *getFirstObjectByType(ObjectType type) const;

	/** Return a search context to iterate over all objects of this type. */
	::Aurora::NWScript::ObjectSearch *findObjectsByType(ObjectType type) const;

	static ScriptObject *toScriptObject(::Aurora::NWScript::Object *object);

	static DragonAge::Object *toObject(::Aurora::NWScript::Object *object);

	static Area      *toArea     (::Aurora::NWScript::Object *object);
	static Waypoint  *toWaypoint (::Aurora::NWScript::Object *object);
	static Placeable *toPlaceable(::Aurora::NWScript::Object *object);
	static Creature  *toCreature (::Aurora::NWScript::Object *object);

private:
	typedef std::list<ScriptObject *> ObjectList;

	ObjectList _objects[kObjectTypeMAX];
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_OBJECTCONTAINER_H
