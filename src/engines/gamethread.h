/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/gamethread.h
 *  The thread the game logic runs in.
 */

#ifndef ENGINES_GAMETHREAD_H
#define ENGINES_GAMETHREAD_H

#include "common/ustring.h"
#include "common/thread.h"

#include "aurora/types.h"

namespace Engines {

/** The game thread, running all game logic. */
class GameThread : public Common::Thread {
public:
	GameThread();
	~GameThread();

	void init(const Common::UString &baseDir);

	void run();

private:
	Common::UString _baseDir;
	Aurora::GameID _gameID;

	void threadMethod();
};

} // End of namespace Engines

#endif // ENGINES_GAMETHREAD_H
