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

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/nwn/objectcontainer.h"
#include "src/engines/nwn/object.h"

#include "src/engines/nwn/script/functions.h"

namespace Engines {

namespace NWN {

void Functions::speakOneLinerConversation(Aurora::NWScript::FunctionContext &ctx) {
	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getCaller());
	if (!object)
		return;

	const Common::UString &dlg = ctx.getParams()[0].getString();
	NWN::Object *tokenTarget = NWN::ObjectContainer::toObject(getParamObject(ctx, 1));

	object->speakOneLiner(dlg, tokenTarget);
}

} // End of namespace NWN

} // End of namespace Engines
