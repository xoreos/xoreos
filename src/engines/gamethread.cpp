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
 *  The thread the game logic runs in.
 */

#include <cassert>

#include "src/version/version.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/configman.h"

#include "src/engines/gamethread.h"
#include "src/engines/enginemanager.h"

#include "src/graphics/windowman.h"

#include "src/events/events.h"

namespace Engines {

GameThread::GameThread() {
}

GameThread::~GameThread() {
	destroyThread();
}

void GameThread::init(const Common::UString &baseDir, const std::list<const EngineProbe *> &probes) {
	// Detecting the game

	_game.reset(EngineMan.probeGame(baseDir, probes));
	if (!_game)
		throw Common::Exception("Unable to detect the game");

	// Get the game description from the config, or alternatively
	// construct one from the game name and platform.
	Common::UString description;
	if (!ConfigMan.getKey("description", description))
		description = _game->getGameName(true);

	WindowMan.setWindowTitle(Common::UString(Version::getProjectName()) + " -- " + description);

	status("Detected game \"%s\"", _game->getGameName(false).c_str());
}

void GameThread::run() {
	if (ConfigMan.getBool("listlangs", false)) {
		assert(_game);

		EngineMan.listLanguages(*_game);

		EventMan.requestQuit();
		EventMan.doQuit();
		return;
	}

	if (!createThread("GameInstance"))
		throw Common::Exception("Failed creating game logic thread");
}

void GameThread::threadMethod() {
	assert(_game);

	try {
		EngineMan.run(*_game);
	} catch (...) {
		Common::exceptionDispatcherError();
	}

	EventMan.doQuit();
}

} // End of namespace Engines
