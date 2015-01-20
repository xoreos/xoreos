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

/** @file engines/enginemanager.cpp
 *  The global engine manager, omniscient about all engines
 */

#include "common/util.h"
#include "common/ustring.h"
#include "common/file.h"
#include "common/filelist.h"
#include "common/filepath.h"
#include "common/debugman.h"
#include "common/configman.h"

#include "aurora/error.h"
#include "aurora/resman.h"
#include "aurora/talkman.h"
#include "aurora/2dareg.h"
#include "../aurora/util.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/fontman.h"
#include "graphics/aurora/textureman.h"

#include "events/events.h"
#include "events/requests.h"

#include "engines/enginemanager.h"
#include "engines/engineprobe.h"

#include "engines/aurora/tokenman.h"
#include "engines/aurora/model.h"

// The engines
#include "engines/nwn/nwn.h"
#include "engines/nwn2/nwn2.h"
#include "engines/kotor/kotor.h"
#include "engines/kotor2/kotor2.h"
#include "engines/jade/jade.h"
#include "engines/thewitcher/thewitcher.h"
#include "engines/sonic/sonic.h"
#include "engines/dragonage/dragonage.h"
#include "engines/dragonage2/dragonage2.h"

DECLARE_SINGLETON(Engines::EngineManager)

namespace Engines {

GameInstance::GameInstance(const Common::UString &target) : _target(target),
	_gameID(Aurora::kGameIDUnknown), _platform(Aurora::kPlatformUnknown), _engine(0) {

}

GameInstance::~GameInstance() {
	delete _engine;
}


static const EngineProbe *kProbes[] = {
	&NWN::kNWNEngineProbeLinux,
	&NWN::kNWNEngineProbeMac,
	&NWN::kNWNEngineProbeWin,
	&NWN::kNWNEngineProbeFallback,
	&NWN2::kNWN2EngineProbe,
	&KotOR::kKotOREngineProbeWin,
	&KotOR::kKotOREngineProbeMac,
	&KotOR::kKotOREngineProbeXbox,
	&KotOR2::kKotOR2EngineProbeWin,
	&KotOR2::kKotOR2EngineProbeXbox,
	&Jade::kJadeEngineProbe,
	&TheWitcher::kTheWitcherEngineProbe,
	&Sonic::kSonicEngineProbe,
	&DragonAge::kDragonAgeEngineProbe,
	&DragonAge2::kDragonAge2EngineProbe
};

bool EngineManager::probeGame(GameInstance &game) const {
	game._gameID   = Aurora::kGameIDUnknown;
	game._platform = Aurora::kPlatformUnknown;

	if (Common::FilePath::isDirectory(game._target)) {
		// Try to probe from that directory

		Common::FileList rootFiles;

		if (!rootFiles.addDirectory(game._target))
			// Fatal: can't read the directory
			return false;

		return probeGame(game, rootFiles);
	}

	if (Common::FilePath::isRegularFile(game._target)) {
		// Try to probe from that file

		Common::File file;
		if (file.open(game._target))
			return probeGame(game, file);
	}

	return false;
}

bool EngineManager::probeGame(GameInstance &game, const Common::FileList &rootFiles) const {
	// Try to find the first engine able to handle the directory's data
	for (int i = 0; i < ARRAYSIZE(kProbes); i++) {

		if (kProbes[i]->probe(game._target, rootFiles)) {
			// Found one

			game._gameID   = kProbes[i]->getGameID();
			game._platform = kProbes[i]->getPlatform();

			return true;
		}

	}

	// None found
	return false;
}

bool EngineManager::probeGame(GameInstance &game, Common::SeekableReadStream &stream) const {
	// Try to find the first engine able to handle the stream's data
	for (int i = 0; i < ARRAYSIZE(kProbes); i++) {

		if (kProbes[i]->probe(stream)) {
			// Found one

			game._gameID   = kProbes[i]->getGameID();
			game._platform = kProbes[i]->getPlatform();

			return true;
		}

	}

	// None found
	return false;
}

Common::UString EngineManager::getGameName(GameInstance &game, bool platform) const {
	Common::UString gameName;

	for (int i = 0; i < ARRAYSIZE(kProbes); i++) {
		if (kProbes[i]->getGameID() == game._gameID) {
			gameName = kProbes[i]->getGameName();

			if (platform)
				gameName += " (" + Aurora::getPlatformDescription(game._platform) + ")";

			break;
		}
	}

	return gameName;
}

void EngineManager::createEngine(GameInstance &game) const {
	if (game._engine)
		return;

	for (int i = 0; i < ARRAYSIZE(kProbes); i++) {
		if ((kProbes[i]->getGameID()   == game._gameID) &&
		    (kProbes[i]->getPlatform() == game._platform)) {

			game._engine = kProbes[i]->createEngine();
			break;
		}
	}

	if (!game._engine)
		throw Common::Exception("Failed to create the engine for GameID %d", game._gameID);
}

void EngineManager::run(GameInstance &game) const {
	try {
		createEngine(game);

		game._engine->run(game._target);
		EventMan.requestQuit();

		// Clean up after the engine
		cleanup(game);

		EventMan.doQuit();

	} catch(...) {
		EventMan.requestQuit();

		// Clean up after the engine
		cleanup(game);

		EventMan.doQuit();

		throw;
	}

}

void EngineManager::cleanup(GameInstance &game) const {
	try {
		delete game._engine;
		game._engine = 0;

		DebugMan.clearEngineChannels();

		unregisterModelLoader();

		RequestMan.sync();

		FontMan.clear();
		CursorMan.clear();
		TextureMan.clear();

		TokenMan.clear();

		TalkMan.clear();
		TwoDAReg.clear();
		ResMan.clear();

		ConfigMan.setGame();

	} catch (...) {
	}
}

} // End of namespace Engines
