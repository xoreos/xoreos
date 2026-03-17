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
 *  Star Wars: Knights of the Old Republic engine functions messing with objects.
 */

// TODO: check what happens on using invalid objects.

#include <cmath>
#include <memory>

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotorbase/types.h"
#include "src/engines/kotorbase/object.h"
#include "src/engines/kotorbase/placeable.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/objectcontainer.h"
#include "src/engines/kotorbase/location.h"
#include "src/engines/kotorbase/area.h"
#include "src/engines/kotorbase/game.h"
#include "src/engines/kotorbase/creature.h"
#include "src/engines/kotorbase/creaturesearch.h"

#include "src/engines/kotorbase/script/functions.h"

namespace Engines {

namespace KotORBase {

void Functions::getClickingObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getTriggerer();
}

void Functions::getEnteringObject(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: This should return the *last* entered object, i.e. it should remember past triggerers.
	ctx.getReturn() = ctx.getTriggerer();
}

void Functions::getExitingObject(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: This should return the *last* exited object, i.e. it should remember past triggerers.
	ctx.getReturn() = ctx.getTriggerer();
}

void Functions::getIsObjectValid(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getParamObject(ctx, 0) != 0;
}

void Functions::getIsPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = KotORBase::ObjectContainer::toPC(getParamObject(ctx, 0)) != 0;
}

void Functions::getObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString name = ctx.getParams()[0].getString();
	int nth = ctx.getParams()[1].getInt();

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(_game->getModule().findObjectsByTag(name));
	for (int i = 0; i < nth; ++i) {
		search->next();
	}

	ctx.getReturn() = search->get();
}

void Functions::getMinOneHP(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	Object *kotorObject = ObjectContainer::toObject(object);

	if (!kotorObject)
		throw Common::Exception("Functions::getMinOneHP(): invalid object");

	ctx.getReturn() = kotorObject->getMinOneHitPoints();
}

void Functions::setMinOneHP(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	bool enabled = ctx.getParams()[1].getInt();

	Object *kotorObject = ObjectContainer::toObject(object);

	if (!kotorObject)
		throw Common::Exception("Functions::setMinOneHP(): invalid object");

	kotorObject->setMinOneHitPoints(enabled);
}

void Functions::getCurrentHitPoints(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	Object *kotorObject = ObjectContainer::toObject(object);

	ctx.getReturn() = kotorObject ? kotorObject->getCurrentHitPoints() : 0;
}

void Functions::getMaxHitPoints(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	Object *kotorObject = ObjectContainer::toObject(object);

	ctx.getReturn() = kotorObject ? kotorObject->getMaxHitPoints() : 0;
}

void Functions::setMaxHitPoints(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	int maxHitPoints = ctx.getParams()[1].getInt();

	if (maxHitPoints == 0)
		maxHitPoints = 1;

	Object *kotorObject = ObjectContainer::toObject(object);
	if (!kotorObject)
		throw Common::Exception("Functions::setMaxHitPoints(): Invalid object");

	kotorObject->setCurrentHitPoints(maxHitPoints);
	kotorObject->setMaxHitPoints(maxHitPoints);
}

void Functions::getStandardFaction(Aurora::NWScript::FunctionContext &ctx) {
	const Object *object = ObjectContainer::toObject(ctx.getParams()[0].getObject());

	if (!object)
		ctx.getReturn() = kFactionInvalid;
	else
		ctx.getReturn() = object->getFaction();
}

void Functions::changeToStandardFaction(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = ObjectContainer::toObject(ctx.getParams()[0].getObject());
	int faction = ctx.getParams()[1].getInt();

	if (!object)
		throw Common::Exception("Functions::changeToStandardFaction(): Invalid object");

	object->setFaction(Faction(faction));
}

void Functions::createItemOnObject(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &itemTag = ctx.getParams()[0].getString();
	Aurora::NWScript::Object *object = ctx.getParams()[1].getObject();
	int32_t count = ctx.getParams()[2].getInt();

	Creature *creature = ObjectContainer::toCreature(object);
	if (creature) {
		creature->getInventory().addItem(itemTag, count);
		Item *item = creature->addScriptItem(itemTag);
		if (item)
			ctx.getReturn() = item;
		return;
	}

	Placeable *placeable = ObjectContainer::toPlaceable(object);
	if (placeable) {
		placeable->getInventory().addItem(itemTag, count);
		return;
	}

	throw Common::Exception("Functions::createItemOnObject(): Invalid object");
}

