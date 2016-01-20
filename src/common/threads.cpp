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
 *  Threading system helpers.
 *
 *  See also class Thread in thread.h.
 */

#include <cassert>

#include <SDL_thread.h>

#include "src/common/types.h"
#include "src/common/error.h"
#include "src/common/threads.h"

static bool   threadsInited = false;
static SDL_threadID threadsMainID;

namespace Common {

void initThreads() {
	assert(!threadsInited);

	threadsInited = true;
	threadsMainID = SDL_ThreadID();
}

bool initedThreads() {
	return threadsInited;
}

bool isMainThread() {
	assert(threadsInited);

	return SDL_ThreadID() == threadsMainID;
}

void enforceMainThread() {
	if (!isMainThread())
		throw Exception("Unsafe function called in non-main thread");
}

} // End of namespace Common
