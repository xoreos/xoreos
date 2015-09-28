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
 *  Dragon Age: Origins engine functions messing with creatures.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/dragonage/game.h"
#include "src/engines/dragonage/campaigns.h"
#include "src/engines/dragonage/campaign.h"
#include "src/engines/dragonage/creature.h"

#include "src/engines/dragonage/script/functions.h"

namespace Engines {

namespace DragonAge {

void Functions::isHero(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = false;

	Campaign *campaign = _game->getCampaigns().getCurrentCampaign();
	if (!campaign)
		return;

	ctx.getReturn() = getParamObject(ctx, 0)  == campaign->getPC();
}

} // End of namespace DragonAge

} // End of namespace Engines
