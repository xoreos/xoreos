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
 *  Jade Empire engine functions assigning actions to objects.
 */

#include "src/common/error.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"

#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/ncsfile.h"

#include "src/engines/jade/types.h"
#include "src/engines/jade/creature.h"
#include "src/engines/jade/game.h"
#include "src/engines/jade/module.h"
#include "src/engines/jade/object.h"
#include "src/engines/jade/objectcontainer.h"
#include "src/engines/jade/placeable.h"

#include "src/engines/jade/script/functions.h"

namespace Engines {

namespace Jade {

void Functions::assignCommand(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString script = ctx.getScriptName();
	if (script.empty())
		throw Common::Exception("Functions::assignCommand(): Script needed");

	const Aurora::NWScript::ScriptState &state = ctx.getParams()[1].getScriptState();

	_game->getModule().delayScript(script, state, getParamObject(ctx, 0), ctx.getTriggerer(), 0);
}

void Functions::delayCommand(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString script = ctx.getScriptName();
	if (script.empty())
		throw Common::Exception("Functions::assignCommand(): Script needed");

	uint32_t delay = ctx.getParams()[0].getFloat() * 1000;

	const Aurora::NWScript::ScriptState &state = ctx.getParams()[1].getScriptState();

	_game->getModule().delayScript(script, state, ctx.getCaller(), ctx.getTriggerer(), delay);
}

void Functions::executeScript(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString script = ctx.getParams()[1].getString();

	// Max resource name length is 16, and ExecuteScript should truncate accordingly
	script.truncate(16);

	if (!ResMan.hasResource(script, Aurora::kFileTypeNCS))
		return;

	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	try {
		Aurora::NWScript::NCSFile ncs(script);

		ncs.run(object);
	} catch (Common::Exception &e) {
		e.add("Failed ExecuteScript(\"%s\", %s)", script.c_str(), formatTag(object).c_str());

		Common::printException(e, "WARNING: ");
	}
}

// TODO: These functions need to assign an action, instead of simply delaying the script a bit.

void Functions::actionDoCommand(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString script = ctx.getScriptName();
	if (script.empty())
		throw Common::Exception("Functions::actionDoCommand(): Script needed");

	const Aurora::NWScript::ScriptState &state = ctx.getParams()[0].getScriptState();

	_game->getModule().delayScript(script, state, ctx.getCaller(), ctx.getTriggerer(), 0);
}

void Functions::actionOpenDoor(Aurora::NWScript::FunctionContext &ctx) {
	Placeable *door = Jade::ObjectContainer::toPlaceable(getParamObject(ctx, 0));
	if (door)
		door->open(Jade::ObjectContainer::toObject(ctx.getCaller()));
}

void Functions::actionCloseDoor(Aurora::NWScript::FunctionContext &ctx) {
	Placeable *door = Jade::ObjectContainer::toPlaceable(getParamObject(ctx, 0));
	if (door)
		door->close(Jade::ObjectContainer::toObject(ctx.getCaller()));
}

void Functions::actionSpeakStringByStrRef(Aurora::NWScript::FunctionContext &ctx) {
	Jade::Object *object = Jade::ObjectContainer::toObject(ctx.getCaller());

	if (object)
		object->speakString(ctx.getParams()[0].getInt());
}

void Functions::actionStartConversation(Aurora::NWScript::FunctionContext &ctx) {
	Jade::Object *source = Jade::ObjectContainer::toObject(ctx.getCaller());
	Jade::Object *target = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!source || !target)
		return;

	Creature *pc = Jade::ObjectContainer::toPC(target);
	if (!pc) {
		warning("TODO: ActionStartConversation: Non-PC target \"%s\"", target->getTag().c_str());
		return;
	}

	if (source->getPCSpeaker()) {
		if (source->getPCSpeaker() != pc) {
			Creature *otherPC = Jade::ObjectContainer::toPC(source->getPCSpeaker());

			warning("Functions::actionStartConversation(): "
			        "Object \"%s\" already in conversation with PC \"%s\"",
			        source->getTag().c_str(), otherPC ? otherPC->getName().c_str() : "");
			return;
		}
	}

	Common::UString conversation = ctx.getParams()[1].getString();
	if (conversation.empty())
		conversation = source->getConversation();

	/* TODO
	const int32_t range = ctx.getParams()[2].getInt();
	const int32_t delay = ctx.getParams()[3].getInt();
	const bool ignoreLOS = ctx.getParams()[4].getInt() != 0;
	*/
	const bool noWidescreen = ctx.getParams()[5].getInt() != 0;
	const bool resetZoom = ctx.getParams()[6].getInt() != 0;

	_game->getModule().startConversation(conversation, *pc, *source, noWidescreen, resetZoom);
}

void Functions::actionPlayAnimation(Aurora::NWScript::FunctionContext &ctx) {
	Jade::Object *object = Jade::ObjectContainer::toObject(ctx.getCaller());
	if (!object)
		return;

	Animation animation = (Animation) ctx.getParams()[0].getInt();

	// TODO: speed, second
	// float speed   = ctx.getParams()[1].getFloat();
	// float seconds = ctx.getParams()[2].getFloat();

	object->playAnimation(animation);
}

void Functions::actionJumpToLocation(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: walkStraightLineToPoint
	// bool walkStraightLineToPoint = ctx.getParams()[1].getInt() != 0;

	Jade::Object   *object = Jade::ObjectContainer::toObject(ctx.getCaller());
	Jade::Location *moveTo = Jade::ObjectContainer::toLocation(ctx.getParams()[0].getEngineType());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);
}

void Functions::actionMoveToLocation(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: runType
	// int32_t runType = ctx.getParams()[1].getInt();
	// int32_t moveAnim = ctx.getParams()[2].getInt();

	Jade::Object   *object = Jade::ObjectContainer::toObject(ctx.getCaller());
	Jade::Location *moveTo = Jade::ObjectContainer::toLocation(ctx.getParams()[0].getEngineType());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);

	unimplementedFunction(ctx);
}

void Functions::actionJumpToObject(Aurora::NWScript::FunctionContext &ctx) {
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

void Functions::actionMoveToObject(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: runType and range
	// int32_t runType = ctx.getParams()[1].getInt();
	// int32_t range = ctx.getParams()[2].getInt();
	// int32_t moveAnim = ctx.getParams()[3].getInt();

	Jade::Object *object = Jade::ObjectContainer::toObject(ctx.getCaller());
	Jade::Object *moveTo = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);

	unimplementedFunction(ctx);
}

} // End of namespace Jade

} // End of namespace Engines
