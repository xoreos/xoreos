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
 *  The global engine manager, omniscient about all engines
 */

#include <cassert>

#include "src/common/scopedptr.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/readfile.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/debugman.h"
#include "src/common/configman.h"

#include "src/aurora/types.h"
#include "src/aurora/util.h"
#include "src/aurora/language.h"
#include "src/aurora/resman.h"
#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/textureman.h"

#include "src/events/events.h"
#include "src/events/requests.h"

#include "src/engines/enginemanager.h"
#include "src/engines/engineprobe.h"

#include "src/engines/aurora/tokenman.h"
#include "src/engines/aurora/model.h"

#include "src/engines/engine.h"

DECLARE_SINGLETON(Engines::EngineManager)

namespace Engines {

GameInstance::GameInstance() {
}

GameInstance::~GameInstance() {
}


class GameInstanceEngine : public GameInstance {
public:
	GameInstanceEngine(const Common::UString &target);
	~GameInstanceEngine();

	Common::UString getGameName(bool platform) const;

	/** Find an engine capable of running the game found in the GameInstance's target. */
	bool probe(const std::list<const EngineProbe *> &probes);
	/** Reset the GameInstance to a pre-probe state. */
	void reset();

	/** Run the probed game in the GameInstance's target. */
	void run();

	/** List all available languages supported by this GameInstance's target. */
	void listLanguages();

private:
	Common::UString _target;

	const EngineProbe *_probe;
	Common::ScopedPtr<Engine> _engine;

	bool probe(const Common::FileList &rootFiles, const std::list<const EngineProbe *> &probes);
	bool probe(Common::SeekableReadStream &stream, const std::list<const EngineProbe *> &probes);

	void createEngine();
	void destroyEngine();
};

GameInstanceEngine::GameInstanceEngine(const Common::UString &target) : _target(target), _probe(0) {
}

GameInstanceEngine::~GameInstanceEngine() {
}

void GameInstanceEngine::reset() {
	destroyEngine();
	_probe = 0;
}

bool GameInstanceEngine::probe(const std::list<const EngineProbe *> &probes) {
	if (Common::FilePath::isDirectory(_target)) {
		// Try to probe from that directory

		Common::FileList rootFiles;

		if (!rootFiles.addDirectory(_target))
			// Fatal: can't read the directory
			return false;

		return probe(rootFiles, probes);
	}

	if (Common::FilePath::isRegularFile(_target)) {
		// Try to probe from that file

		Common::ReadFile file;
		if (file.open(_target))
			return probe(file, probes);
	}

	return false;
}

bool GameInstanceEngine::probe(const Common::FileList &rootFiles,
                               const std::list<const EngineProbe *> &probes) {

	// Try to find the first engine able to handle the directory's data
	for (std::list<const EngineProbe *>::const_iterator p = probes.begin(); p != probes.end(); ++p) {
		if ((*p)->probe(_target, rootFiles)) {
			_probe = *p;
			return true;
		}
	}

	return false;
}

bool GameInstanceEngine::probe(Common::SeekableReadStream &stream,
                               const std::list<const EngineProbe *> &probes) {

	// Try to find the first engine able to handle the directory's data
	for (std::list<const EngineProbe *>::const_iterator p = probes.begin(); p != probes.end(); ++p) {
		if ((*p)->probe(stream)) {
			_probe = *p;
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
	_engine.reset(_probe->createEngine());
}

void GameInstanceEngine::destroyEngine() {
	_engine.reset();
}

void GameInstanceEngine::listLanguages() {
	createEngine();

	std::vector<Aurora::Language> langs;
	if (_engine->detectLanguages(_probe->getGameID(), _target, _probe->getPlatform(), langs)) {
		if (!langs.empty()) {
			info("Available languages:");
			for (std::vector<Aurora::Language>::iterator l = langs.begin(); l != langs.end(); ++l)
				info("- %s", LangMan.getLanguageName(*l).c_str());
		}
	}

	std::vector<Aurora::Language> langsT, langsV;
	if (_engine->detectLanguages(_probe->getGameID(), _target, _probe->getPlatform(), langsT, langsV)) {
		if (!langsT.empty()) {
			info("Available text languages:");
			for (std::vector<Aurora::Language>::iterator l = langsT.begin(); l != langsT.end(); ++l)
				info("- %s", LangMan.getLanguageName(*l).c_str());
		}

		if (!langsV.empty()) {
			info("Available voice languages:");
			for (std::vector<Aurora::Language>::iterator l = langsV.begin(); l != langsV.end(); ++l)
				info("- %s", LangMan.getLanguageName(*l).c_str());
		}
	}

	destroyEngine();
}

void GameInstanceEngine::run() {
	createEngine();

	_engine->start(_probe->getGameID(), _target, _probe->getPlatform());

	destroyEngine();
}


GameInstance *EngineManager::probeGame(const Common::UString &target,
                                       const std::list<const EngineProbe *> &probes) const {

	Common::ScopedPtr<GameInstanceEngine> game(new GameInstanceEngine(target));
	if (game->probe(probes))
		return game.release();

	return 0;
}

void EngineManager::listLanguages(GameInstance &game) const {
	GameInstanceEngine *gameEngine = dynamic_cast<GameInstanceEngine *>(&game);
	assert(gameEngine);

	gameEngine->listLanguages();
}

void EngineManager::run(GameInstance &game) const {
	GameInstanceEngine *gameEngine = dynamic_cast<GameInstanceEngine *>(&game);
	assert(gameEngine);

	gameEngine->run();

	GfxMan.lockFrame();
	EventMan.requestQuit();
	GfxMan.unlockFrame();

	cleanup();
}

void EngineManager::cleanup() const {
	try {
		unregisterModelLoader();

		RequestMan.sync();

		FontMan.clear();
		CursorMan.clear();
		TextureMan.clear();

		TokenMan.clear();

		LangMan.clear();
		TalkMan.clear();
		TwoDAReg.clear();
		ResMan.clear();

		ConfigMan.setGame();

	} catch (...) {
	}
}

} // End of namespace Engines
