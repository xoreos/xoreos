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
 *  Neverwinter Nights engine functions messing with objects.
 */

#include <memory>

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/game.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/objectcontainer.h"
#include "src/engines/nwn/object.h"
#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/script/functions.h"

namespace Engines {

namespace NWN {

using Aurora::NWScript::kTypeInt;
using Aurora::NWScript::kTypeFloat;
using Aurora::NWScript::kTypeString;
using Aurora::NWScript::kTypeObject;
using Aurora::NWScript::kTypeScriptState;

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

void Functions::getLocalInt(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeInt).getInt();
}

void Functions::getLocalFloat(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeFloat).getFloat();
}

void Functions::getLocalString(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeString).getString();
}

void Functions::getLocalObject(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeObject).getObject();
}

void Functions::setLocalInt(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getInt());
}

void Functions::setLocalFloat(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getFloat());
}

void Functions::setLocalString(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getString());
}

void Functions::setLocalObject(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getObject());
}

void Functions::getObjectType(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32_t) kObjectTypeInvalid;

	NWN::Object *object = NWN::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object || ((uint32_t)object->getType() >= kObjectTypeMAX))
		return;

	ctx.getReturn() = (int32_t) object->getType();
}

void Functions::getTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	Aurora::NWScript::Object *object = getParamObject(ctx, 0);

	// PCs don't have a tag, and the scripts depend on that
	if (object == _game->getModule().getPC())
		object = 0;

	if (object)
		ctx.getReturn() = object->getTag();
}

void Functions::getName(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: bOriginalName

	NWN::Object *object = NWN::ObjectContainer::toObject(getParamObject(ctx, 0));
	ctx.getReturn().getString() = object ? object->getName() : "";
}

void Functions::getArea(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	NWN::Object *object = NWN::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (object)
		ctx.getReturn() = (Aurora::NWScript::Object *) object->getArea();
}

void Functions::getLocation(Aurora::NWScript::FunctionContext &ctx) {
	NWN::Object *object = NWN::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	ctx.getReturn() = object->getLocation();
}

void Functions::getPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().setVector(0.0f, 0.0f, 0.0f);

	NWN::Object *object = NWN::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	float x, y, z;
	object->getPosition(x, y, z);

	ctx.getReturn().setVector(x, y, z);
}

#define SQR(x) ((x) * (x))
void Functions::getDistanceToObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1.0f;

	NWN::Object *object1 = NWN::ObjectContainer::toObject(getParamObject(ctx, 0));
	NWN::Object *object2 = NWN::ObjectContainer::toObject(ctx.getCaller());
	if (!object1 || !object2)
		return;

	float x1, y1, z1;
	object1->getPosition(x1, y1, z1);

	float x2, y2, z2;
	object2->getPosition(x2, y2, z2);

	ctx.getReturn() = sqrtf(SQR(x1 - x2) + SQR(y1 - y2) + SQR(z1 - z2));
}
#undef SQR

void Functions::getObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	const Common::UString &tag = ctx.getParams()[0].getString();
	if (tag.empty())
		return;

	int nth = ctx.getParams()[1].getInt();

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(_game->getModule().findObjectsByTag(tag));
	while (nth-- > 0)
		search->next();

	ctx.getReturn() = search->get();
}

void Functions::getWaypointByTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	const Common::UString &tag = ctx.getParams()[0].getString();
	if (tag.empty())
		return;

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(_game->getModule().findObjectsByTag(tag));
	Aurora::NWScript::Object *object = 0;

	while ((object = search->next())) {
		Waypoint *waypoint = NWN::ObjectContainer::toWaypoint(object);

		if (waypoint) {
			ctx.getReturn() = (Aurora::NWScript::Object *) waypoint;
			break;
		}
	}
}

void Functions::getNearestObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	NWN::Object *target = NWN::ObjectContainer::toObject(getParamObject(ctx, 1));
	if (!target)
		return;

	// Bitfield of type(s) to check for
	uint32_t type = ctx.getParams()[0].getInt();
	// We want the nth nearest object
	size_t nth  = MAX<int32_t>(ctx.getParams()[2].getInt() - 1, 0);

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(_game->getModule().findObjects());
	Aurora::NWScript::Object *object = 0;

	std::list<Object *> objects;
	while ((object = search->next())) {
		// Needs to be a valid object, not the target, but in the target's area
		NWN::Object *nwnObject = NWN::ObjectContainer::toObject(object);
		if (!nwnObject || (nwnObject == target) || (nwnObject->getArea() != target->getArea()))
			continue;

		// Ignore invalid object types
		const uint32_t objectType = (uint32_t) nwnObject->getType();
		if (objectType >= kObjectTypeMAX)
			continue;

		if (type & objectType)
			objects.push_back(nwnObject);
	}

	objects.sort(ObjectDistanceSort(*target));

	std::list<Object *>::iterator it = objects.begin();
	for (size_t n = 0; (n < nth) && (it != objects.end()); ++n)
		++it;

	if (it != objects.end())
		ctx.getReturn() = *it;
}

