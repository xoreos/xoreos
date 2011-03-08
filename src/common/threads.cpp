/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
static uint32 threadsMainID;

namespace Common {

void initThreads() {
	assert(!threadsInited);

	threadsInited = true;
	threadsMainID = SDL_ThreadID();
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
