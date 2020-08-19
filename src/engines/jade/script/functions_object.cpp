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
 *  Jade Empire engine functions messing with objects.
 */

#include <memory>
#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/jade/types.h"
#include "src/engines/jade/game.h"
#include "src/engines/jade/module.h"
#include "src/engines/jade/objectcontainer.h"
#include "src/engines/jade/object.h"

#include "src/engines/jade/script/functions.h"

namespace Engines {

namespace Jade {

using Aurora::NWScript::kTypeInt;
using Aurora::NWScript::kTypeFloat;
using Aurora::NWScript::kTypeString;
using Aurora::NWScript::kTypeObject;
using Aurora::NWScript::kTypeScriptState;

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
	ctx.getReturn() = Jade::ObjectContainer::toPC(getParamObject(ctx, 0)) != 0;
}

void Functions::getLocalInt(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	Common::UString varName = Common::UString::format("LOCAL_INT_%i", ctx.getParams()[1].getInt());
	if (object)
		ctx.getReturn() = object->getVariable(varName, kTypeInt).getInt();
}

void Functions::getLocalBool(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	Common::UString varName = Common::UString::format("LOCAL_BOOL_%i", ctx.getParams()[1].getInt());
	if (object)
		ctx.getReturn() = object->getVariable(varName, kTypeInt).getInt() != 0;
}

void Functions::getLocalFloat(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	Common::UString varName = Common::UString::format("LOCAL_FLOAT_%i", ctx.getParams()[1].getInt());
	if (object)
		ctx.getReturn() = object->getVariable(varName, kTypeFloat).getFloat();
}

void Functions::getLocalString(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	Common::UString varName = Common::UString::format("LOCAL_STRING_%i", ctx.getParams()[1].getInt());
	if (object)
		ctx.getReturn() = object->getVariable(varName, kTypeString).getString();
}

void Functions::getLocalObject(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	Common::UString varName = Common::UString::format("LOCAL_OBJECT_%i", ctx.getParams()[1].getInt());
	if (object)
		ctx.getReturn() = object->getVariable(varName, kTypeObject).getObject();
}

void Functions::setLocalInt(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	Common::UString varName = Common::UString::format("LOCAL_INT_%i", ctx.getParams()[1].getInt());
	if (object)
		object->setVariable(varName, ctx.getParams()[2].getInt());
}

void Functions::setLocalBool(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	Common::UString varName = Common::UString::format("LOCAL_BOOL_%i", ctx.getParams()[1].getInt());
	if (object)
		object->setVariable(varName, ctx.getParams()[2].getInt() != 0);
}

void Functions::setLocalFloat(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	Common::UString varName = Common::UString::format("LOCAL_FLOAT_%i", ctx.getParams()[1].getInt());
	if (object)
		object->setVariable(varName, ctx.getParams()[2].getFloat());
}

void Functions::setLocalString(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	Common::UString varName = Common::UString::format("LOCAL_STRING_%i", ctx.getParams()[1].getInt());
	if (object)
		object->setVariable(varName, ctx.getParams()[2].getString());
}

void Functions::setLocalObject(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	Common::UString varName = Common::UString::format("LOCAL_OBJECT_%i", ctx.getParams()[1].getInt());
	if (object)
		object->setVariable(varName, ctx.getParams()[2].getObject());
}

void Functions::getObjectType(Aurora::NWScript::FunctionContext &ctx) {
	Jade::Object *object = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));

	ctx.getReturn() = (int32_t) (object ? object->getType() : -1);
}

void Functions::getTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getTag();
}

void Functions::getArea(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Jade::Object *object = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (object)
		ctx.getReturn() = (Aurora::NWScript::Object *) object->getArea();
}

void Functions::getLocation(Aurora::NWScript::FunctionContext &ctx) {
	Jade::Object *object = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	ctx.getReturn() = object->getLocation();
}

void Functions::getPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().setVector(0.0f, 0.0f, 0.0f);

	Jade::Object *object = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	float x, y, z;
	object->getPosition(x, y, z);

	ctx.getReturn().setVector(x, y, z);
}

#define SQR(x) ((x) * (x))
void Functions::getDistanceToObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1.0f;

	Jade::Object *object1 = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));
	Jade::Object *object2 = Jade::ObjectContainer::toObject(ctx.getCaller());
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
		Waypoint *waypoint = Jade::ObjectContainer::toWaypoint(object);

		if (waypoint) {
			ctx.getReturn() = (Aurora::NWScript::Object *) waypoint;
			break;
		}
	}
}

void Functions::getNearestObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Jade::Object *target = Jade::ObjectContainer::toObject(getParamObject(ctx, 1));
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
		Jade::Object *nwnObject = Jade::ObjectContainer::toObject(object);
		if (!nwnObject || (nwnObject == target) || (nwnObject->getArea() != target->getArea()))
			continue;

		// Ignore invalid object types
		uint32_t objectType = (uint32_t) nwnObject->getType();
		if ((objectType == kObjectTypeInvalid) || (objectType >= kObjectTypeMAX))
			continue;

		// Convert the type into a bitfield value and check against the type bitfield

		if (type & (1 << (objectType - 1)))
			objects.push_back(nwnObject);
	}

	objects.sort(ObjectDistanceSort(*target));

	std::list<Object *>::iterator it = objects.begin();
	for (size_t n = 0; (n < nth) && (it != objects.end()); ++n)
		++it;

	if (it != objects.end())
		ctx.getReturn() = *it;
}

void Functions::playAnimation(Aurora::NWScript::FunctionContext &ctx) {
	Jade::Object *object = Jade::ObjectContainer::toObject(ctx.getCaller());
	if (!object)
		return;

	Animation animation = (Animation) ctx.getParams()[0].getInt();

	// TODO: speed, second
	// float speed   = ctx.getParams()[1].getFloat();
	// float seconds = ctx.getParams()[2].getFloat();

	object->playAnimation(animation);
}

void Functions::jumpToLocation(Aurora::NWScript::FunctionContext &ctx) {
	Jade::Object *object = Jade::ObjectContainer::toObject(ctx.getCaller());
	Jade::Location *moveTo = Jade::ObjectContainer::toLocation(ctx.getParams()[0].getEngineType());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);
}

void Functions::jumpToObject(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: walkStraightLineToPoint
	// bool walkStraightLineToPoint = ctx.getParams()[1].getInt() != 0;

	Jade::Object *object = Jade::ObjectContainer::toObject(ctx.getCaller());
	Jade::Object *moveTo = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);
}

void Functions::getObjectConversationResref(Aurora::NWScript::FunctionContext &ctx) {
	Jade::Object *object = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));
	ctx.getReturn() = object->getConversation();
}

void Functions::getPriorActivation(Aurora::NWScript::FunctionContext &ctx) {
	Jade::Object *object = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));
	ctx.getReturn() = (int32_t) (object->getLastTriggerer() != 0);
}

void Functions::setObjectNoCollide(Aurora::NWScript::FunctionContext &ctx) {
	Jade::Object *object = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	bool noCollide = ctx.getParams()[1].getInt() != 0;

	object->setNoCollide(noCollide);
}

} // End of namespace Jade

} // End of namespace Engines
