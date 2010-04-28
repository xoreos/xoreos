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


Condition::Condition() : _ownMutex(true) {
	_mutex = new Mutex;

	_condition = SDL_CreateCond();

	assert(_condition);
}

Condition::Condition(Mutex &mutex) : _ownMutex(false), _mutex(&mutex) {
	_condition = SDL_CreateCond();

	assert(_condition);
}

Condition::~Condition() {
	SDL_DestroyCond(_condition);

	if (_ownMutex)
		delete _mutex;
}

bool Condition::wait(uint32 timeout) {
	int gotSignal;

	if (_ownMutex)
		_mutex->lock();

	if (timeout == 0)
		gotSignal = SDL_CondWait(_condition, _mutex->_mutex);
	else
		gotSignal = SDL_CondWaitTimeout(_condition, _mutex->_mutex, timeout);

	if (_ownMutex)
		_mutex->unlock();

	return gotSignal == 0;
}

void Condition::signal() {
	SDL_CondSignal(_condition);
}

} // End of namespace Common
