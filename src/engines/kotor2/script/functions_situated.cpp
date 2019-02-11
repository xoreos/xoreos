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
 *  Star Wars: Knights of the Old Republic II - The Sith Lords engine functions messing with situated objects.
 */

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotorbase/object.h"

#include "src/engines/kotor2/objectcontainer.h"
#include "src/engines/kotor2/situated.h"

#include "src/engines/kotor2/script/functions.h"

namespace Engines {

namespace KotOR2 {

void Functions::getLocked(Aurora::NWScript::FunctionContext &ctx) {
	Situated *situated = KotOR2::ObjectContainer::toSituated(getParamObject(ctx, 0));

	ctx.getReturn() = situated ? situated->isLocked() : 0;
}

void Functions::setLocked(Aurora::NWScript::FunctionContext &ctx) {
	Situated *situated = KotOR2::ObjectContainer::toSituated(getParamObject(ctx, 0));
	if (situated)
		situated->setLocked(ctx.getParams()[1].getInt() != 0);
}

void Functions::getIsOpen(Aurora::NWScript::FunctionContext &ctx) {
	Situated *situated = KotOR2::ObjectContainer::toSituated(getParamObject(ctx, 0));

	ctx.getReturn() = situated ? situated->isOpen() : 0;
}

void Functions::getLastOpenedBy(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Situated *situated = KotOR2::ObjectContainer::toSituated(ctx.getCaller());
	if (!situated)
		return;

	ctx.getReturn() = (Aurora::NWScript::Object *) situated->getLastOpenedBy();
}

void Functions::getLastClosedBy(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Situated *situated = KotOR2::ObjectContainer::toSituated(ctx.getCaller());
	if (!situated)
		return;

	ctx.getReturn() = (Aurora::NWScript::Object *) situated->getLastClosedBy();
}

void Functions::getLastUsedBy(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Situated *situated = KotOR2::ObjectContainer::toSituated(ctx.getCaller());
	if (!situated)
		return;

	ctx.getReturn() = (Aurora::NWScript::Object *) situated->getLastUsedBy();
}

} // End of namespace KotOR2

} // End of namespace Engines
