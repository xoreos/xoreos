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

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/nwn/game.h"
#include "src/engines/nwn/module.h"

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

// TODO: These functions need to assign an action, instead of simply delaying the script a bit.

void Functions::actionDoCommand(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString script = ctx.getScriptName();
	if (script.empty())
		throw Common::Exception("Functions::actionDoCommand(): Script needed");

	const Aurora::NWScript::ScriptState &state = ctx.getParams()[0].getScriptState();

	_game->getModule().delayScript(script, state, ctx.getCaller(), ctx.getTriggerer(), 0);
}

} // End of namespace NWN

} // End of namespace Engines
