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
 *  The context handling the gameplay in Star Wars: Knights of the Old Republic II - The Sith Lords.
 */

#include <cassert>

#include <algorithm>

#include "src/common/filepath.h"
#include "src/common/filelist.h"
#include "src/common/configman.h"

#include "src/events/events.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotorbase/area.h"

#include "src/engines/kotor2/game.h"
#include "src/engines/kotor2/kotor2.h"
#include "src/engines/kotor2/console.h"
#include "src/engines/kotor2/creature.h"
#include "src/engines/kotor2/module.h"

#include "src/engines/kotor2/gui/main/main.h"

#include "src/engines/kotor2/script/functions.h"

namespace Engines {

namespace KotOR2 {

Game::Game(KotOR2Engine &engine, Engines::Console &console, Aurora::Platform platform) :
		KotORBase::Game(console),
		_engine(&engine),
		_platform(platform) {

	_functions = std::make_unique<Functions>(*this);
	collectModules();
}

Game::~Game() {
}

void Game::run() {
	_module = std::make_unique<Module>(*_console);

	while (!EventMan.quitRequested()) {
		mainMenu();
		runModule();
	}

	_module.reset();
}

void Game::runModule() {
	if (EventMan.quitRequested() || !_module->isLoaded()) {
		_module->clear();
		return;
	}

	_module->enter();
	EventMan.enableKeyRepeat(true);

	while (!EventMan.quitRequested() && _module->isRunning()) {
		Events::Event event;
		while (EventMan.pollEvent(event))
			_module->addEvent(event);

		_module->processEventQueue();
		EventMan.delay(10);
	}

	EventMan.enableKeyRepeat(false);
	_module->leave();

	_module->clear();
}

void Game::mainMenu() {
	EventMan.flushEvents();

	MainMenu menu(*_module, _console);

	_console->disableCommand("loadmodule", "not available in the main menu");
	_console->disableCommand("exitmodule", "not available in the main menu");

	menu.show();
	menu.run();
	menu.hide();

	_console->enableCommand("loadmodule");
	_console->enableCommand("exitmodule");
}

const Common::UString &Game::getDefaultMenuMusic() const {
	static Common::UString music("mus_sion");
	return music;
}

void Game::collectModules() {
	_modules.clear();

	Common::UString moduleDir = ConfigMan.getString("KOTOR2_moduleDir");
	if (moduleDir.empty())
		return;

	Common::FileList mods;
	mods.addDirectory(moduleDir);

	for (Common::FileList::const_iterator m = mods.begin(); m != mods.end(); ++m) {
		Common::UString file = m->toLower();
		if (!file.endsWith("_s.rim"))
			continue;

		file = Common::FilePath::getStem(file);
		file.truncate(file.size() - Common::UString("_s").size());

		_modules.push_back(file);
	}

	std::sort(_modules.begin(), _modules.end(), Common::UString::iless());
}

bool Game::hasModule(const Common::UString &module) const {
	auto found = std::find_if(_modules.begin(), _modules.end(), [&](const Common::UString &x) {
		return x.equalsIgnoreCase(module);
	});

	return found != _modules.end();
}

} // End of namespace KotOR2

} // End of namespace Engines
