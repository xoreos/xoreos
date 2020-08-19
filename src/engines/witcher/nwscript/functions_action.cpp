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
 *  The Witcher engine functions assigning actions to objects.
 */

#include "src/common/error.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"

#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/ncsfile.h"
#include "src/aurora/nwscript/util.h"

#include "src/engines/witcher/types.h"
#include "src/engines/witcher/game.h"
#include "src/engines/witcher/module.h"
#include "src/engines/witcher/objectcontainer.h"
#include "src/engines/witcher/door.h"
#include "src/engines/witcher/creature.h"

#include "src/engines/witcher/nwscript/functions.h"

namespace Engines {

namespace Witcher {

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
	Common::UString script = ctx.getParams()[0].getString();

	// Max resource name length is 16, and ExecuteScript should truncate accordingly
	script.truncate(16);

	if (!ResMan.hasResource(script, Aurora::kFileTypeNCS))
		return;

	Aurora::NWScript::Object *object = getParamObject(ctx, 1);
	try {
		Aurora::NWScript::NCSFile ncs(script);

		// Let the child script inherit the environment of this parent script
		Aurora::NWScript::VariableContainer *env = ctx.getCurrentEnvironment();
		if (env)
			ncs.setEnvironment(*env);

		ncs.run(object);
	} catch (...) {
		Common::exceptionDispatcherWarning("Failed ExecuteScript(\"%s\", %s)",
		                                   script.c_str(), Aurora::NWScript::formatTag(object).c_str());
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
	Door *door = Witcher::ObjectContainer::toDoor(getParamObject(ctx, 0));
	if (door)
		door->open(Witcher::ObjectContainer::toObject(ctx.getCaller()));
}

void Functions::actionCloseDoor(Aurora::NWScript::FunctionContext &ctx) {
	Door *door = Witcher::ObjectContainer::toDoor(getParamObject(ctx, 0));
	if (door)
		door->close(Witcher::ObjectContainer::toObject(ctx.getCaller()));
}

void Functions::actionSpeakString(Aurora::NWScript::FunctionContext &ctx) {
	Witcher::Object *object = Witcher::ObjectContainer::toObject(ctx.getCaller());

	if (object)
		object->speakString(ctx.getParams()[0].getString(), ctx.getParams()[1].getInt());
}

void Functions::actionJumpToLocation(Aurora::NWScript::FunctionContext &ctx) {
	Witcher::Object   *object = Witcher::ObjectContainer::toObject(ctx.getCaller());
	Witcher::Location *moveTo = Witcher::ObjectContainer::toLocation(ctx.getParams()[0].getEngineType());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);
}

void Functions::actionMoveToLocation(Aurora::NWScript::FunctionContext &ctx) {
	Witcher::Object   *object = Witcher::ObjectContainer::toObject(ctx.getCaller());
	Witcher::Location *moveTo = Witcher::ObjectContainer::toLocation(ctx.getParams()[0].getEngineType());

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

	Witcher::Object *object = Witcher::ObjectContainer::toObject(ctx.getCaller());
	Witcher::Object *moveTo = Witcher::ObjectContainer::toObject(getParamObject(ctx, 0));

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);
}

void Functions::actionMoveToObject(Aurora::NWScript::FunctionContext &ctx) {
	Witcher::Object *object = Witcher::ObjectContainer::toObject(ctx.getCaller());
	Witcher::Object *moveTo = Witcher::ObjectContainer::toObject(getParamObject(ctx, 0));

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);

	unimplementedFunction(ctx);
}

} // End of namespace Witcher

} // End of namespace Engines
