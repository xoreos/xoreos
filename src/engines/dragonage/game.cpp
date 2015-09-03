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
 *  The context handling the gameplay in Dragon Age: Origins.
 */

#include <cassert>

#include "src/common/error.h"

#include "src/engines/dragonage/game.h"
#include "src/engines/dragonage/dragonage.h"
#include "src/engines/dragonage/campaigns.h"
#include "src/engines/dragonage/campaign.h"

namespace Engines {

namespace DragonAge {

Game::Game(DragonAgeEngine &engine, ::Engines::Console &console) :
	_engine(&engine), _campaigns(0), _console(&console) {

}

Game::~Game() {
	delete _campaigns;
}

Campaigns &Game::getCampaigns() {
	assert(_campaigns);

	return *_campaigns;
}

void Game::run() {
	_campaigns = new Campaigns(*_console, *_engine);

	runCampaigns();

	delete _campaigns;
	_campaigns = 0;
}

void Game::runCampaigns() {
	const Campaign *singlePlayer = _campaigns->findCampaign("Single Player");
	if (!singlePlayer)
		throw Common::Exception("Can't find the default single player campaign");

	_campaigns->load(*singlePlayer);
	_campaigns->run();
}

} // End of namespace DragonAge

} // End of namespace Engines
