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

#include <boost/bind.hpp>

#include "src/common/ustring.h"
#include "src/common/util.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"

#include "src/aurora/resman.h"

#include "src/graphics/aurora/fontman.h"

#include "src/engines/aurora/util.h"

#include "src/engines/witcher/console.h"
#include "src/engines/witcher/witcher.h"
#include "src/engines/witcher/campaign.h"
#include "src/engines/witcher/module.h"
#include "src/engines/witcher/area.h"

namespace Engines {

namespace Witcher {

Console::Console() : ::Engines::Console(Graphics::Aurora::kSystemFontMono, 13),
	_campaign(0), _module(0), _maxSizeMusic(0) {

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
}

Console::~Console() {
}

void Console::setCampaign(Campaign *campaign) {
	_campaign = campaign;
}

void Console::setModule(Module *module) {
	_module = module;
}

void Console::updateCaches() {
	::Engines::Console::updateCaches();

	updateMusic();
	updateAreas();
}

void Console::updateMusic() {
	_music.clear();
	_maxSizeMusic = 0;

	setArguments("playmusic", _music);

	Common::UString musicDir = Common::FilePath::findSubDirectory(ResMan.getDataBaseDir(), "data/music", true);
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

	_music.sort(Common::UString::iless());
	setArguments("playmusic", _music);
}

void Console::updateAreas() {
	_areas.clear();
	if (!_module) {
		setArguments("gotoarea");
		return;
	}

	const std::vector<Common::UString> &areas = _module->getIFO().getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		_areas.push_back(*a);

	_areas.sort(Common::UString::iless());
	setArguments("gotoarea", _areas);
}

void Console::cmdListMusic(const CommandLine &UNUSED(cl)) {
	updateMusic();
	printList(_music, _maxSizeMusic);
}

void Console::cmdStopMusic(const CommandLine &UNUSED(cl)) {
	Area *area = 0;
	if (!_module || !(area = _module->getCurrentArea()))
		return;

	area->stopAmbientMusic();
}

void Console::cmdPlayMusic(const CommandLine &cl) {
	Area *area = 0;
	if (!_module || !(area = _module->getCurrentArea()))
		return;

	area->playAmbientMusic(cl.args);
}

void Console::cmdMove(const CommandLine &cl) {
	std::vector<Common::UString> args;
	splitArguments(cl.args, args);

	float x, z, y;
	if ((args.size() < 3) ||
	    (sscanf(args[0].c_str(), "%f", &x) != 1) ||
	    (sscanf(args[1].c_str(), "%f", &y) != 1) ||
	    (sscanf(args[2].c_str(), "%f", &z) != 1)) {

		printCommandHelp(cl.cmd);
		return;
	}

	if (!_module)
		return;

	_module->movePC(x, y, z);
}

void Console::cmdListAreas(const CommandLine &UNUSED(cl)) {
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

	const std::vector<Common::UString> &areas = _module->getIFO().getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		if (a->equalsIgnoreCase(cl.args)) {
			_module->movePC(*a);
			return;
		}

	printf("Area \"%s\" does not exist", cl.args.c_str());
}

} // End of namespace Witcher

} // End of namespace Engines
