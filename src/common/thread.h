/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/thread.h
 *  Threading helpers.
 */

#ifndef COMMON_THREAD_H
#define COMMON_THREAD_H

#include <SDL_thread.h>

#include "common/noncopyable.h"

namespace Common {

class Thread : NonCopyable {
public:
	Thread();
	~Thread();

	bool createThread();
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
