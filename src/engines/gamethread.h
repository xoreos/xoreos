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

#ifndef ENGINES_GAMETHREAD_H
#define ENGINES_GAMETHREAD_H

#include <list>

#include "src/common/scopedptr.h"
#include "src/common/thread.h"

namespace Engines {

class GameInstance;
class EngineProbe;

/** The game thread, running all game logic. */
class GameThread : public Common::Thread {
public:
	GameThread();
	~GameThread();

	void init(const Common::UString &baseDir, const std::list<const EngineProbe *> &probes);

	void run();

private:
	Common::ScopedPtr<GameInstance> _game;

	void threadMethod();
};

} // End of namespace Engines

#endif // ENGINES_GAMETHREAD_H
