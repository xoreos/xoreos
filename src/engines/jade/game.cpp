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
 *  The context handling the gameplay in Jade Empire.
 */

#include <cassert>

#include "src/aurora/resman.h"

#include "src/events/events.h"

#include "src/sound/sound.h"
#include "src/sound/audiostream.h"
#include "src/sound/xactwavebank.h"

#include "src/engines/aurora/util.h"

#include "src/engines/jade/game.h"
#include "src/engines/jade/jade.h"
#include "src/engines/jade/console.h"
#include "src/engines/jade/module.h"
#include "src/engines/jade/area.h"
#include "src/engines/jade/creature.h"

#include "src/engines/jade/gui/main/main.h"

#include "src/engines/jade/script/functions.h"

namespace Engines {

namespace Jade {

Game::Game(JadeEngine &engine, ::Engines::Console &console, Aurora::Platform platform) :
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
	try {
		_musicBank.reset(Sound::XACTWaveBank::load("musicbank"));
	} catch (...) {
		Common::exceptionDispatcherWarning();
	}

	_module.reset(new Module(*_console));

	while (!EventMan.quitRequested()) {
		mainMenu();
		runModule();
	}

	_module.reset();
}

void Game::runModule() {
	Common::ScopedPtr<Creature> fakePC(new Creature);
	fakePC->createFakePC();

	_module->usePC(fakePC.release());

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

void Game::playMenuMusic() {
	stopMenuMusic();

	if (_musicBank) {
		/* TODO: music.2da contains an entry for "mus_thm_MAINTHEME1", but this
		 * gives us a state number. We will probably need to throw that state
		 * number at the XACT SoundBank (xsb) somehow.
		 *
		 * Until that works, we'll get the main menu music by its index in the
		 * XACT WaveBank (xwb) instead.
		 */
		static const size_t mainMenuMusic = 46;

		try {
			Sound::AudioStream *stream = Sound::makeLoopingAudioStream(_musicBank->getWave(mainMenuMusic), 0);

			_menuMusic = SoundMan.playAudioStream(stream, Sound::kSoundTypeMusic);
			SoundMan.startChannel(_menuMusic);

		} catch (...) {
			Common::exceptionDispatcherWarning();
		}
	}
}

void Game::stopMenuMusic() {
	SoundMan.stopChannel(_menuMusic);
}

void Game::mainMenu() {
	EventMan.flushEvents();

	MainMenu menu(*_module, _console);

	_console->disableCommand("loadmodule", "not available in the main menu");
	_console->disableCommand("exitmodule", "not available in the main menu");

	menu.show();
	playMenuMusic();
	menu.run();
	stopMenuMusic();
	menu.hide();

	_console->enableCommand("loadmodule");
	_console->enableCommand("exitmodule");
}

void Game::getModules(std::vector<Common::UString> &modules) {
	modules.clear();

	std::list<Aurora::ResourceManager::ResourceID> ares;
	ResMan.getAvailableResources(Aurora::kFileTypeARE, ares);

	for (std::list<Aurora::ResourceManager::ResourceID>::const_iterator a = ares.begin(); a != ares.end(); ++a)
		modules.push_back(a->name);

	std::sort(modules.begin(), modules.end(), Common::UString::iless());
}

} // End of namespace Jade

} // End of namespace Engines
