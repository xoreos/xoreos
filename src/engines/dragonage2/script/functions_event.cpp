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
 *  Dragon Age II engine functions handling events.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/variablecontainer.h"
#include "src/aurora/nwscript/enginetype.h"

#include "src/engines/dragonage2/objectcontainer.h"
#include "src/engines/dragonage2/event.h"

#include "src/engines/dragonage2/script/functions.h"

namespace Engines {

namespace DragonAge2 {

void Functions::getCurrentEvent(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (const Aurora::NWScript::EngineType *) 0;

	Aurora::NWScript::VariableContainer *env = ctx.getCurrentEnvironment();
	if (!env || !env->hasVariable("Event"))
		return;

	ctx.getReturn() = env->getVariable("Event");
}

void Functions::getEventType(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kEventTypeInvalid;

	const Event *event = DragonAge2::ObjectContainer::toEvent(ctx.getParams()[0].getEngineType());
	if (!event)
		return;

	ctx.getReturn() = (int32) event->getType();
}

void Functions::getEventCreator(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	const Event *event = DragonAge2::ObjectContainer::toEvent(ctx.getParams()[0].getEngineType());
	if (!event)
		return;

	ctx.getReturn() = event->getCreator();
}

void Functions::getEventTarget(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	const Event *event = DragonAge2::ObjectContainer::toEvent(ctx.getParams()[0].getEngineType());
	if (!event)
		return;

	ctx.getReturn() = event->getTarget();
}

} // End of namespace DragonAge2

} // End of namespace Engines
