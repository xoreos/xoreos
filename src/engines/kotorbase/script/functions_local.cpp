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
 *  Star Wars: Knights of the Old Republic engine functions managing
 *  local variables.
 */

#include "src/common/ustring.h"
#include "src/common/strutil.h"

#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/variable.h"
#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/object.h"

#include "src/engines/kotorbase/script/functions.h"

namespace Engines {

namespace KotORBase {

void Functions::getLocalBoolean(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (object) {
		Common::UString name = Common::composeString(ctx.getParams()[1].getInt());
		Aurora::NWScript::Variable &var = object->getVariable(name, Aurora::NWScript::kTypeInt);
		ctx.getReturn() = var.getInt() != 0;
	}
}

void Functions::setLocalBoolean(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (object) {
		Common::UString name = Common::composeString(ctx.getParams()[1].getInt());
		Aurora::NWScript::Variable &var = object->getVariable(name, Aurora::NWScript::kTypeInt);
		var = ctx.getParams()[2].getInt();
	}
}

void Functions::getLocalNumber(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (object) {
		Common::UString name = Common::composeString(ctx.getParams()[1].getInt());
		Aurora::NWScript::Variable &var = object->getVariable(name, Aurora::NWScript::kTypeInt);
		ctx.getReturn() = var.getInt();
	}
}

void Functions::setLocalNumber(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (object) {
		Common::UString name = Common::composeString(ctx.getParams()[1].getInt());
		Aurora::NWScript::Variable &var = object->getVariable(name, Aurora::NWScript::kTypeInt);
		var = ctx.getParams()[2].getInt();
	}
}

} // End of namespace KotORBase

} // End of namespace Engines
