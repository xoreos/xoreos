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
 *  The context handling the gameplay in Neverwinter Nights 2.
 */

#include <cassert>

#include <algorithm>

#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"

#include "src/events/events.h"

#include "src/engines/nwn2/game.h"
#include "src/engines/nwn2/nwn2.h"
#include "src/engines/nwn2/console.h"
#include "src/engines/nwn2/campaign.h"
#include "src/engines/nwn2/module.h"
#include "src/engines/nwn2/area.h"

#include "src/engines/nwn2/script/functions.h"

namespace Engines {

namespace NWN2 {

Game::Game(NWN2Engine &engine, ::Engines::Console &console) :
	_engine(&engine), _console(&console) {

	_functions = std::make_unique<Functions>(*this);
}

Game::~Game() {
}

Campaign &Game::getCampaign() {
	assert(_campaign);

	return *_campaign;
}

Module &Game::getModule() {
	assert(_campaign);

	return _campaign->getModule();
}

int32_t Game::getGameDifficulty() {
	return ConfigMan.getInt("difficulty");
}

void Game::run() {
	_campaign = std::make_unique<Campaign>(*_console);

	while (!EventMan.quitRequested()) {
		runCampaign();
	}

	_campaign.reset();
}

void Game::runCampaign() {
	std::vector<Common::UString> characters;
	getCharacters(characters, true);

	if (characters.empty())
		throw Common::Exception("No characters in the localvault directory");

	_campaign->load("neverwinter nights 2 campaign");
	_campaign->usePC(*characters.begin(), true);

	if (EventMan.quitRequested() || !_campaign->isLoaded()) {
		_campaign->clear();
		return;
	}

	_campaign->enter();
	EventMan.enableKeyRepeat(true);

	while (!EventMan.quitRequested() && _campaign->isRunning()) {
		Events::Event event;
		while (EventMan.pollEvent(event))
			_campaign->addEvent(event);

		_campaign->processEventQueue();
		EventMan.delay(10);
	}

	EventMan.enableKeyRepeat(false);
	_campaign->leave();

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

void Game::getCampaigns(std::vector<Common::UString> &campaigns) {
	campaigns.clear();

	const Common::UString directory = ConfigMan.getString("NWN2_campaignDir");

	const Common::FileList camFiles(directory, -1);

	for (Common::FileList::const_iterator c = camFiles.begin(); c != camFiles.end(); ++c) {
		if (!Common::FilePath::getFile(*c).equalsIgnoreCase("campaign.cam"))
			continue;

		const Common::UString cam = Common::FilePath::relativize(directory, Common::FilePath::getDirectory(*c));
		if (cam.empty() || (cam == "."))
			continue;

		campaigns.push_back(cam);
	}

	std::sort(campaigns.begin(), campaigns.end(), Common::UString::iless());
}

void Game::getModules(std::vector<Common::UString> &modules) {
	modules.clear();

	const Common::UString directory = ConfigMan.getString("NWN2_moduleDir");

	const Common::FileList modFiles(directory);

	for (Common::FileList::const_iterator m = modFiles.begin(); m != modFiles.end(); ++m) {
		if (!Common::FilePath::getExtension(*m).equalsIgnoreCase(".mod"))
			continue;

		modules.push_back(Common::FilePath::getStem(*m));
	}

	std::sort(modules.begin(), modules.end(), Common::UString::iless());
}

void Game::getCharacters(std::vector<Common::UString> &characters, bool local) {
	characters.clear();

	Common::UString pcDir = ConfigMan.getString(local ? "NWN2_localPCDir" : "NWN2_serverPCDir");
	if (pcDir.empty())
		return;

	Common::FileList chars;
	chars.addDirectory(pcDir);

	for (Common::FileList::const_iterator c = chars.begin(); c != chars.end(); ++c) {
		if (!Common::FilePath::getExtension(*c).equalsIgnoreCase(".bic"))
			continue;

		characters.push_back(Common::FilePath::getStem(*c));
	}

	std::sort(characters.begin(), characters.end(), Common::UString::iless());
}

} // End of namespace NWN2

} // End of namespace Engines
