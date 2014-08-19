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

/** @file engines/nwn/console.cpp
 *  NWN (debug) console.
 */

#include <boost/bind.hpp>

#include "common/ustring.h"
#include "common/util.h"
#include "common/configman.h"

#include "aurora/talkman.h"

#include "graphics/aurora/fontman.h"

#include "engines/aurora/util.h"

#include "engines/nwn/console.h"
#include "engines/nwn/nwn.h"
#include "engines/nwn/module.h"
#include "engines/nwn/area.h"

static const int kCampaignNames[] = {
	10314 , 5557  , 5558  , 5559, 5560,
	40041 , 40042 , 40043 ,
	100777, 100778, 100779
};

static const char *kCampaignModules[] = {
	"Prelude"      , "Chapter1"     , "Chapter2"     , "Chapter3", "Chapter4",
	"XP1-Chapter 1", "XP1-Interlude", "XP1-Chapter 2",
	"XP2_Chapter1" , "XP2_Chapter2" , "XP2_Chapter3"
};

namespace Engines {

namespace NWN {

Console::Console() : ::Engines::Console(Graphics::Aurora::kSystemFontMono, 13),
	_module(0), _maxSizeMusic(0) {

	registerCommand("quitmodule"   , boost::bind(&Console::cmdQuitModule   , this, _1),
			"Usage: quitmodule\nLeave the module, returning to the main menu");
	registerCommand("listcampaigns", boost::bind(&Console::cmdListCampaigns, this, _1),
			"Usage: listcampaigns\nList all original campaign modules");
	registerCommand("loadcampaign" , boost::bind(&Console::cmdLoadCampaign , this, _1),
			"Usage: loadcampaign\nLoad a original campaign modules, "
			"replacing the currently running module");
	registerCommand("listmodules"  , boost::bind(&Console::cmdListModules  , this, _1),
			"Usage: listmodules\nList all modules");
	registerCommand("loadmodule"   , boost::bind(&Console::cmdLoadModule   , this, _1),
			"Usage: loadmodule <module>\nLoads a module, "
			"replacing the currently running one");
	registerCommand("listareas"    , boost::bind(&Console::cmdListAreas    , this, _1),
			"Usage: listareas\nList all areas in the current module");
	registerCommand("gotoarea"     , boost::bind(&Console::cmdGotoArea     , this, _1),
			"Usage: gotoarea <area>\nMove to a specific area");
	registerCommand("listmusic"    , boost::bind(&Console::cmdListMusic    , this, _1),
			"Usage: listmusic\nList all available music resources");
	registerCommand("stopmusic"    , boost::bind(&Console::cmdStopMusic    , this, _1),
			"Usage: stopmusic\nStop the currently playing music resource");
	registerCommand("playmusic"    , boost::bind(&Console::cmdPlayMusic    , this, _1),
			"Usage: playmusic [<music>]\nPlay the specified music resource. "
			"If none was specified, play the default area music.");
}

Console::~Console() {
}

void Console::setModule(Module *module) {
	_module = module;
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
	_modules.clear();

	std::vector<Common::UString> modules;
	NWNEngine::getModules(modules);

	for (std::vector<Common::UString>::iterator m = modules.begin(); m != modules.end(); ++m)
		_modules.push_back(*m);

	setArguments("loadmodule", _modules);
}

void Console::updateAreas() {
	_areas.clear();
	if (!_module) {
		setArguments("gotoarea");
		return;
	}

	const std::vector<Common::UString> &areas = _module->_ifo.getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		_areas.push_back(*a);

	setArguments("gotoarea", _areas);
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

	setArguments("playmusic", _music);
}

void Console::cmdQuitModule(const CommandLine &cl) {
	_module->_exit = true;
}

void Console::cmdListCampaigns(const CommandLine &cl) {
	updateCampaigns();
	for (std::list<Common::UString>::iterator c = _campaigns.begin(); c != _campaigns.end(); ++c)
		print(*c);
}

void Console::cmdLoadCampaign(const CommandLine &cl) {
	if (!_module)
		return;

	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	CampaignMap::const_iterator c = _campaignModules.find(cl.args);
	if (c == _campaignModules.end()) {
		printf("No such campaign module \"%s\"", cl.args.c_str());
		return;
	}

	Common::UString module = Common::UString(kCampaignModules[c->second]) + ".nwm";
	_module->changeModule(module);
}

void Console::cmdListModules(const CommandLine &cl) {
	updateModules();
	for (std::list<Common::UString>::iterator m = _modules.begin(); m != _modules.end(); ++m)
		print(*m);
}

void Console::cmdLoadModule(const CommandLine &cl) {
	if (!_module)
		return;

	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	for (std::list<Common::UString>::iterator m = _modules.begin(); m != _modules.end(); ++m) {
		if (m->equalsIgnoreCase(cl.args)) {
			_module->changeModule(cl.args + ".mod");
			return;
		}
	}

	printf("No such module \"%s\"", cl.args.c_str());
}

void Console::cmdListAreas(const CommandLine &cl) {
	if (!_module)
		return;

	updateAreas();
	for (std::list<Common::UString>::iterator a = _areas.begin(); a != _areas.end(); ++a)
		printf("%s (\"%s\")", a->c_str(), Area::getName(*a).c_str());
}

void Console::cmdGotoArea(const CommandLine &cl) {
	if (!_module)
		return;

	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	const std::vector<Common::UString> &areas = _module->_ifo.getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		if (a->equalsIgnoreCase(cl.args)) {
			_module->_newArea = *a;
			return;
		}

	printf("Area \"%s\" does not exist", cl.args.c_str());
}

void Console::cmdListMusic(const CommandLine &cl) {
	updateMusic();
	printList(_music, _maxSizeMusic);
}

void Console::cmdStopMusic(const CommandLine &cl) {
	if (!_module || !_module->_currentArea)
		return;

	_module->_currentArea->stopAmbientMusic();
}

void Console::cmdPlayMusic(const CommandLine &cl) {
	if (!_module || !_module->_currentArea)
		return;

	_module->_currentArea->playAmbientMusic(cl.args);
}

} // End of namespace NWN

} // End of namespace Engines
