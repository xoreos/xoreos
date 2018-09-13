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
 *  Star Wars: Knights of the Old Republic engine functions handling the party.
 */

#include "src/common/util.h"
#include "src/common/scopedptr.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotor/types.h"
#include "src/engines/kotor/game.h"
#include "src/engines/kotor/module.h"
#include "src/engines/kotor/objectcontainer.h"
#include "src/engines/kotor/object.h"

#include "src/engines/kotor/script/functions.h"

namespace Engines {

namespace KotOR {

void Functions::isObjectPartyMember(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	Creature *creature = ObjectContainer::toCreature(object);

	if (!creature)
		throw Common::Exception("Functions::isObjectPartyMember(): object is not a creature");

	ctx.getReturn() = _game->getModule().isObjectPartyMember(creature);
}

void Functions::getPartyMemberByIndex(Aurora::NWScript::FunctionContext &ctx) {
	int index = ctx.getParams()[0].getInt();

	ctx.getReturn() = _game->getModule().getPartyMember(index);
}

void Functions::showPartySelectionGUI(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &exitScript = ctx.getParams()[0].getString();
	int forceNPC1 = ctx.getParams()[1].getInt();
	int forceNPC2 = ctx.getParams()[2].getInt();

	_game->getModule().showPartySelectionGUI(exitScript, forceNPC1, forceNPC2);
}

void Functions::isAvailableCreature(Aurora::NWScript::FunctionContext &ctx) {
	int slot = ctx.getParams()[0].getInt();

	ctx.getReturn() = _game->getModule().isAvailableCreature(slot);
}

void Functions::addAvailableNPCByTemplate(Aurora::NWScript::FunctionContext &ctx) {
	const int slot = ctx.getParams()[0].getInt();
	const Common::UString &templ = ctx.getParams()[1].getString();

	_game->getModule().addAvailablePartyMember(slot, templ);
}

} // End of namespace KotOR

} // End of namespace Engines
