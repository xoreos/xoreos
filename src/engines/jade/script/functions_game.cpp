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
 *  Jade Empire engine functions operating on the current game.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/jade/game.h"

#include "src/engines/jade/script/functions.h"

namespace Engines {

namespace Jade {

using Aurora::NWScript::kTypeInt;
using Aurora::NWScript::kTypeFloat;
using Aurora::NWScript::kTypeString;

void Functions::getGlobalInt(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = _game->getVariable(ctx.getParams()[0].getString(), kTypeInt).getInt();
}

void Functions::getGlobalBool(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = _game->getVariable(ctx.getParams()[0].getString(), kTypeInt).getInt() != 0;
}

void Functions::getGlobalString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = _game->getVariable(ctx.getParams()[0].getString(), kTypeString).getString();
}

void Functions::getGlobalFloat(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = _game->getVariable(ctx.getParams()[0].getString(), kTypeFloat).getFloat();
}

void Functions::setGlobalInt(Aurora::NWScript::FunctionContext &ctx) {
	_game->setVariable(ctx.getParams()[0].getString(), ctx.getParams()[1].getInt());
}

void Functions::setGlobalBool(Aurora::NWScript::FunctionContext &ctx) {
	_game->setVariable(ctx.getParams()[0].getString(), ctx.getParams()[1].getInt() != 0);
}

void Functions::setGlobalString(Aurora::NWScript::FunctionContext &ctx) {
	_game->setVariable(ctx.getParams()[0].getString(), ctx.getParams()[1].getString());
}

void Functions::setGlobalFloat(Aurora::NWScript::FunctionContext &ctx) {
	_game->setVariable(ctx.getParams()[0].getString(), ctx.getParams()[1].getFloat());
}

} // End of namespace Jade

} // End of namespace Engines
