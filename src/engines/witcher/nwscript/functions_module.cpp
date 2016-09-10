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
 *  The Witcher engine functions operating on the current module.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/witcher/game.h"
#include "src/engines/witcher/module.h"
#include "src/engines/witcher/location.h"
#include "src/engines/witcher/objectcontainer.h"

#include "src/engines/witcher/nwscript/functions.h"

namespace Engines {

namespace Witcher {

void Functions::getModule(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) &_game->getModule();
}

void Functions::location(Aurora::NWScript::FunctionContext &ctx) {
	Location loc;

	loc.setArea  (Witcher::ObjectContainer::toArea(ctx.getParams()[0].getObject()));
	loc.setFacing(ctx.getParams()[2].getFloat());

	float x, y, z;
	ctx.getParams()[1].getVector(x, y, z);
	loc.setPosition(x, y, z);

	ctx.getReturn() = loc;
}

void Functions::getPositionFromLocation(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().setVector(0.0f, 0.0f, 0.0f);

	Location *loc = Witcher::ObjectContainer::toLocation(ctx.getParams()[0].getEngineType());
	if (!loc)
		return;

	float x, y, z;
	loc->getPosition(x, y, z);

	ctx.getReturn().setVector(x, y, z);
}

void Functions::startNewModule(Aurora::NWScript::FunctionContext &ctx) {
	std::vector<Common::UString> modules;
	_game->getModules(modules);

	for (std::vector<Common::UString>::const_iterator m = modules.begin(); m != modules.end(); ++m) {
		if (m->equalsIgnoreCase(ctx.getParams()[0].getString())) {
			_game->getModule().load(*m + ".mod");
			return;
		}
	}

	warning("Can't start module \"%s\": No such module", ctx.getParams()[0].getString().c_str());
}

void Functions::getFirstPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) _game->getModule().getPC();
}

void Functions::getNextPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;
}

} // End of namespace Witcher

} // End of namespace Engines
