/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/thread.cpp
 *  Threading helpers.
 */

#include <SDL_timer.h>

#include "common/thread.h"

namespace Common {

Thread::Thread() {
	_thread = 0;

	_killThread    = false;
	_threadRunning = false;
}

Thread::~Thread() {
	destroyThread();
}

bool Thread::createThread() {
	if (_threadRunning)
		// Already running, nothing to do
		return true;

	// Try to create the thread
	if (!(_thread = SDL_CreateThread(threadHelper, (void *) this)))
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

	// Still running? Just kill the bugger, then
	SDL_KillThread(_thread);

	_killThread    = false;
	_threadRunning = false;

	return false;
}

int Thread::threadHelper(void *obj) {
	Thread *thread = (Thread *) obj;

	// The thread is running.
	thread->_threadRunning = true;

	// Run the thread
	thread->threadMethod();

	// Thead thread is not running.
	thread->_threadRunning = false;

	return 0;
}

} // End of namespace Common
