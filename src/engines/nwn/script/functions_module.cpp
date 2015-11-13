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
 *  Neverwinter Nights engine functions operating on the current module.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn/game.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/location.h"
#include "src/engines/nwn/objectcontainer.h"

#include "src/engines/nwn/script/functions.h"

namespace Engines {

namespace NWN {

void Functions::getModule(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) &_game->getModule();
}

void Functions::location(Aurora::NWScript::FunctionContext &ctx) {
	Location loc;

	loc.setArea  (NWN::ObjectContainer::toArea(ctx.getParams()[0].getObject()));
	loc.setFacing(ctx.getParams()[2].getFloat());

	float x, y, z;
	ctx.getParams()[1].getVector(x, y, z);
	loc.setPosition(x, y, z);

	ctx.getReturn() = loc;
}

void Functions::getPositionFromLocation(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().setVector(0.0f, 0.0f, 0.0f);

	Location *loc = NWN::ObjectContainer::toLocation(ctx.getParams()[0].getEngineType());
	if (!loc)
		return;

	float x, y, z;
	loc->getPosition(x, y, z);

	ctx.getReturn().setVector(x, y, z);
}

void Functions::startNewModule(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString mod = ctx.getParams()[0].getString();

	if (!Game::hasModule(mod)) {
		warning("Can't start module \"%s\": No such module", mod.c_str());
		return;
	}

	_game->getModule().load(mod);
}

void Functions::endGame(Aurora::NWScript::FunctionContext &ctx) {
	_game->getModule().exit();

	const Common::UString video = ctx.getParams()[0].getString();
	if (!video.empty())
		playVideo(video);

	playVideo("credits");
}

void Functions::getFirstPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) _game->getModule().getPC();
}

void Functions::getNextPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;
}

} // End of namespace NWN

} // End of namespace Engines
