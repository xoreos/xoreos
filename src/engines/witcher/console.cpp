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
 *  The Witcher (debug) console.
 */

#include <algorithm>

#include <boost/bind.hpp>

#include "src/common/ustring.h"
#include "src/common/util.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/aurora/util.h"

#include "src/engines/witcher/console.h"
#include "src/engines/witcher/witcher.h"
#include "src/engines/witcher/game.h"
#include "src/engines/witcher/campaign.h"
#include "src/engines/witcher/module.h"
#include "src/engines/witcher/area.h"

namespace Engines {

namespace Witcher {

Console::Console(WitcherEngine &engine) :
	::Engines::Console(engine, Graphics::Aurora::kSystemFontMono, 13),
	_engine(&engine), _maxSizeMusic(0) {

	registerCommand("listmusic"    , boost::bind(&Console::cmdListMusic    , this, _1),
			"Usage: listmusic\nList all available music resources");
	registerCommand("stopmusic"    , boost::bind(&Console::cmdStopMusic    , this, _1),
			"Usage: stopmusic\nStop the currently playing music resource");
	registerCommand("playmusic"    , boost::bind(&Console::cmdPlayMusic    , this, _1),
			"Usage: playmusic [<music>]\nPlay the specified music resource. "
			"If none was specified, play the default area music.");
	registerCommand("move"         , boost::bind(&Console::cmdMove         , this, _1),
			"Usage: move <x> <y> <z>\nMove to this position in the current area");
	registerCommand("listareas"    , boost::bind(&Console::cmdListAreas    , this, _1),
			"Usage: listareas\nList all areas in the current module");
	registerCommand("gotoarea"     , boost::bind(&Console::cmdGotoArea     , this, _1),
			"Usage: gotoarea <area>\nMove to a specific area");
	registerCommand("listcampaigns", boost::bind(&Console::cmdListCampaigns, this, _1),
			"Usage: listcampaigns\nList all campaigns");
	registerCommand("loadcampaign" , boost::bind(&Console::cmdLoadCampaign , this, _1),
			"Usage: loadcampaign <campaign>\nLoads a campaign, "
			"replacing the currently running one");
	registerCommand("listmodules"  , boost::bind(&Console::cmdListModules  , this, _1),
			"Usage: listmodules\nList all modules");
	registerCommand("loadmodule"   , boost::bind(&Console::cmdLoadModule   , this, _1),
			"Usage: loadmodule <module>\nLoads a module, "
			"replacing the currently running one");
}

Console::~Console() {
}

void Console::updateCaches() {
	::Engines::Console::updateCaches();

	updateMusic();
	updateCampaigns();
	updateModules();
	updateAreas();
}

void Console::updateMusic() {
	_music.clear();
	_maxSizeMusic = 0;

	setArguments("playmusic", _music);

	Common::UString musicDir = Common::FilePath::findSubDirectory(ResMan.getDataBase(), "data/music", true);
	if (musicDir.empty())
		return;

	Common::FileList musicFiles;
	if (!musicFiles.addDirectory(musicDir, -1))
		return;

	Common::FileList oggFiles;
	if (!musicFiles.getSubList(".ogg", true, oggFiles))
		return;

	for (Common::FileList::const_iterator o = oggFiles.begin(); o != oggFiles.end(); ++o) {
		_music.push_back(Common::FilePath::getStem(*o));

		_maxSizeMusic = MAX(_maxSizeMusic, _music.back().size());
	}

	std::sort(_music.begin(), _music.end(), Common::UString::iless());
	setArguments("playmusic", _music);
}

void Console::updateCampaigns() {
	Game::getCampaigns(_campaigns);

	setArguments("loadcampaign", _campaigns);
}

void Console::updateModules() {
	Game::getModules(_modules);

	setArguments("loadmodule", _modules);
}

void Console::updateAreas() {
	setArguments("gotoarea", _engine->getGame().getModule().getIFO().getAreas());
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

void Console::cmdMove(const CommandLine &cl) {
	std::vector<Common::UString> args;
	splitArguments(cl.args, args);

	float x, z, y;
	if ((args.size() < 3) ||
	    (std::sscanf(args[0].c_str(), "%f", &x) != 1) ||
	    (std::sscanf(args[1].c_str(), "%f", &y) != 1) ||
	    (std::sscanf(args[2].c_str(), "%f", &z) != 1)) {

		printCommandHelp(cl.cmd);
		return;
	}

	_engine->getGame().getModule().movePC(x, y, z);
}

void Console::cmdListAreas(const CommandLine &UNUSED(cl)) {
	updateAreas();

	const std::vector<Common::UString> &areas = _engine->getGame().getModule().getIFO().getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		printf("%s (\"%s\")", a->c_str(), Area::getName(*a).getString().c_str());
}

void Console::cmdGotoArea(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	const std::vector<Common::UString> &areas = _engine->getGame().getModule().getIFO().getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		if (a->equalsIgnoreCase(cl.args)) {
			hide();
			_engine->getGame().getModule().movePC(*a);
			return;
		}

	printf("Area \"%s\" does not exist", cl.args.c_str());
}

void Console::cmdListCampaigns(const CommandLine &UNUSED(cl)) {
	updateCampaigns();

	for (std::vector<Common::UString>::const_iterator c = _campaigns.begin(); c != _campaigns.end(); ++c)
		printf("%s (\"%s\")", c->c_str(), Campaign::getName(*c).c_str());
}

void Console::cmdLoadCampaign(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	updateCampaigns();
	for (std::vector<Common::UString>::const_iterator c = _campaigns.begin(); c != _campaigns.end(); ++c) {
		if (c->equalsIgnoreCase(cl.args)) {
			hide();
			_engine->getGame().getCampaign().load(*c);
			return;
		}
	}

	printf("No such campaign \"%s\"", cl.args.c_str());
}

void Console::cmdListModules(const CommandLine &UNUSED(cl)) {
	updateModules();

	for (std::vector<Common::UString>::const_iterator m = _modules.begin(); m != _modules.end(); ++m) {
		const Common::UString name = Module::getName(*m);

		if (!name.empty())
			printf("%s (\"%s\")", m->c_str(), name.c_str());
		else
			printf("%s", m->c_str());
	}
}

void Console::cmdLoadModule(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	updateModules();
	for (std::vector<Common::UString>::const_iterator m = _modules.begin(); m != _modules.end(); ++m) {
		if (m->equalsIgnoreCase(cl.args)) {
			hide();
			_engine->getGame().getCampaign().loadModule(cl.args);
			return;
		}
	}

	printf("No such module \"%s\"", cl.args.c_str());
}

} // End of namespace Witcher

} // End of namespace Engines
