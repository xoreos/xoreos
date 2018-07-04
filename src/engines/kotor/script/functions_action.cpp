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

#include "glm/geometric.hpp"

#include "src/common/error.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotor/types.h"
#include "src/engines/kotor/game.h"
#include "src/engines/kotor/module.h"
#include "src/engines/kotor/objectcontainer.h"
#include "src/engines/kotor/door.h"

#include "src/engines/kotor/script/functions.h"

namespace Engines {

namespace KotOR {

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

	uint32 delay = ctx.getParams()[0].getFloat() * 1000;

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

	Engines::KotOR::Object *object = ObjectContainer::toObject(ctx.getParams()[0].getObject());
	if (!object)
		object = _game->getModule().getPC();

	float range = ctx.getParams()[2].getFloat();

	float x, y, z;
	object->getPosition(x, y, z);

	Action action(kActionMoveToPoint);
	action.setRange(range);
	action.setPoint(x, y, z);

	caller->clearActionQueue();
	caller->enqueueAction(action);
}

void Functions::clearAllActions(Aurora::NWScript::FunctionContext &ctx) {
	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller)
		caller = _game->getModule().getPC();

	caller->clearActionQueue();
}

} // End of namespace KotOR

} // End of namespace Engines
