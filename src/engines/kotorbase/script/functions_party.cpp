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

#include "src/engines/kotorbase/types.h"
#include "src/engines/kotorbase/object.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/objectcontainer.h"
#include "src/engines/kotorbase/game.h"
#include "src/engines/kotorbase/creature.h"

#include "src/engines/kotorbase/script/functions.h"

namespace Engines {

namespace KotORBase {

void Functions::isObjectPartyMember(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	Creature *creature = ObjectContainer::toCreature(object);

	if (!creature)
		throw Common::Exception("Functions::isObjectPartyMember(): object is not a creature");

	ctx.getReturn() = _game->getModule().isObjectPartyMember(creature);
}

void Functions::getPartyMemberByIndex(Aurora::NWScript::FunctionContext &ctx) {
	int index = ctx.getParams()[0].getInt();

	ctx.getReturn() = _game->getModule().getPartyMemberByIndex(index);
}

void Functions::showPartySelectionGUI(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &exitScript = ctx.getParams()[0].getString();
	int forceNPC1 = ctx.getParams()[1].getInt();
	int forceNPC2 = ctx.getParams()[2].getInt();

	_game->getModule().showPartySelectionGUI(forceNPC1, forceNPC2);
	ScriptContainer::runScript(exitScript, ctx.getCaller(), ctx.getTriggerer());
}

void Functions::isAvailableCreature(Aurora::NWScript::FunctionContext &ctx) {
	int slot = ctx.getParams()[0].getInt();

	ctx.getReturn() = _game->getModule().isAvailableCreature(slot);
}

void Functions::addAvailableNPCByTemplate(Aurora::NWScript::FunctionContext &ctx) {
	const int slot = ctx.getParams()[0].getInt();
	const Common::UString &templ = ctx.getParams()[1].getString();

	_game->getModule().addAvailableNPCByTemplate(slot, templ);
}

void Functions::setPartyLeader(Aurora::NWScript::FunctionContext &ctx) {
	int npc = ctx.getParams()[0].getInt();
	_game->getModule().setPartyLeader(npc);
}

} // End of namespace KotORBase

} // End of namespace Engines
