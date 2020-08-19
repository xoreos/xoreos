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
 *  Jade Empire engine functions handling events.
 */

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/nwscript/enginetype.h"
#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/variablecontainer.h"

#include "src/engines/jade/event.h"
#include "src/engines/jade/object.h"
#include "src/engines/jade/objectcontainer.h"

#include "src/engines/jade/script/functions.h"

namespace Engines {

namespace Jade {

void Functions::eventUserDefined(Aurora::NWScript::FunctionContext &ctx) {
	int32_t eventNumber = ctx.getParams()[0].getInt();

	ctx.getReturn() = Event(kScriptOnUserdefined, eventNumber);
}

void Functions::signalEvent(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::VariableContainer *env = ctx.getCurrentEnvironment();
	if (!env)
		return;

	Object *object = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));
	const Event *event   = Jade::ObjectContainer::toEvent(ctx.getParams()[1].getEngineType());

	if (event->getScript() == kScriptOnUserdefined)
		env->setVariable("EVENT_USER_DEFINED_" + object->getTag(), event->getEventNumber());
	object->runScript(event->getScript(), object, object);
}

void Functions::getUserDefinedEventNumber(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32_t) 0;

	Object *object = Jade::ObjectContainer::toObject(ctx.getCaller());

	Aurora::NWScript::VariableContainer *env = ctx.getCurrentEnvironment();
	if (!env || !env->hasVariable("EVENT_USER_DEFINED_" + object->getTag()))
		return;

	ctx.getReturn() = env->getVariable("EVENT_USER_DEFINED_" + object->getTag()).getInt();
}

void Functions::setUserDefinedEventNumber(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Aurora::NWScript::kTypeVoid;

	Aurora::NWScript::VariableContainer *env = ctx.getCurrentEnvironment();
	if (!env)
		return;

	Object *object = Jade::ObjectContainer::toObject(getParamObject(ctx, 0));
	int32_t eventNumber = ctx.getParams()[1].getInt();

	env->setVariable("EVENT_USER_DEFINED_" + object->getTag(), eventNumber);
}

} // End of namespace Jade

} // End of namespace Engines
