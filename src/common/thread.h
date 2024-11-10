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

#ifndef COMMON_THREAD_H
#define COMMON_THREAD_H

#if defined(__MINGW32__ ) && !defined(_GLIBCXX_HAS_GTHREADS)
	#include "external/mingw-std-threads/mingw.thread.h"
#else
	#include <thread>
#endif

#include <atomic>

#include "src/common/ustring.h"

namespace Common {

/** A class that creates its own thread. */
class Thread {
public:
	Thread();
	virtual ~Thread();

	Thread(const Thread &) = delete;
	Thread &operator=(const Thread &) = delete;

	bool createThread(const UString &name = "");
	bool destroyThread();

	/**
	 * Set the name of the thread, if available on the platform.
	 * Otherwise, a no-op. If the name is longer than the platform
	 * supports, it is truncated to fit.
	 */
	static void setCurrentThreadName(const Common::UString &name);

protected:
	std::atomic<bool> _killThread;

private:
	std::thread _thread;
	Common::UString _name;

	std::atomic<bool> _threadRunning;

	virtual void threadMethod() = 0;

	static int threadHelper(void *obj);
};

} // End of namespace Common

#endif // COMMON_THREAD_H
