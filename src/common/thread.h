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

#include <SDL_thread.h>

#include <boost/noncopyable.hpp>

#include "src/common/ustring.h"

namespace Common {

/** A class that creates its own thread. */
class Thread : boost::noncopyable {
public:
	Thread();
	virtual ~Thread();

	bool createThread(const UString &name = "");
	bool destroyThread();

protected:
	volatile bool _killThread;

private:
	SDL_Thread *_thread;

	bool _threadRunning;

	virtual void threadMethod() = 0;

	static int threadHelper(void *obj);
};

} // End of namespace Common

#endif // COMMON_THREAD_H
