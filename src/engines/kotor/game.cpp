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
 *  The context handling the gameplay in Star Wars: Knights of the Old Republic.
 */

#include <cassert>

#include <algorithm>

#include "src/common/filepath.h"
#include "src/common/filelist.h"
#include "src/common/configman.h"

#include "src/events/events.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotor/game.h"
#include "src/engines/kotor/kotor.h"
#include "src/engines/kotor/console.h"
#include "src/engines/kotor/module.h"
#include "src/engines/kotor/area.h"
#include "src/engines/kotor/creature.h"

#include "src/engines/kotor/gui/main/main.h"

#include "src/engines/kotor/script/functions.h"

namespace Engines {

namespace KotOR {

Game::Game(KotOREngine &engine, ::Engines::Console &console, Aurora::Platform platform) :
	_engine(&engine), _platform(platform), _console(&console) {

	_functions.reset(new Functions(*this));
}

Game::~Game() {
}

Module &Game::getModule() {
	assert(_module);

	return *_module;
}

void Game::run() {
	_module.reset(new Module(*_console));

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

void Game::playMenuMusic(Common::UString music) {
	stopMenuMusic();

	if (music.empty())
		music = "mus_theme_cult";

	_menuMusic = playSound(music, Sound::kSoundTypeMusic, true);
}

void Game::stopMenuMusic() {
	SoundMan.stopChannel(_menuMusic);
}

void Game::playMusic(const Common::UString &music) {
	if (_module && _module->isRunning()) {
		Area *area = _module->getCurrentArea();
		if (area)
			area->playAmbientMusic(music);

		return;
	}

	playMenuMusic(music);
}

void Game::stopMusic() {
	stopMenuMusic();

	if (_module && _module->isRunning()) {
		Area *area = _module->getCurrentArea();
		if (area)
			area->stopAmbientMusic();
	}
}

void Game::mainMenu() {
	EventMan.flushEvents();

	MainMenu menu(*_module, _platform == Aurora::kPlatformXbox, _console);

	_console->disableCommand("loadmodule", "not available in the main menu");
	_console->disableCommand("exitmodule", "not available in the main menu");

	menu.show();
	menu.run();
	menu.hide();

	_console->enableCommand("loadmodule");
	_console->enableCommand("exitmodule");
}

void Game::getModules(std::vector<Common::UString> &modules) {
	modules.clear();

	Common::UString moduleDir = ConfigMan.getString("KOTOR_moduleDir");
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

		modules.push_back(file);
	}

	std::sort(modules.begin(), modules.end(), Common::UString::iless());
}

} // End of namespace KotOR

} // End of namespace Engines
