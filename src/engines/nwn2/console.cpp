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

/** @file engines/nwn2/console.cpp
 *  NWN2 (debug) console.
 */

#include <boost/bind.hpp>

#include "common/ustring.h"
#include "common/util.h"
#include "common/filepath.h"
#include "common/filelist.h"

#include "aurora/resman.h"

#include "graphics/aurora/fontman.h"

#include "engines/nwn2/console.h"
#include "engines/nwn2/module.h"
#include "engines/nwn2/area.h"

namespace Engines {

namespace NWN2 {

Console::Console() : ::Engines::Console(Graphics::Aurora::kSystemFontMono, 13),
	_module(0), _maxSizeMusic(0) {

	registerCommand("listmusic"  , boost::bind(&Console::cmdListMusic  , this, _1),
			"Usage: listmusic\nList all available music resources");
	registerCommand("stopmusic"  , boost::bind(&Console::cmdStopMusic  , this, _1),
			"Usage: stopmusic\nStop the currently playing music resource");
	registerCommand("playmusic"  , boost::bind(&Console::cmdPlayMusic  , this, _1),
			"Usage: playmusic [<music>]\nPlay the specified music resource. "
			"If none was specified, play the default area music.");
	registerCommand("listareas"  , boost::bind(&Console::cmdListAreas  , this, _1),
			"Usage: listareas\nList all areas in the current module");
	registerCommand("gotoarea"   , boost::bind(&Console::cmdGotoArea   , this, _1),
			"Usage: gotoarea <area>\nMove to a specific area");
	registerCommand("listmodules", boost::bind(&Console::cmdListModules, this, _1),
			"Usage: listmodules\nList all modules");
	registerCommand("loadmodule" , boost::bind(&Console::cmdLoadModule , this, _1),
			"Usage: loadmodule <module>\nLoads a module, "
			"replacing the currently running one");
}

Console::~Console() {
}

void Console::setModule(Module *module) {
	_module = module;
}

void Console::updateCaches() {
	::Engines::Console::updateCaches();

	updateMusic();
	updateAreas();
	updateModules();
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

void Console::updateModules() {
	_modules.clear();
	setArguments("loadmodule", _modules);

	Common::UString baseDir   = ResMan.getDataBaseDir();
	Common::UString moduleDir = Common::FilePath::findSubDirectory(baseDir, "modules", true);
	if (moduleDir.empty())
		return;

	Common::FileList modules;
	if (!modules.addDirectory(moduleDir))
		return;

	for (Common::FileList::const_iterator m = modules.begin(); m != modules.end(); ++m) {
		if (!Common::FilePath::getExtension(*m).equalsIgnoreCase(".mod"))
			continue;

		_modules.push_back(Common::FilePath::getStem(*m));
	}

	_modules.sort(Common::UString::iless());
	setArguments("loadmodule", _modules);
}

void Console::cmdListMusic(const CommandLine &UNUSED(cl)) {
	updateMusic();
	printList(_music, _maxSizeMusic);
}

void Console::cmdStopMusic(const CommandLine &UNUSED(cl)) {
	if (!_module || !_module->_currentArea)
		return;

	_module->_currentArea->stopAmbientMusic();
}

void Console::cmdPlayMusic(const CommandLine &cl) {
	if (!_module || !_module->_currentArea)
		return;

	_module->_currentArea->playAmbientMusic(cl.args);
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

	const std::vector<Common::UString> &areas = _module->_ifo.getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		if (a->equalsIgnoreCase(cl.args)) {
			_module->_newArea = *a;
			return;
		}

	printf("Area \"%s\" does not exist", cl.args.c_str());
}

void Console::cmdListModules(const CommandLine &UNUSED(cl)) {
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

} // End of namespace NWN2

} // End of namespace Engines
