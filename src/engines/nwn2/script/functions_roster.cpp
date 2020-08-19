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
 *  Neverwinter Nights 2 engine functions messing with the roster.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/game.h"
#include "src/engines/nwn2/module.h"
#include "src/engines/nwn2/roster.h"

#include "src/engines/nwn2/script/functions.h"

namespace Engines {

namespace NWN2 {

void Functions::getRosterNPCPartyLimit(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32_t) _game->getModule().getRoster().getRosterNPCPartyLimit();
}

void Functions::setRosterNPCPartyLimit(Aurora::NWScript::FunctionContext &ctx) {
	const uint32_t limit = (uint32_t) ctx.getParams()[0].getInt();
	_game->getModule().getRoster().setRosterNPCPartyLimit(limit);
}

void Functions::addRosterMemberByTemplate(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &name = ctx.getParams()[0].getString();
	const Common::UString &cTemplate = ctx.getParams()[1].getString();
	ctx.getReturn() = _game->getModule().getRoster().addRosterMemberByTemplate(name, cTemplate);
}

void Functions::getFirstRosterMember(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = _game->getModule().getRoster().getFirstRosterMember();
}

void Functions::getNextRosterMember(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = _game->getModule().getRoster().getNextRosterMember();
}

void Functions::getIsRosterMemberAvailable(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &name = ctx.getParams()[0].getString();
	ctx.getReturn() = _game->getModule().getRoster().getIsRosterMemberAvailable(name);
}

void Functions::getIsRosterMemberCampaignNPC(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &name = ctx.getParams()[0].getString();
	ctx.getReturn() = _game->getModule().getRoster().getIsRosterMemberCampaignNPC(name);
}

void Functions::getIsRosterMemberSelectable(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &name = ctx.getParams()[0].getString();
	ctx.getReturn() = _game->getModule().getRoster().getIsRosterMemberSelectable(name);
}

void Functions::getPartyName(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = _game->getModule().getRoster().getPartyName();
}

void Functions::getPartyMotto(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = _game->getModule().getRoster().getPartyMotto();
}

void Functions::setIsRosterMemberAvailable(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &name = ctx.getParams()[0].getString();
	const bool available = ctx.getParams()[1].getInt() != 0;
	ctx.getReturn() = _game->getModule().getRoster().setIsRosterMemberAvailable(name, available);
}

void Functions::setIsRosterMemberCampaignNPC(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &name = ctx.getParams()[0].getString();
	const bool campaignNPC = ctx.getParams()[1].getInt() != 0;
	ctx.getReturn() = _game->getModule().getRoster().setIsRosterMemberCampaignNPC(name, campaignNPC);
}

void Functions::setIsRosterMemberSelectable(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &name = ctx.getParams()[0].getString();
	const bool selectable = ctx.getParams()[1].getInt() != 0;
	ctx.getReturn() = _game->getModule().getRoster().setIsRosterMemberSelectable(name, selectable);
}


} // End of namespace NWN2

} // End of namespace Engines
