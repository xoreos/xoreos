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

#include "aurora/types.h"
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
#include "engines/witcher/witcher.h"
#include "engines/sonic/sonic.h"
#include "engines/dragonage/dragonage.h"
#include "engines/dragonage2/dragonage2.h"

DECLARE_SINGLETON(Engines::EngineManager)

namespace Engines {

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
	&Witcher::kWitcherEngineProbe,
	&Sonic::kSonicEngineProbe,
	&DragonAge::kDragonAgeEngineProbe,
	&DragonAge2::kDragonAge2EngineProbe
};


GameInstance::GameInstance() {
}

GameInstance::~GameInstance() {
}


class GameInstanceEngine : public GameInstance {
public:
	~GameInstanceEngine();

	Common::UString getGameName(bool platform) const;

private:
	Common::UString _target;

	const EngineProbe *_probe;
	Engine *_engine;

	GameInstanceEngine(const Common::UString &target);

	/** Find an engine capable of running the game found in the GameInstance's target. */
	bool probe();
	/** Reset the GameInstance to a pre-probe state. */
	void reset();

	/** Run the probed game in the GameInstance's target. */
	void run();

	bool probe(const Common::FileList &rootFiles);
	bool probe(Common::SeekableReadStream &stream);

	void createEngine();
	void destroyEngine();

	friend class EngineManager;
};

GameInstanceEngine::GameInstanceEngine(const Common::UString &target) : _target(target),
	_probe(0), _engine(0) {
}

GameInstanceEngine::~GameInstanceEngine() {
	delete _engine;
}

void GameInstanceEngine::reset() {
	destroyEngine();
	_probe = 0;
}

bool GameInstanceEngine::probe() {
	if (Common::FilePath::isDirectory(_target)) {
		// Try to probe from that directory

		Common::FileList rootFiles;

		if (!rootFiles.addDirectory(_target))
			// Fatal: can't read the directory
			return false;

		return probe(rootFiles);
	}

	if (Common::FilePath::isRegularFile(_target)) {
		// Try to probe from that file

		Common::File file;
		if (file.open(_target))
			return probe(file);
	}

	return false;
}

bool GameInstanceEngine::probe(const Common::FileList &rootFiles) {
	// Try to find the first engine able to handle the directory's data
	for (int i = 0; i < ARRAYSIZE(kProbes); i++) {
		if (kProbes[i]->probe(_target, rootFiles)) {
			_probe = kProbes[i];
			return true;
		}
	}

	return false;
}

bool GameInstanceEngine::probe(Common::SeekableReadStream &stream) {
	// Try to find the first engine able to handle the stream's data
	for (int i = 0; i < ARRAYSIZE(kProbes); i++) {
		if (kProbes[i]->probe(stream)) {
			_probe = kProbes[i];
			return true;
		}
	}

	return false;
}

Common::UString GameInstanceEngine::getGameName(bool platform) const {
	if (!_probe)
		return "";

	Common::UString gameName = _probe->getGameName();
	if (platform)
		gameName += " (" + Aurora::getPlatformDescription(_probe->getPlatform()) + ")";

	return gameName;
}

void GameInstanceEngine::createEngine() {
	if (!_probe)
		throw Common::Exception("GameInstanceEngine::createEngine(): No game probed");

	destroyEngine();
	_engine = _probe->createEngine();
}

void GameInstanceEngine::destroyEngine() {
	delete _engine;
	_engine = 0;
}

void GameInstanceEngine::run() {
	createEngine();

	_engine->start(_probe->getGameID(), _target, _probe->getPlatform());

	destroyEngine();
}


GameInstance *EngineManager::probeGame(const Common::UString &target) const {
	GameInstanceEngine *game = new GameInstanceEngine(target);
	if (game->probe())
		return game;

	delete game;
	return 0;
}

void EngineManager::run(GameInstance &game) const {
	GameInstanceEngine *gameEngine = dynamic_cast<GameInstanceEngine *>(&game);
	assert(gameEngine);

	try {
		gameEngine->run();
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
	GameInstanceEngine *gameEngine = dynamic_cast<GameInstanceEngine *>(&game);
	assert(gameEngine);

	try {
		gameEngine->reset();

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
