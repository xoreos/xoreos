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
	_campaign->load("thewitcher");

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

void Game::getCampaigns(std::vector<Common::UString> &campaigns) {
	campaigns.clear();

	const Common::FileList mmdFiles(ConfigMan.getString("WITCHER_moduleDir"), -1);

	for (Common::FileList::const_iterator c = mmdFiles.begin(); c != mmdFiles.end(); ++c) {
		if (!Common::FilePath::getExtension(*c).equalsIgnoreCase(".mmd"))
			continue;

		const Common::UString mmd = Common::FilePath::getStem(*c);
		if (mmd.empty())
			continue;

		campaigns.push_back(mmd);
	}
}

void Game::getModules(std::vector<Common::UString> &modules) {
	modules.clear();

	const Common::FileList modFiles(ConfigMan.getString("WITCHER_moduleDir"), -1);

	for (Common::FileList::const_iterator c = modFiles.begin(); c != modFiles.end(); ++c) {
		if (!Common::FilePath::getExtension(*c).equalsIgnoreCase(".mod") &&
		    !Common::FilePath::getExtension(*c).equalsIgnoreCase(".adv"))
			continue;

		const Common::UString mod = Common::FilePath::getStem(*c);
		if (mod.empty())
			continue;

		modules.push_back(mod);
	}
}

} // End of namespace Witcher

} // End of namespace Engines
