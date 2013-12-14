/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/threads.cpp
 *  Threading system helpers.
 */

#ifndef COMMON_THREADS_H
#define COMMON_THREADS_H

#include <SDL_thread.h>

#include "common/types.h"
#include "common/error.h"

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

#endif // COMMON_THREADS_H