void Functions::getNearestObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Object *) 0;

	const Common::UString &tag = ctx.getParams()[0].getString();
	if (tag.empty())
		return;

	NWN::Object *target = NWN::ObjectContainer::toObject(getParamObject(ctx, 1));
	if (!target)
		return;

	size_t nth = MAX<int32_t>(ctx.getParams()[2].getInt() - 1, 0);

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(_game->getModule().findObjectsByTag(tag));
	Aurora::NWScript::Object *object = 0;

	std::list<Object *> objects;
	while ((object = search->next())) {
		// Needs to be a valid object, not the target, but in the target's area
		NWN::Object *nwnObject = NWN::ObjectContainer::toObject(object);
		if (!nwnObject || (nwnObject == target) || (nwnObject->getArea() != target->getArea()))
			continue;

		objects.push_back(nwnObject);
	}

	objects.sort(ObjectDistanceSort(*target));

	std::list<Object *>::iterator it = objects.begin();
	for (size_t n = 0; (n < nth) && (it != objects.end()); ++n)
		++it;

	if (it != objects.end())
		ctx.getReturn() = *it;
}

void Functions::getNearestCreature(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	NWN::Object *target = NWN::ObjectContainer::toObject(getParamObject(ctx, 2));
	if (!target)
		return;

	size_t nth = MAX<int32_t>(ctx.getParams()[3].getInt() - 1, 0);

	/* TODO: Criteria:
	 *
	 * int crit1Type  = ctx.getParams()[0].getInt();
	 * int crit1Value = ctx.getParams()[1].getInt();
	 * int crit2Type  = ctx.getParams()[4].getInt();
	 * int crit2Value = ctx.getParams()[5].getInt();
	 * int crit3Type  = ctx.getParams()[6].getInt();
	 * int crit3Value = ctx.getParams()[7].getInt();
	 */

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(_game->getModule().findObjects());
	Aurora::NWScript::Object *object = 0;

	std::list<Object *> creatures;
	while ((object = search->next())) {
		Creature *creature = NWN::ObjectContainer::toCreature(object);

		if (creature && (creature != target) && (creature->getArea() == target->getArea()))
			creatures.push_back(creature);
	}

	creatures.sort(ObjectDistanceSort(*target));

	std::list<Object *>::iterator it = creatures.begin();
	for (size_t n = 0; (n < nth) && (it != creatures.end()); ++n)
		++it;

	if (it != creatures.end())
		ctx.getReturn() = *it;
}

void Functions::playAnimation(Aurora::NWScript::FunctionContext &ctx) {
	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getCaller());
	if (!object)
		return;

	const Animation animation = (Animation) ctx.getParams()[0].getInt();

	float speed   = ctx.getParams()[1].getFloat();
	float seconds = ctx.getParams()[2].getFloat();

	if (isAnimationLooping(animation)) {
		/* Looping animations. Animations that can play for a variable amount
		   of time, like talking or meditating. */

		// Length of 0 seconds? Don't play the animation.
		if (seconds == 0.0f)
			return;

		if (seconds < 0.0f) {
			warning("Functions::%s(): Seconds == %f", ctx.getName().c_str(), seconds);
			return;
		}

		if (speed <= 0.0f) {
			warning("Functions::%s(): Speed == %f", ctx.getName().c_str(), speed);
			return;
		}

	} else {
		/* Fire-and-forget animations. Simple one-action animations, like
		   bowing, drinking from a flask, etc. On these animations, the speed
		   is ignored; they're always played a normal speed. Likewise, the
		   animation length (in seconds) is ignored; they're always played
		   one single time. We set the length to zero so that our animation
		   system can figure out the specifics. */

		speed   = 1.0f;
		seconds = 0.0f;
	}

	object->playAnimation(animation, true, seconds, speed);
}

void Functions::jumpToLocation(Aurora::NWScript::FunctionContext &ctx) {
	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getCaller());
	NWN::Location *moveTo = NWN::ObjectContainer::toLocation(ctx.getParams()[0].getEngineType());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);
}

void Functions::jumpToObject(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: walkStraightLineToPoint
	// bool walkStraightLineToPoint = ctx.getParams()[1].getInt() != 0;

	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getCaller());
	NWN::Object *moveTo = NWN::ObjectContainer::toObject(getParamObject(ctx, 0));

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);
}

} // End of namespace NWN

} // End of namespace Engines
