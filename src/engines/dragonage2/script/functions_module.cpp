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
 *  Dragon Age II engine functions operating on the current module/campaign.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/ncsfile.h"
#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/dragonage2/game.h"
#include "src/engines/dragonage2/campaigns.h"
#include "src/engines/dragonage2/campaign.h"

#include "src/engines/dragonage2/script/functions.h"

namespace Engines {

namespace DragonAge2 {

void Functions::getModule(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) _game->getCampaigns().getCurrentCampaign();
}

void Functions::getHero(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Campaign *campaign = _game->getCampaigns().getCurrentCampaign();
	if (!campaign)
		return;

	ctx.getReturn() = (Aurora::NWScript::Object *) campaign->getPC();
}

void Functions::doAreaTransition(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString newArea = ctx.getParams()[0].getString();
	const Common::UString waypoint = ctx.getParams()[1].getString();

	_game->getCampaigns().getCurrentCampaign()->movePC(newArea);

	// TODO: Move Characters to waypoint.

	ctx.getReturn() = true;
}

} // End of namespace DragonAge2

} // End of namespace Engines