void Functions::getArea(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: return current area of the specified object
	ctx.getReturn() = _game->getModule().getCurrentArea();
}

void Functions::getLocation(Aurora::NWScript::FunctionContext &ctx) {
	const Object *object = ObjectContainer::toObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	ctx.getReturn() = object->getLocation();
}

void Functions::jumpToLocation(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = ObjectContainer::toObject(ctx.getCaller());
	Location *moveTo = ObjectContainer::toLocation(ctx.getParams()[0].getEngineType());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);
	object->setPosition(x, y, z);
}

void Functions::jumpToObject(Aurora::NWScript::FunctionContext &ctx) {
	Object *caller = ObjectContainer::toObject(ctx.getCaller());
	Object *target = ObjectContainer::toObject(getParamObject(ctx, 0));

	if (!caller || !target)
		return;

	float x, y, z;
	target->getPosition(x, y, z);
	caller->setPosition(x, y, z);
}

void Functions::locationCreate(Aurora::NWScript::FunctionContext &ctx) {
	// Location(vector position, float facing) -> location engine type
	float x, y, z;
	ctx.getParams()[0].getVector(x, y, z);
	float facing = ctx.getParams()[1].getFloat();

	Location loc;
	loc.setPosition(x, y, z);
	loc.setFacing(facing);

	ctx.getReturn() = loc;
}

void Functions::getWaypointByTag(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &tag = ctx.getParams()[0].getString();

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(
		_game->getModule().findObjectsByTag(tag));

	// Iterate until we find a waypoint
	Aurora::NWScript::Object *found = nullptr;
	while (Aurora::NWScript::Object *o = search->next()) {
		if (ObjectContainer::toWaypoint(o)) {
			found = o;
			break;
		}
	}

	ctx.getReturn() = found;
}

void Functions::getItemInSlot(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = 0;

	Aurora::NWScript::Object *object = ctx.getParams()[1].getObject();
	if (object)
		creature = ObjectContainer::toCreature(object);
	else
		creature = _game->getModule().getPC();

	if (!creature)
		throw Common::Exception("Functions::getItemInSlot(): Invalid creature");

	int slot = ctx.getParams()[0].getInt();
	Item *item = creature->getEquipedItem(static_cast<InventorySlot>(slot));
	if (item)
		ctx.getReturn() = item;
}

void Functions::getNearestCreature(Aurora::NWScript::FunctionContext &ctx) {
	CreatureSearchCriteria criteria;
	criteria.firstCriteriaType = static_cast<CreatureType>(ctx.getParams()[0].getInt());
	criteria.firstCriteriaValue = ctx.getParams()[1].getInt();

	Object *target = ObjectContainer::toCreature(ctx.getParams()[2].getObject());
	int nth = ctx.getParams()[3].getInt();

	criteria.secondCriteriaType = static_cast<CreatureType>(ctx.getParams()[4].getInt());
	criteria.secondCriteriaValue = ctx.getParams()[5].getInt();
	criteria.thirdCriteriaType = static_cast<CreatureType>(ctx.getParams()[6].getInt());
	criteria.thirdCriteriaValue = ctx.getParams()[7].getInt();

	ctx.getReturn() = _game->getModule().getCurrentArea()->getNearestCreature(target, nth, criteria);
}

void Functions::getTag(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = ObjectContainer::toObject(ctx.getParams()[0].getObject());
	if (!object)
		throw Common::Exception("Functions::getTag() parameter is not an object");

	ctx.getReturn() = object->getTag();
}

void Functions::destroyObject(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = ObjectContainer::toObject(ctx.getParams()[0].getObject());
	if (object)
		_game->getModule().getCurrentArea()->removeObject(object);
}

void Functions::getPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().setVector(0.0f, 0.0f, 0.0f);

	Object *object = ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	float x, y, z;
	object->getPosition(x, y, z);
	ctx.getReturn().setVector(x, y, z);
}

void Functions::getFacing(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0.0f;

	Object *object = ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	float x, y, z, angle;
	object->getOrientation(x, y, z, angle);
	ctx.getReturn() = angle;
}

