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

/** @file engines/enginemanager.h
 *  The global engine manager, omniscient about all engines
 */

#ifndef ENGINES_ENGINEMANAGER_H
#define ENGINES_ENGINEMANAGER_H

#include "common/ustring.h"
#include "common/singleton.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
	class FileList;
}

namespace Engines {

class Engine;

class GameInstance {
public:
	GameInstance(const Common::UString &target);
	~GameInstance();

private:
	Common::UString _target;

	Aurora::GameID   _gameID;
	Aurora::Platform _platform;

	Engine *_engine;

	friend class EngineManager;
};

/** The global engine manager. */
class EngineManager : public Common::Singleton<EngineManager> {
public:
	/** Find an engine capable of running the specified game.
	 *
	 *  @param game The game game containing the target file or directory.
	 *  @return true if an engine capable of running the game was found.
	 */
	bool probeGame(GameInstance &game) const;

	/** Create and initialize the specified game's engine. */
	void createEngine(GameInstance &game) const;

	/** Run the specified game. */
	void run(GameInstance &game) const;

	/** Return the full game name to that game. */
	Common::UString getGameName(GameInstance &game, bool platform = false) const;

private:
	bool probeGame(GameInstance &game, const Common::FileList &rootFiles) const;
	bool probeGame(GameInstance &game, Common::SeekableReadStream &stream) const;

	void cleanup(GameInstance &game) const;
};

} // End of namespace Engines

/** Shortcut for accessing the sound manager. */
#define EngineMan Engines::EngineManager::instance()

#endif // ENGINES_ENGINEMANAGER_H
