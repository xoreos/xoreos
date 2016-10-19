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

#ifndef COMMON_MUTEX_H
#define COMMON_MUTEX_H

#include <SDL_thread.h>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"

namespace Common {

/** A mutex. */
class Mutex : boost::noncopyable {
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:
	SDL_mutex *_mutex;

	friend class Condition;
};

/** A semaphore . */
class Semaphore : boost::noncopyable {
public:
	Semaphore(uint value = 0);
	~Semaphore();

	bool lock(uint32 timeout = 0);
	bool lockTry();
	void unlock();

	uint32 getValue();

private:
	SDL_sem *_semaphore;
};

/** Convenience class that locks a mutex on creation and unlocks it on destruction. */
class StackLock : boost::noncopyable {
public:
	StackLock(Mutex &mutex);
	StackLock(Semaphore &semaphore);
	~StackLock();

private:
	Mutex     *_mutex;
	Semaphore *_semaphore;
};

/** A condition. */
class Condition : boost::noncopyable {
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
