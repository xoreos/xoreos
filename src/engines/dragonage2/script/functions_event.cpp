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
#include "src/common/error.h"

#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/variablecontainer.h"
#include "src/aurora/nwscript/enginetype.h"

#include "src/engines/dragonage2/objectcontainer.h"
#include "src/engines/dragonage2/event.h"

#include "src/engines/dragonage2/script/functions.h"
#include "src/engines/dragonage2/script/container.h"

namespace Engines {

namespace DragonAge2 {

void Functions::getCurrentEvent(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (const Aurora::NWScript::EngineType *) 0;

	Aurora::NWScript::VariableContainer *env = ctx.getCurrentEnvironment();
	if (!env || !env->hasVariable("Event"))
		return;

	ctx.getReturn() = env->getVariable("Event");
}

void Functions::getCurrentEventType(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32_t) kEventTypeInvalid;

	Aurora::NWScript::VariableContainer *env = ctx.getCurrentEnvironment();
	if (!env || !env->hasVariable("Event"))
		return;

	const Event *event = DragonAge2::ObjectContainer::toEvent(env->getVariable("Event").getEngineType());
	if (!event)
		return;

	ctx.getReturn() = (int32_t) event->getType();
}

void Functions::isEventValid(Aurora::NWScript::FunctionContext &ctx) {
	const Event *event = DragonAge2::ObjectContainer::toEvent(ctx.getParams()[0].getEngineType());

	ctx.getReturn() = event && (event->getType() != kEventTypeInvalid);
}

void Functions::getEventType(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32_t) kEventTypeInvalid;

	const Event *event = DragonAge2::ObjectContainer::toEvent(ctx.getParams()[0].getEngineType());
	if (!event)
		return;

	ctx.getReturn() = (int32_t) event->getType();
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

void Functions::setEventType(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0].getEngineType();

	Event *event = DragonAge2::ObjectContainer::toEvent(ctx.getReturn().getEngineType());
	if (!event)
		return;

	event->setType((EventType) ctx.getParams()[1].getInt());
}

void Functions::setEventCreator(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0].getEngineType();

	Event *event = DragonAge2::ObjectContainer::toEvent(ctx.getReturn().getEngineType());
	if (!event)
		return;

	event->setCreator(ctx.getParams()[1].getObject());
}

void Functions::getEventTypeRef(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32_t) kEventTypeInvalid;

	const Aurora::NWScript::Variable *variable = ctx.getParams()[0].getReference();
	if (!variable)
		return;

	const Event *event = DragonAge2::ObjectContainer::toEvent(variable->getEngineType());
	if (!event)
		return;

	ctx.getReturn() = (int32_t) event->getType();
}

void Functions::getEventCreatorRef(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	const Aurora::NWScript::Variable *variable = ctx.getParams()[0].getReference();
	if (!variable)
		return;

	const Event *event = DragonAge2::ObjectContainer::toEvent(variable->getEngineType());
	if (!event)
		return;

	ctx.getReturn() = event->getCreator();
}

void Functions::getEventTargetRef(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	const Aurora::NWScript::Variable *variable = ctx.getParams()[0].getReference();
	if (!variable)
		return;

	const Event *event = DragonAge2::ObjectContainer::toEvent(variable->getEngineType());
	if (!event)
		return;

	ctx.getReturn() = event->getCreator();
}

void Functions::setEventTypeRef(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Variable *variable = ctx.getParams()[0].getReference();
	if (!variable)
		return;

	Event *event = DragonAge2::ObjectContainer::toEvent(variable->getEngineType());
	if (!event)
		return;

	event->setType((EventType) ctx.getParams()[1].getInt());
}

void Functions::setEventCreatorRef(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Variable *variable = ctx.getParams()[0].getReference();
	if (!variable)
		return;

	Event *event = DragonAge2::ObjectContainer::toEvent(variable->getEngineType());
	if (!event)
		return;

	event->setCreator(ctx.getParams()[1].getObject());
}

void Functions::handleEvent(Aurora::NWScript::FunctionContext &ctx) {
	Event invalidEvent;

	Event *event = DragonAge2::ObjectContainer::toEvent(ctx.getParams()[0].getEngineType());
	if (!event)
		event = &invalidEvent;

	// TODO: According to the Dragon Age wiki, "The maximum level of event rerouteing is 8".

	DragonAge2::ScriptContainer::runScript(ctx.getParams()[1].getString(), *event);
}

void Functions::handleEventRef(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Variable *variable = ctx.getParams()[0].getReference();
	if (!variable)
		throw Common::Exception("Functions::handleEventRef(): Empty reference");

	Event *event = DragonAge2::ObjectContainer::toEvent(variable->getEngineType());
	if (!event)
		throw Common::Exception("Functions::handleEventRef(): Reference is not an event");

	// TODO: Maximum level of event rerouteing?

	DragonAge2::ScriptContainer::runScript(ctx.getParams()[1].getString(), *event);
}

} // End of namespace DragonAge2

} // End of namespace Engines
