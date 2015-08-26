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
 *  The context handling the gameplay in The Witcher.
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"

#include "src/events/events.h"

#include "src/engines/witcher/game.h"
#include "src/engines/witcher/witcher.h"
#include "src/engines/witcher/console.h"
#include "src/engines/witcher/campaign.h"
#include "src/engines/witcher/module.h"
#include "src/engines/witcher/area.h"

namespace Engines {

namespace Witcher {

Game::Game(WitcherEngine &engine, ::Engines::Console &console) :
	_engine(&engine), _campaign(0), _console(&console) {

}

Game::~Game() {
	delete _campaign;
}

Campaign &Game::getCampaign() {
	assert(_campaign);

	return *_campaign;
}

Module &Game::getModule() {
	assert(_campaign);

	return _campaign->getModule();
}

void Game::run() {
	_campaign = new Campaign(*_console);

	while (!EventMan.quitRequested()) {
		runCampaign();
	}

	delete _campaign;
	_campaign = 0;
}

void Game::runCampaign() {
	const std::list<CampaignDescription> &campaigns = _campaign->getCampaigns();
	if (campaigns.empty())
		error("No campaigns found");

	// Find the original The Witcher campaign
	const CampaignDescription *witcherCampaign = 0;
	for (std::list<CampaignDescription>::const_iterator c = campaigns.begin(); c != campaigns.end(); ++c)
		if (c->tag == "thewitcher")
			witcherCampaign = &*c;

	// If that's not available, load the first one found
	if (!witcherCampaign)
		witcherCampaign = &*campaigns.begin();

	_campaign->load(*witcherCampaign);
	_campaign->run();
	_campaign->clear();
}

void Game::playMusic(const Common::UString &music) {
	if (!_campaign || !_campaign->getModule().isRunning())
		return;

	Area *area = _campaign->getModule().getCurrentArea();
	if (!area)
		return;

	area->playAmbientMusic(music);
}

void Game::stopMusic() {
	if (!_campaign || !_campaign->getModule().isRunning())
		return;

	Area *area = _campaign->getModule().getCurrentArea();
	if (!area)
		return;

	area->stopAmbientMusic();
}

void Game::refreshLocalized() {
	if (!_campaign)
		return;

	_campaign->refreshLocalized();
}

} // End of namespace Witcher

} // End of namespace Engines
