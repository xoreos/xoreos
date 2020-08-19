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
 *  Neverwinter Nights engine functions handling conversations.
 */

#include "src/common/util.h"

#include "src/aurora/talkman.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/nwn/game.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/objectcontainer.h"
#include "src/engines/nwn/object.h"
#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/script/functions.h"

namespace Engines {

namespace NWN {

void Functions::speakString(Aurora::NWScript::FunctionContext &ctx) {
	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getCaller());

	if (object)
		object->speakString(ctx.getParams()[0].getString(), ctx.getParams()[1].getInt());
}

void Functions::speakStringByStrRef(Aurora::NWScript::FunctionContext &ctx) {
	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getCaller());
	if (!object)
		return;

	// TODO: Volume
	const uint32_t strRef = (uint32_t) ctx.getParams()[0].getInt();
	const uint32_t volume = (uint32_t) ctx.getParams()[1].getInt();

	object->speakString(TalkMan.getString(strRef).c_str(), volume);
}

void Functions::speakOneLinerConversation(Aurora::NWScript::FunctionContext &ctx) {
	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getCaller());
	if (!object)
		return;

	const Common::UString &dlg = ctx.getParams()[0].getString();
	NWN::Object *tokenTarget = NWN::ObjectContainer::toObject(getParamObject(ctx, 1));

	object->speakOneLiner(dlg, tokenTarget);
}

void Functions::beginConversation(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	// Get the script object parameters
	Aurora::NWScript::Object *obj1 = ctx.getCaller();
	Aurora::NWScript::Object *obj2 = getParamObject(ctx, 1);
	if (!obj2)
		obj2 = ctx.getTriggerer();
	if (!obj2)
		obj2 = _game->getModule().getPC();

	// Try to convert them to an NWN Creature and Object
	NWN::Creature *pc     = NWN::ObjectContainer::toPC(obj2);
	NWN::Object   *object = NWN::ObjectContainer::toObject(obj1);

	// Try the other way round, if necessary
	if (!pc || !object) {
		pc     = NWN::ObjectContainer::toPC(obj1);
		object = NWN::ObjectContainer::toObject(obj2);
	}

	// Fail
	if (!pc || !object)
		return;

	if (object->getPCSpeaker()) {
		if (object->getPCSpeaker() != pc) {
			Creature *otherPC = NWN::ObjectContainer::toPC(object->getPCSpeaker());

			warning("Functions::beginConversation(): "
			        "Object \"%s\" already in conversation with PC \"%s\"",
			        object->getTag().c_str(), otherPC ? otherPC->getName().c_str() : "");
			return;
		}
	}

	Common::UString conversation = ctx.getParams()[0].getString();
	if (conversation.empty())
		conversation = object->getConversation();

	ctx.getReturn() = _game->getModule().startConversation(conversation, *pc, *object);
}

void Functions::getPCSpeaker(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *speaker = 0;

	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getCaller());
	if (object)
		speaker = object->getPCSpeaker();

	ctx.getReturn() = speaker;
}

void Functions::getLastSpeaker(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	unimplementedFunction(ctx);
}

void Functions::isInConversation(Aurora::NWScript::FunctionContext &ctx) {
	NWN::Object *object = NWN::ObjectContainer::toObject(getParamObject(ctx, 0));

	ctx.getReturn() = object ? (object->getPCSpeaker() != 0) : 0;
}

void Functions::getListenPatternNumber(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1;

	unimplementedFunction(ctx);
}

} // End of namespace NWN

} // End of namespace Engines
