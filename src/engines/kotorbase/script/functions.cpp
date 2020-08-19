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
 *  Engine functions for KotOR games.
 */

#include <cassert>
#include <cstdlib>

#include "src/common/util.h"
#include "src/common/random.h"

#include "src/aurora/nwscript/functionman.h"
#include "src/aurora/nwscript/util.h"

#include "src/engines/kotorbase/types.h"
#include "src/engines/kotorbase/object.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/area.h"
#include "src/engines/kotorbase/objectcontainer.h"
#include "src/engines/kotorbase/game.h"

#include "src/engines/kotorbase/script/functions.h"

namespace Engines {

namespace KotORBase {

Functions::Functions(Game &game) : _game(&game) {
}

Functions::~Functions() {
	FunctionMan.clear();
}

void Functions::getRunScriptVar(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = _game->getModule().getRunScriptVar();
}

void Functions::unimplementedFunction(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: %s %s(%s)", Aurora::NWScript::formatType(ctx.getReturn().getType()).c_str(),
	                           ctx.getName().c_str(), Aurora::NWScript::formatParams(ctx).c_str());
}

void Functions::executeScript(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &script = ctx.getParams()[0].getString();

	Object *target = ObjectContainer::toObject(ctx.getParams()[1].getObject());
	if (!target)
		throw Common::Exception("Functions::executeScript(): Invalid target");

	_game->getModule().setRunScriptVar(ctx.getParams()[2].getInt());

	target->runScript(script, target, ctx.getCaller());
}

int32_t Functions::getRandom(int min, int max, int32_t n) {
	if (n < 1)
		n = 1;

	int32_t r = 0;

	while (n-- > 0)
		r += RNG.getNext(min, max + 1);

	return r;
}

Common::UString Functions::formatFloat(float f, int width, int decimals) {
	return Common::UString::format("%*.*f", width, decimals, f);
}

Aurora::NWScript::Object *Functions::getParamObject(const Aurora::NWScript::FunctionContext &ctx, size_t n) {
	Object *object = ObjectContainer::toObject(ctx.getParams()[n].getObject());
	if (!object || (object->getType() == kObjectTypeInvalid))
		return 0;

	if (object->getType() == kObjectTypeSelf)
		return ctx.getCaller();

	return object;
}

void Functions::jumpTo(KotORBase::Object *object, float x, float y, float z) {
	object->setPosition(x, y, z);
}

} // End of namespace KotORBase

} // End of namespace Engines
