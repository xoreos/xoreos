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
 *  Neverwinter Nights engine functions assigning actions to objects.
 */

#include "src/common/error.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"

#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/ncsfile.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/game.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/objectcontainer.h"
#include "src/engines/nwn/door.h"

#include "src/engines/nwn/script/functions.h"

namespace Engines {

namespace NWN {

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

void Functions::executeScript(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString script = ctx.getParams()[0].getString();

	// Max resource name length is 16, and ExecuteScript should truncate accordingly
	script.truncate(16);

	if (!ResMan.hasResource(script, Aurora::kFileTypeNCS))
		return;

	Aurora::NWScript::Object *object = getParamObject(ctx, 1);
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
	Door *door = NWN::ObjectContainer::toDoor(getParamObject(ctx, 0));
	if (door)
		door->open(NWN::ObjectContainer::toObject(ctx.getCaller()));
}

void Functions::actionCloseDoor(Aurora::NWScript::FunctionContext &ctx) {
	Door *door = NWN::ObjectContainer::toDoor(getParamObject(ctx, 0));
	if (door)
		door->close(NWN::ObjectContainer::toObject(ctx.getCaller()));
}

void Functions::actionSpeakString(Aurora::NWScript::FunctionContext &ctx) {
	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getCaller());

	if (object)
		object->speakString(ctx.getParams()[0].getString(), ctx.getParams()[1].getInt());
}

void Functions::actionPlayAnimation(Aurora::NWScript::FunctionContext &ctx) {
	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getCaller());
	if (!object)
		return;

	Animation animation = (Animation) ctx.getParams()[0].getInt();

	// TODO: speed, second
	// float speed   = ctx.getParams()[1].getFloat();
	// float seconds = ctx.getParams()[2].getFloat();

	object->playAnimation(animation);
}

} // End of namespace NWN

} // End of namespace Engines
