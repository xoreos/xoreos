/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file events/requests.cpp
 *  Inter-thread request events.
 */

#include "common/error.h"
#include "common/util.h"
#include "common/threads.h"

#include "events/requests.h"
#include "events/events.h"

#include "graphics/images/decoder.h"

DECLARE_SINGLETON(Events::RequestManager);

namespace Events {

RequestManager::~RequestManager() {
	clearList();
}

void RequestManager::init() {
	clearList();

	if (!createThread())
		throw Common::Exception("Failed to create requests thread: %s", SDL_GetError());
}

void RequestManager::deinit() {
	if (!destroyThread())
		warning("RequestManager::deinit(): Requests thread had to be killed");

	clearList();
}

void RequestManager::dispatch(RequestID request) {
	Common::StackLock lock(_mutexUse);

	if ((*request)->_dispatched)
		// We are already waiting for an answer
		return;

	// Set state
	(*request)->_dispatched = true;

	// And send the event
	if (!EventMan.pushEvent((*request)->_event))
		throw Common::Exception("Failed dispatching request");

	if (Common::isMainThread())
		// If we're currently in the main thread, to avoid a dead-lock, process events now
		EventMan.processEvents();
}

void RequestManager::waitReply(RequestID request) {
	// Locking our use mutex, to prevent race conditions
	_mutexUse.lock();

	if (!(*request)->_dispatched) {
		// The request either wasn't yet dispatched, or we've already gotten a reply

		// Copy the reply (if any) to the reply memory
		(*request)->copyToReply();

		_mutexUse.unlock();
		return;
	}

	// Lock the mutex for the condition
	(*request)->_mutexReply.lock();

	// We don't need our use mutex now
	_mutexUse.unlock();

	// Wait for a reply
	(*request)->_hasReply->wait();

	// Got a reply

	// Now we need the use mutex again
	_mutexUse.lock();

	// Unlock the relocked reply mutex
	(*request)->_mutexReply.unlock();

	// Copy the reply (if any) to the reply memory
	(*request)->copyToReply();

	// And mark the request as garbage, so that it may be collected
	(*request)->setGarbage();

	// And finally give the use mutex free again
	_mutexUse.unlock();
}

void RequestManager::forget(RequestID request) {
	Common::StackLock lock(_mutexUse);

	(*request)->setGarbage();
}

void RequestManager::dispatchAndWait(RequestID request) {
	dispatch(request);
	waitReply(request);
}

void RequestManager::dispatchAndForget(RequestID request) {
	dispatch(request);
	forget(request);
}

void RequestManager::sync() {
	RequestID syncID = newRequest(kITCEventSync);

	dispatchAndWait(syncID);
}

RequestID RequestManager::fullscreen(bool fs) {
	if (fs)
		return newRequest(kITCEventFullscreen);
	else
		return newRequest(kITCEventWindowed);
}

RequestID RequestManager::resize(int width, int height) {
	RequestID rID = newRequest(kITCEventResize);

	(*rID)->_resize.width  = width;
	(*rID)->_resize.height = height;

	return rID;
}

RequestID RequestManager::changeFSAA(int level) {
	RequestID rID = newRequest(kITCEventChangeFSAA);

	(*rID)->_fsaa.level = level;

	return rID;
}

RequestID RequestManager::changeVSync(bool vsync) {
	RequestID rID = newRequest(kITCEventChangeVSync);

	(*rID)->_vsync.vsync = vsync;

	return rID;
}

RequestID RequestManager::rebuild(Graphics::GLContainer &glContainer) {
	RequestID rID = newRequest(kITCEventRebuildGLContainer);

	(*rID)->_glContainer.glContainer = &glContainer;

	return rID;
}

RequestID RequestManager::destroy(Graphics::GLContainer &glContainer) {
	RequestID rID = newRequest(kITCEventDestroyGLContainer);

	(*rID)->_glContainer.glContainer = &glContainer;

	return rID;
}

RequestID RequestManager::newRequest(ITCEvent type) {
	Common::StackLock lock(_mutexUse);

	_requests.push_back(new Request(type));

	return --_requests.end();
}

void RequestManager::clearList() {
	Common::StackLock lock(_mutexUse);

	for (RequestList::iterator request = _requests.begin(); request != _requests.end(); ++request)
		delete *request;
	_requests.clear();
}

void RequestManager::collectGarbage() {
	Common::StackLock lock(_mutexUse);

	RequestList::iterator request = _requests.begin();
	while (request != _requests.end()) {
		if ((*request)->isGarbage()) {
			delete *request;
			request = _requests.erase(request);
		} else
			++request;
	}
}

void RequestManager::threadMethod() {
	while (!_killThread) {
		collectGarbage();
		EventMan.delay(100);
	}
}

} // End of namespace Events
