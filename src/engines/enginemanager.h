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

namespace Common {
	class SeekableReadStream;
	class FileList;
}

namespace Engines {

class GameInstance {
public:
	GameInstance();
	virtual ~GameInstance();

	virtual Common::UString getGameName(bool platform) const = 0;
};

/** The global engine manager. */
class EngineManager : public Common::Singleton<EngineManager> {
public:
	/** Return a game instance capable of running the game in this directory. */
	GameInstance *probeGame(const Common::UString &target) const;

	/** Run this game instance. */
	void run(GameInstance &game) const;

private:
	void cleanup(GameInstance &game) const;
};

} // End of namespace Engines

/** Shortcut for accessing the sound manager. */
#define EngineMan Engines::EngineManager::instance()

#endif // ENGINES_ENGINEMANAGER_H
