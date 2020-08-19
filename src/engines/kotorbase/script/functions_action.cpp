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
 *  Star Wars: Knights of the Old Republic engine functions assigning actions to objects.
 */

#include "external/glm/geometric.hpp"

#include "src/common/error.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotorbase/types.h"
#include "src/engines/kotorbase/door.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/objectcontainer.h"
#include "src/engines/kotorbase/game.h"
#include "src/engines/kotorbase/action.h"
#include "src/engines/kotorbase/creature.h"

#include "src/engines/kotorbase/script/functions.h"

namespace Engines {

namespace KotORBase {

void Functions::getCurrentAction(Aurora::NWScript::FunctionContext &ctx) {
	Creature *object = ObjectContainer::toCreature(ctx.getParams()[0].getObject());
	if (!object)
		throw Common::Exception("Functions::getCurrentAction(): Invalid object");

	const Action *action = object->getCurrentAction();
	if (!action)
		ctx.getReturn() = kActionQueueEmpty;
	else
		ctx.getReturn() = action->type;
}

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

void Functions::actionStartConversation(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &convName = ctx.getParams()[1].getString();
	_game->getModule().startConversation(convName, ctx.getCaller());
}

void Functions::actionOpenDoor(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	Door *door = ObjectContainer::toDoor(object);
	if (!door)
		throw Common::Exception("Functions::actionOpenDoor(): Object is not a door");

	door->open(0);
}

void Functions::actionCloseDoor(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	Door *door = ObjectContainer::toDoor(object);
	if (!door)
		throw Common::Exception("Functions::actionCloseDoor(): Object is not a door");

	door->close(0);
}

void Functions::actionMoveToObject(Aurora::NWScript::FunctionContext &ctx) {
	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller)
		throw Common::Exception("Functions::actionMoveToObject(): Invalid caller");

	Object *object = ObjectContainer::toObject(ctx.getParams()[0].getObject());
	if (!object)
		object = _game->getModule().getPartyLeader();

	float range = ctx.getParams()[2].getFloat();

	float x, y, z;
	object->getPosition(x, y, z);

	Action action(kActionMoveToPoint);
	action.range = range;
	action.location = glm::vec3(x, y, z);

	caller->addAction(action);
}

void Functions::actionFollowLeader(Aurora::NWScript::FunctionContext &ctx) {
	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller)
		throw Common::Exception("Functions::actionFollowLeader(): Invalid caller");

	Action action(kActionFollowLeader);
	action.range = 1.0f;

	caller->addAction(action);
}

void Functions::clearAllActions(Aurora::NWScript::FunctionContext &ctx) {
	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller)
		caller = _game->getModule().getPartyLeader();

	caller->clearActions();
}

} // End of namespace KotORBase

} // End of namespace Engines
