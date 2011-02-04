/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/mutex.h
 *  Thread mutex classes.
 */

#ifndef COMMON_MUTEX_H
#define COMMON_MUTEX_H

#include <SDL_thread.h>

#include "common/types.h"

namespace Common {

/** A mutex. */
class Mutex {
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:
	SDL_mutex *_mutex;

	friend class Condition;
};

/** Convenience class that locks a mutex on creation and unlocks it on destruction. */
class StackLock {
public:
	StackLock(Mutex &mutex);
	~StackLock();

private:
	Mutex *_mutex;
};

/** A condition. */
class Condition {
public:
	Condition();
	Condition(Mutex &mutex);
	~Condition();

	bool wait(uint32 timeout = 0);
	void signal();

private:
	bool _ownMutex;

	Mutex *_mutex;
	SDL_cond *_condition;
};

} // End of namespace Common

#endif // COMMON_MUTEX_H
