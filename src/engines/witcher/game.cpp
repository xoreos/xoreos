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

#include <algorithm>

#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"

#include "src/aurora/lua/scriptman.h"

#include "src/events/events.h"

#include "src/engines/witcher/game.h"
#include "src/engines/witcher/witcher.h"
#include "src/engines/witcher/console.h"
#include "src/engines/witcher/campaign.h"
#include "src/engines/witcher/module.h"
#include "src/engines/witcher/area.h"

#include "src/engines/witcher/nwscript/functions.h"

#include "src/engines/witcher/lua/bindings.h"

namespace Engines {

namespace Witcher {

Game::Game(WitcherEngine &engine, ::Engines::Console &console) : _engine(&engine), _console(&console) {
	_functions.reset(new Functions(*this));
	_bindings.reset(new LuaBindings());
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

void Game::run() {
	_campaign.reset(new Campaign(*_console));

	// Don't know what are this file for. Seems to be some internal scripts.
	// They are absent in game resources.
	LuaScriptMan.addIgnoredFile("global_local");
	LuaScriptMan.addIgnoredFile("startup_local");

	// This needs a bit more research, so ignore them for now.
	LuaScriptMan.addIgnoredFile("combatsystem");
	LuaScriptMan.addIgnoredFile("gui_defs_sum_v2");

	// This are the only files that need to be called manually, I guess.
	// Other script files are called from Lua scripts.
	LuaScriptMan.executeFile("global");
	LuaScriptMan.executeFile("startup");

	while (!EventMan.quitRequested()) {
		runCampaign();
	}

	_campaign.reset();
}

void Game::runCampaign() {
	_campaign->load("thewitcher");
	_campaign->usePC("wiedzmin");

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

	std::sort(campaigns.begin(), campaigns.end(), Common::UString::iless());
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

	std::sort(modules.begin(), modules.end(), Common::UString::iless());
}

} // End of namespace Witcher

} // End of namespace Engines
