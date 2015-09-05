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
 *  Dragon Age: Origins (debug) console.
 */

#include <algorithm>

#include <boost/bind.hpp>

#include "src/graphics/aurora/types.h"

#include "src/engines/dragonage/console.h"
#include "src/engines/dragonage/dragonage.h"
#include "src/engines/dragonage/game.h"
#include "src/engines/dragonage/campaigns.h"
#include "src/engines/dragonage/campaign.h"
#include "src/engines/dragonage/area.h"

namespace Engines {

namespace DragonAge {

Console::Console(DragonAgeEngine &engine) :
	::Engines::Console(engine, Graphics::Aurora::kSystemFontMono, 13),
	_engine(&engine) {

	registerCommand("listareas"    , boost::bind(&Console::cmdListAreas     , this, _1),
			"Usage: listareas\nList all areas in the current campaign");
	registerCommand("loadarea"     , boost::bind(&Console::cmdLoadArea     , this, _1),
			"Usage: loadarea <name>\nLoad and show a specific area in the current campaign");
	registerCommand("listcampaigns", boost::bind(&Console::cmdListCampaigns, this, _1),
			"Usage: listcampaigns\nList all playable campaigns");
	registerCommand("loadcampaign" , boost::bind(&Console::cmdLoadCampaign , this, _1),
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
	for (Campaigns::PlayableCampaigns::const_iterator c = campaigns.begin(); c != campaigns.end(); ++c)
		campaignTags.push_back((*c)->getUID());

	std::sort(campaignTags.begin(), campaignTags.end(), Common::UString::iless());
	setArguments("loadcampaign", campaignTags);
}

void Console::cmdListAreas(const CommandLine &UNUSED(cl)) {
	const Campaign *campaign = _engine->getGame().getCampaigns().getCurrentCampaign();
	if (!campaign)
		return;

	const std::vector<Common::UString> &areas = campaign->getAreas();

	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a) {
		const Common::UString &rim = campaign->getAreaRIM(*a);

		printf("%s (\"%s\")", a->c_str(), Area::getName(*a, rim).c_str());
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

	const std::vector<Common::UString> &areas = campaign->getAreas();

	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a) {
		if (a->equalsIgnoreCase(cl.args)) {
			hide();
			campaign->movePC(*a);
			return;
		}
	}

	printf("No such area \"%s\"", cl.args.c_str());
}

void Console::cmdListCampaigns(const CommandLine &UNUSED(cl)) {
	const Campaigns &campaignsCtx = _engine->getGame().getCampaigns();

	const Campaigns::PlayableCampaigns &campaigns = campaignsCtx.getCampaigns();

	for (Campaigns::PlayableCampaigns::const_iterator c = campaigns.begin(); c != campaigns.end(); ++c)
		printf("%s (\"%s\")", (*c)->getUID().c_str(), (*c)->getName().getString().c_str());
}

void Console::cmdLoadCampaign(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	Campaigns &campaignsCtx = _engine->getGame().getCampaigns();

	const Campaigns::PlayableCampaigns &campaigns = campaignsCtx.getCampaigns();
	for (Campaigns::PlayableCampaigns::const_iterator c = campaigns.begin(); c != campaigns.end(); ++c) {
		if ((*c)->getUID().equalsIgnoreCase(cl.args)) {
			hide();
			campaignsCtx.load(**c);
			return;
		}
	}

	printf("No such campaign \"%s\"", cl.args.c_str());
}

} // End of namespace DragonAge

} // End of namespace Engines