void Functions::getItemPossessor(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = static_cast<Aurora::NWScript::Object *>(nullptr);

	Aurora::NWScript::Object *rawObj = ctx.getParams()[0].getObject();
	if (!rawObj)
		return;

	// Search all creatures in the current area for one that has this item equipped or in script items
	const std::vector<Creature *> &creatures = _game->getModule().getCurrentArea()->getCreatures();
	for (Creature *creature : creatures) {
		if (!creature)
			continue;
		// Check equipped items
		for (int slot = 0; slot < kInventorySlotMAX; ++slot) {
			Item *equipped = creature->getEquipedItem(static_cast<InventorySlot>(slot));
			if (equipped && equipped == static_cast<Object *>(rawObj)) {
				ctx.getReturn() = creature;
				return;
			}
		}
		// Check script items
		Item *found = creature->findInventoryItemByTag(
		    static_cast<Object *>(rawObj)->getTag());
		if (found && found == static_cast<Object *>(rawObj)) {
			ctx.getReturn() = creature;
			return;
		}
	}
}

void Functions::getItemPossessedBy(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = static_cast<Aurora::NWScript::Object *>(nullptr);

	Creature *creature = ObjectContainer::toCreature(getParamObject(ctx, 0));
	if (!creature)
		return;

	const Common::UString &tag = ctx.getParams()[1].getString();

	// Check equipped items first (live Item objects)
	Item *item = creature->findInventoryItemByTag(tag);
	if (item) {
		ctx.getReturn() = item;
		return;
	}

	// If tag is in inventory (tag+count), create a live Item object for scripting
	if (creature->getInventory().hasItem(tag)) {
		item = creature->addScriptItem(tag);
		if (item)
			ctx.getReturn() = item;
	}
}

void Functions::getObjectType(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = static_cast<int32_t>(kObjectTypeInvalid);

	Object *object = ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object || (static_cast<uint32_t>(object->getType()) >= kObjectTypeMAX))
		return;

	ctx.getReturn() = static_cast<int32_t>(object->getType());
}

void Functions::getDistanceToObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1.0f;

	Object *target = ObjectContainer::toObject(getParamObject(ctx, 0));
	Object *caller = ObjectContainer::toObject(ctx.getCaller());
	if (!target || !caller)
		return;

	float x1, y1, z1;
	target->getPosition(x1, y1, z1);

	float x2, y2, z2;
	caller->getPosition(x2, y2, z2);

	float dx = x1 - x2;
	float dy = y1 - y2;
	float dz = z1 - z2;
	ctx.getReturn() = std::sqrt(dx * dx + dy * dy + dz * dz);
}

void Functions::exploreAreaForPlayer(Aurora::NWScript::FunctionContext &ctx) {
	// Marks the entire area as explored on the minimap for the given player.
	// Full minimap revelation requires a rendering-layer hook; this stub
	// satisfies script execution so the Endar Spire module runs without error.
}

void Functions::getFirstItemInInventory(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = ObjectContainer::toObject(getParamObject(ctx, 0));
	ctx.getReturn() = static_cast<Aurora::NWScript::Object *>(nullptr);

	_inventoryIterObject = object;
	_inventoryIterTags.clear();
	_inventoryIterIndex = 0;

	if (!object)
		return;

	Creature *creature = ObjectContainer::toCreature(object);
	if (creature) {
		for (const auto &kv : creature->getInventory().getItems())
			_inventoryIterTags.push_back(kv.first);
	} else {
		Placeable *placeable = ObjectContainer::toPlaceable(object);
		if (placeable)
			for (const auto &kv : placeable->getInventory().getItems())
				_inventoryIterTags.push_back(kv.first);
	}

	if (_inventoryIterTags.empty())
		return;

	const Common::UString &tag = _inventoryIterTags[_inventoryIterIndex++];
	creature = ObjectContainer::toCreature(object);
	if (creature) {
		Item *item = creature->addScriptItem(tag);
		if (item)
			ctx.getReturn() = item;
	} else {
		Placeable *placeable = ObjectContainer::toPlaceable(object);
		if (placeable) {
			Item *item = placeable->addScriptItem(tag);
			if (item)
				ctx.getReturn() = item;
		}
	}
}

void Functions::getNextItemInInventory(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = static_cast<Aurora::NWScript::Object *>(nullptr);

	if (_inventoryIterIndex >= _inventoryIterTags.size())
		return;

	const Common::UString &tag = _inventoryIterTags[_inventoryIterIndex++];
	Object *object = _inventoryIterObject;
	if (!object)
		return;

	Creature *creature = ObjectContainer::toCreature(object);
	if (creature) {
		Item *item = creature->addScriptItem(tag);
		if (item)
			ctx.getReturn() = item;
	} else {
		Placeable *placeable = ObjectContainer::toPlaceable(object);
		if (placeable) {
			Item *item = placeable->addScriptItem(tag);
			if (item)
				ctx.getReturn() = item;
		}
	}
}

} // End of namespace KotORBase

} // End of namespace Engines
