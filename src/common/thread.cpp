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
 *  Threading helpers.
 *
 *  See also threads.h for the global threading system helpers.
 */

#include <SDL_timer.h>

#include "src/common/thread.h"

namespace Common {

Thread::Thread() {
	_thread = 0;

	_killThread    = false;
	_threadRunning = false;
}

Thread::~Thread() {
	destroyThread();
}

bool Thread::createThread(const UString &name) {
	if (_threadRunning)
		// Already running, nothing to do
		return true;

	// Try to create the thread
	if (!(_thread = SDL_CreateThread(threadHelper, name.empty() ? 0 : name.c_str(), static_cast<void *>(this))))
		return false;

	return true;
}

bool Thread::destroyThread() {
	if (!_threadRunning)
		return true;

	// Signal the thread that it should die
	_killThread = true;

	// Wait a whole second for the thread to finish on its own
	for (int i = 0; _threadRunning && (i < 100); i++)
		SDL_Delay(10);

	if (!_threadRunning) {
		// Wait for everything to settle
		SDL_WaitThread(_thread, 0);

		_killThread    = false;
		_threadRunning = false;

		return true;
	}

	/// FIXME: not sure if the thread is really killed

	_killThread    = false;
	_threadRunning = false;

	return false;
}

int Thread::threadHelper(void *obj) {
	Thread *thread = static_cast<Thread *>(obj);

	// The thread is running.
	thread->_threadRunning = true;

	// Run the thread
	thread->threadMethod();

	// Thread thread is not running.
	thread->_threadRunning = false;

	return 0;
}

} // End of namespace Common
