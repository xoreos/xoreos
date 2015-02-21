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
 *  Thread mutex classes.
 */

#include <cassert>

#include "src/common/mutex.h"

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
	SDL_UnlockMutex(_mutex);
}


Semaphore::Semaphore(uint value) {
	_semaphore = SDL_CreateSemaphore(value);
}

Semaphore::~Semaphore() {
	SDL_DestroySemaphore(_semaphore);
}

bool Semaphore::lock(uint32 timeout) {
	int ret;

	if (timeout == 0)
		ret = SDL_SemWait(_semaphore);
	else
		ret = SDL_SemWaitTimeout(_semaphore, timeout);

	return ret == 0;
}

bool Semaphore::lockTry() {
	return SDL_SemTryWait(_semaphore) == 0;
}

void Semaphore::unlock() {
	SDL_SemPost(_semaphore);
}

uint32 Semaphore::getValue() {
	return SDL_SemValue(_semaphore);
}


StackLock::StackLock(Mutex &mutex) : _mutex(&mutex), _semaphore(0) {
	_mutex->lock();
}

StackLock::StackLock(Semaphore &semaphore) : _mutex(0), _semaphore(&semaphore) {
	_semaphore->lock();
}

StackLock::~StackLock() {
	if (_mutex)
		_mutex->unlock();
	if (_semaphore)
		_semaphore->unlock();
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
