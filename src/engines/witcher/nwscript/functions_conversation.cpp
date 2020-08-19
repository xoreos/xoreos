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
 *  The Witcher engine functions handling conversations.
 */

#include "src/common/util.h"

#include "src/aurora/talkman.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/witcher/game.h"
#include "src/engines/witcher/module.h"
#include "src/engines/witcher/objectcontainer.h"
#include "src/engines/witcher/object.h"

#include "src/engines/witcher/nwscript/functions.h"

namespace Engines {

namespace Witcher {

void Functions::speakString(Aurora::NWScript::FunctionContext &ctx) {
	Witcher::Object *object = Witcher::ObjectContainer::toObject(ctx.getCaller());

	if (object)
		object->speakString(ctx.getParams()[0].getString(), ctx.getParams()[1].getInt());
}

void Functions::speakStringByStrRef(Aurora::NWScript::FunctionContext &ctx) {
	Witcher::Object *object = Witcher::ObjectContainer::toObject(ctx.getCaller());
	if (!object)
		return;

	// TODO: Volume
	const uint32_t strRef = (uint32_t) ctx.getParams()[0].getInt();
	const uint32_t volume = (uint32_t) ctx.getParams()[1].getInt();

	object->speakString(TalkMan.getString(strRef).c_str(), volume);
}

void Functions::speakOneLinerConversation(Aurora::NWScript::FunctionContext &ctx) {
	Witcher::Object *object = Witcher::ObjectContainer::toObject(ctx.getCaller());
	if (!object)
		return;

	const Common::UString &dlg = ctx.getParams()[0].getString();
	Witcher::Object *tokenTarget = Witcher::ObjectContainer::toObject(getParamObject(ctx, 1));

	object->speakOneLiner(dlg, tokenTarget);
}

} // End of namespace Witcher

} // End of namespace Engines
