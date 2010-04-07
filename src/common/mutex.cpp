/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/mutex.cpp
 *  Thread mutex classes.
 */

#include <cassert>

#include "common/mutex.h"

namespace Common {

Mutex::Mutex() {
	_mutex = SDL_CreateMutex();

	assert(_mutex);
}

Mutex::~Mutex() {
	SDL_DestroyMutex(_mutex);
}

void Mutex::lock() {
	int res = SDL_LockMutex(_mutex);

	assert(res == 0);
}

void Mutex::unlock() {
	int res = SDL_UnlockMutex(_mutex);

	assert(res == 0);
}


StackLock::StackLock(Mutex &mutex) : _mutex(&mutex) {
	_mutex->lock();
}

StackLock::~StackLock() {
	_mutex->unlock();
}

} // End of namespace Common
