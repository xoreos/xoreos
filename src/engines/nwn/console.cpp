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
 *  Neverwinter Nights (debug) console.
 */

#include <algorithm>
#include <functional>

#include "src/common/ustring.h"
#include "src/common/util.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"
#include "src/aurora/talkman.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn/console.h"
#include "src/engines/nwn/nwn.h"
#include "src/engines/nwn/game.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/area.h"

static const uint32_t kCampaignNames[] = {
	10314 , 5557  , 5558  , 5559, 5560,
	40041 , 40042 , 40043 ,
	100777, 100778, 100779
};

static const char * const kCampaignModules[] = {
	"Prelude"      , "Chapter1"     , "Chapter2"     , "Chapter3", "Chapter4",
	"XP1-Chapter 1", "XP1-Interlude", "XP1-Chapter 2",
	"XP2_Chapter1" , "XP2_Chapter2" , "XP2_Chapter3"
};

namespace Engines {

namespace NWN {

Console::Console(NWNEngine &engine) :
	::Engines::Console(engine, Graphics::Aurora::kSystemFontMono, 13),
	_engine(&engine), _maxSizeMusic(0) {

	registerCommand("exitmodule"   , std::bind(&Console::cmdExitModule   , this, std::placeholders::_1),
			"Usage: exitmodule\nExit the module, returning to the main menu");
	registerCommand("listcampaigns", std::bind(&Console::cmdListCampaigns, this, std::placeholders::_1),
			"Usage: listcampaigns\nList all original campaign modules");
	registerCommand("loadcampaign" , std::bind(&Console::cmdLoadCampaign , this, std::placeholders::_1),
			"Usage: loadcampaign\nLoad a original campaign modules, "
			"replacing the currently running module");
	registerCommand("listmodules"  , std::bind(&Console::cmdListModules  , this, std::placeholders::_1),
			"Usage: listmodules\nList all modules");
	registerCommand("loadmodule"   , std::bind(&Console::cmdLoadModule   , this, std::placeholders::_1),
			"Usage: loadmodule <module>\nLoads a module, "
			"replacing the currently running one");
	registerCommand("listareas"    , std::bind(&Console::cmdListAreas    , this, std::placeholders::_1),
			"Usage: listareas\nList all areas in the current module");
	registerCommand("gotoarea"     , std::bind(&Console::cmdGotoArea     , this, std::placeholders::_1),
			"Usage: gotoarea <area>\nMove to a specific area");
	registerCommand("listmusic"    , std::bind(&Console::cmdListMusic    , this, std::placeholders::_1),
			"Usage: listmusic\nList all available music resources");
	registerCommand("stopmusic"    , std::bind(&Console::cmdStopMusic    , this, std::placeholders::_1),
			"Usage: stopmusic\nStop the currently playing music resource");
	registerCommand("playmusic"    , std::bind(&Console::cmdPlayMusic    , this, std::placeholders::_1),
			"Usage: playmusic [<music>]\nPlay the specified music resource. "
			"If none was specified, play the default area music.");
	registerCommand("showwalkmesh" , std::bind(&Console::cmdShowWalkmesh , this, std::placeholders::_1),
			"Usage: showwalkmesh\nToggle walkmesh display");
}

Console::~Console() {
}

void Console::updateCaches() {
	::Engines::Console::updateCaches();

	updateCampaigns();
	updateModules();
	updateAreas();
	updateMusic();
}

void Console::updateCampaigns() {
	_campaigns.clear();
	_campaignModules.clear();

	_campaigns.reserve(ARRAYSIZE(kCampaignNames));

	// Base game
	for (int i = 0; i < 5; i++) {
		Common::UString name = TalkMan.getString(kCampaignNames[i]);
		name.truncate(name.findFirst('\n'));
		name.trim();

		_campaigns.push_back(name);
		_campaignModules.insert(std::make_pair(name, i));
	}

	// XP1
	if (ConfigMan.getBool("NWN_hasXP1")) {
		for (int i = 5; i < 8; i++) {
			Common::UString name = "XP1: " + TalkMan.getString(kCampaignNames[i]);
			name.truncate(name.findFirst('\n'));
			name.trim();

			_campaigns.push_back(name);
			_campaignModules.insert(std::make_pair(name, i));
		}
	}

	// XP2
	if (ConfigMan.getBool("NWN_hasXP2")) {
		for (int i = 8; i < 11; i++) {
			Common::UString name = "XP2: " + TalkMan.getString(kCampaignNames[i]);
			name.truncate(name.findFirst('\n'));
			name.trim();

			_campaigns.push_back(name);
			_campaignModules.insert(std::make_pair(name, i));
		}
	}

	setArguments("loadcampaign", _campaigns);
}

void Console::updateModules() {
	Game::getModules(_modules);

	setArguments("loadmodule", _modules);
}

void Console::updateAreas() {
	setArguments("gotoarea", _engine->getGame().getModule().getIFO().getAreas());
}

void Console::updateMusic() {
	_music.clear();
	_maxSizeMusic = 0;

	std::list<Aurora::ResourceManager::ResourceID> music;
	ResMan.getAvailableResources(Aurora::kFileTypeBMU, music);

	for (std::list<Aurora::ResourceManager::ResourceID>::const_iterator m = music.begin();
	     m != music.end(); ++m) {

		_music.push_back(m->name);

		_maxSizeMusic = MAX(_maxSizeMusic, _music.back().size());
	}

	std::sort(_music.begin(), _music.end(), Common::UString::iless());
	setArguments("playmusic", _music);
}

void Console::cmdExitModule(const CommandLine &UNUSED(cl)) {
	hide();

	_engine->getGame().getModule().exit();
}

void Console::cmdListCampaigns(const CommandLine &UNUSED(cl)) {
	updateCampaigns();
	for (std::vector<Common::UString>::iterator c = _campaigns.begin(); c != _campaigns.end(); ++c)
		print(*c);
}

void Console::cmdLoadCampaign(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	CampaignMap::const_iterator c = _campaignModules.find(cl.args);
	if (c == _campaignModules.end()) {
		printf("No such campaign module \"%s\"", cl.args.c_str());
		return;
	}

	hide();

	Common::UString mod = Common::UString(kCampaignModules[c->second]) + ".nwm";
	_engine->getGame().getModule().load(mod);
}

void Console::cmdListModules(const CommandLine &UNUSED(cl)) {
	updateModules();
	printList(_modules);
}

void Console::cmdLoadModule(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	for (std::vector<Common::UString>::iterator m = _modules.begin(); m != _modules.end(); ++m) {
		if (m->equalsIgnoreCase(cl.args)) {
			hide();

			_engine->getGame().getModule().load(cl.args + ".mod");
			return;
		}
	}

	printf("No such module \"%s\"", cl.args.c_str());
}

void Console::cmdListAreas(const CommandLine &UNUSED(cl)) {
	updateAreas();

	const std::vector<Common::UString> &areas = _engine->getGame().getModule().getIFO().getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		printf("%s (\"%s\")", a->c_str(), Area::getName(*a).c_str());
}

void Console::cmdGotoArea(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	Module &module = _engine->getGame().getModule();

	const std::vector<Common::UString> &areas = module.getIFO().getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		if (a->equalsIgnoreCase(cl.args)) {
			hide();
			module.movePC(*a);
			return;
		}

	printf("Area \"%s\" does not exist", cl.args.c_str());
}

void Console::cmdListMusic(const CommandLine &UNUSED(cl)) {
	updateMusic();
	printList(_music, _maxSizeMusic);
}

void Console::cmdStopMusic(const CommandLine &UNUSED(cl)) {
	_engine->getGame().stopMusic();
}

void Console::cmdPlayMusic(const CommandLine &cl) {
	_engine->getGame().playMusic(cl.args);
}

void Console::cmdShowWalkmesh(const CommandLine &UNUSED(cl)) {
	_engine->getGame().getModule().toggleWalkmesh();
}

} // End of namespace NWN

} // End of namespace Engines
