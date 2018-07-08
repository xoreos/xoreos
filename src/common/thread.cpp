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

#include "src/common/fallthrough.h"
START_IGNORE_IMPLICIT_FALLTHROUGH
#include <SDL_timer.h>
STOP_IGNORE_IMPLICIT_FALLTHROUGH

#include "src/common/thread.h"
#include "src/common/util.h"

namespace Common {

Thread::Thread() : _killThread(false), _thread(0), _threadRunning(false) {
}

Thread::~Thread() {
	destroyThread();
}

bool Thread::createThread(const UString &name) {
	if (_threadRunning.load(boost::memory_order_relaxed)) {
		if (_name == name) {
			warning("Thread::createThread(): Thread \"%s\" already running", _name.c_str());
			return true;
		}

		warning("Thread::createThread(): Thread \"%s\" already running and trying to rename to \"%s\"", _name.c_str(), name.c_str());
		return false;
	}

	_name = name;

	// Try to create the thread
	if (!(_thread = SDL_CreateThread(threadHelper, _name.empty() ? 0 : _name.c_str(), static_cast<void *>(this))))
		return false;

	return true;
}

bool Thread::destroyThread() {
	if (!_threadRunning.load(boost::memory_order_seq_cst))
		return true;

	// Signal the thread that it should die
	_killThread.store(true, boost::memory_order_seq_cst);

	// Wait a whole second for the thread to finish on its own
	for (int i = 0; _threadRunning.load(boost::memory_order_seq_cst) && (i < 100); i++)
		SDL_Delay(10);

	_killThread.store(false, boost::memory_order_seq_cst);

	const bool stillRunning = _threadRunning.load(boost::memory_order_seq_cst);

	/* Clean up the thread if it's not still running. If the thread is still running,
	 * this would block, potentially indefinitely, so we leak instead in that case.
	 *
	 * We could use SDL_DetachThread() instead, but:
	 * - This only means that thread resources will be released once the thread
	 *   ends. We waited for a whole second, so it might just not.
	 * - SDL_DetachThread() was added in SDL 2.0.2, so we'd need to bump the
	 *   minimum SDL version from 2.0.0 to 2.0.2. Not worth it for this case, IMHO. */
	if (!stillRunning)
		SDL_WaitThread(_thread, 0);

	_thread = 0;

	/* TODO:: If we get threads that we start and stop multiple times within the runtime
	 *        of xoreos, we might need to do something more aggressive here, like throw. */
	if (stillRunning) {
		warning("Thread::destroyThread(): Thread \"%s\" still running", _name.c_str());
		return false;
	}

	return true;
}

int Thread::threadHelper(void *obj) {
	Thread *thread = static_cast<Thread *>(obj);

	// The thread is running.
	thread->_threadRunning.store(true, boost::memory_order_relaxed);

	// Run the thread
	thread->threadMethod();

	// Thread thread is not running.
	thread->_threadRunning.store(false, boost::memory_order_relaxed);

	return 0;
}

} // End of namespace Common
