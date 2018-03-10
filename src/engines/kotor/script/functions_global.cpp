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
 *  Star Wars: Knights of the Old Republic engine functions managing global variables.
 */

#include "src/common/ustring.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotor/game.h"
#include "src/engines/kotor/module.h"

#include "src/engines/kotor/script/functions.h"

namespace Engines {

namespace KotOR {

void Functions::getGlobalNumber(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString id = ctx.getParams()[0].getString();

	ctx.getReturn() = _game->getModule().getGlobalNumber(id);
}

void Functions::setGlobalNumber(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString id = ctx.getParams()[0].getString();
	int value = ctx.getParams()[1].getInt();

	_game->getModule().setGlobalNumber(id, value);
}

} // End of namespace KotOR

} // End of namespace Engines
