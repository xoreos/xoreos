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
 *  Dragon Age II (debug) console.
 */

#include <algorithm>
#include <functional>

#include "src/graphics/aurora/types.h"

#include "src/engines/dragonage2/console.h"
#include "src/engines/dragonage2/dragonage2.h"
#include "src/engines/dragonage2/game.h"
#include "src/engines/dragonage2/campaigns.h"
#include "src/engines/dragonage2/campaign.h"
#include "src/engines/dragonage2/area.h"

namespace Engines {

namespace DragonAge2 {

Console::Console(DragonAge2Engine &engine) :
	::Engines::Console(engine, Graphics::Aurora::kSystemFontMono, 13),
	_engine(&engine) {

	registerCommand("listareas"    , std::bind(&Console::cmdListAreas    , this, std::placeholders::_1),
			"Usage: listareas\nList all areas in the current campaign");
	registerCommand("loadarea"     , std::bind(&Console::cmdLoadArea     , this, std::placeholders::_1),
			"Usage: loadarea <name>\nLoad and show a specific area in the current campaign");
	registerCommand("listcampaigns", std::bind(&Console::cmdListCampaigns, this, std::placeholders::_1),
			"Usage: listcampaigns\nList all playable campaigns");
	registerCommand("loadcampaign" , std::bind(&Console::cmdLoadCampaign , this, std::placeholders::_1),
			"Usage: loadcampaign <name>\nLoad and run a specific campaign");
}

Console::~Console() {
}

void Console::updateCaches() {
	::Engines::Console::updateCaches();

	updateAreas();
	updateCampaigns();
}

void Console::updateAreas() {
	setArguments("loadarea");

	const Campaign *campaign = _engine->getGame().getCampaigns().getCurrentCampaign();
	if (!campaign)
		return;

	setArguments("loadarea", campaign->getAreas());
}

void Console::updateCampaigns() {
	setArguments("loadcampaign");

	const Campaigns &campaignsCtx = _engine->getGame().getCampaigns();

	std::vector<Common::UString> campaignTags;

	const Campaigns::PlayableCampaigns &campaigns = campaignsCtx.getCampaigns();
	for (auto &campaign : campaigns)
		campaignTags.push_back(campaign->getUID());

	std::sort(campaignTags.begin(), campaignTags.end(), Common::UString::iless());
	setArguments("loadcampaign", campaignTags);
}

void Console::cmdListAreas(const CommandLine &UNUSED(cl)) {
	const Campaign *campaign = _engine->getGame().getCampaigns().getCurrentCampaign();
	if (!campaign)
		return;

	for (auto &area : campaign->getAreas()) {
		const Common::UString &rim = campaign->getAreaRIM(area);

		printf("%s (\"%s\")", area.c_str(), Area::getName(area, rim).c_str());
	}
}

void Console::cmdLoadArea(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	Campaign *campaign = _engine->getGame().getCampaigns().getCurrentCampaign();
	if (!campaign)
		return;

	for (auto &area : campaign->getAreas()) {
		if (area.equalsIgnoreCase(cl.args)) {
			hide();
			campaign->movePC(area);
			return;
		}
	}

	printf("No such area \"%s\"", cl.args.c_str());
}

void Console::cmdListCampaigns(const CommandLine &UNUSED(cl)) {
	const Campaigns &campaignsCtx = _engine->getGame().getCampaigns();

	const Campaigns::PlayableCampaigns &campaigns = campaignsCtx.getCampaigns();

	for (auto &campaign : campaigns)
		printf("%s (\"%s\")", campaign->getUID().c_str(), campaign->getName().getString().c_str());
}

void Console::cmdLoadCampaign(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	Campaigns &campaignsCtx = _engine->getGame().getCampaigns();

	const Campaigns::PlayableCampaigns &campaigns = campaignsCtx.getCampaigns();
	for (auto &campaign : campaigns) {
		if (campaign->getUID().equalsIgnoreCase(cl.args)) {
			hide();
			campaignsCtx.load(*campaign);
			return;
		}
	}

	printf("No such campaign \"%s\"", cl.args.c_str());
}

} // End of namespace DragonAge2

} // End of namespace Engines
