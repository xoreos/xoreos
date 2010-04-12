/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

	if (EventMan.isMainThread())
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

RequestID RequestManager::fullscreen(bool fullscreen) {
	return newRequest(kITCEventFullscreen);
}

RequestID RequestManager::resize(int width, int height) {
	return newRequest(kITCEventWindowed);
}

RequestID RequestManager::createTextures(uint32 n, Graphics::TextureID *ids) {
	RequestID rID = newRequest(kITCEventCreateTextures);

	(*rID)->_createTextures.count    = n;
	(*rID)->_createTextures.replyIDs = ids;

	(*rID)->_createTextures.ids = new Graphics::TextureID[n];

	return rID;
}

RequestID RequestManager::destroyTextures(uint32 n, Graphics::TextureID *ids) {
	RequestID rID = newRequest(kITCEventDestroyTextures);

	(*rID)->_destroyTextures.count = n;

	(*rID)->_destroyTextures.ids = new Graphics::TextureID[n];
	memcpy((*rID)->_destroyTextures.ids, ids, n * sizeof(Graphics::TextureID));

	return rID;
}

RequestID RequestManager::loadTexture(Graphics::TextureID id, const Graphics::ImageDecoder *image) {
	RequestID rID = newRequest(kITCEventLoadTexture);

	(*rID)->_loadTexture.id     = id;
	(*rID)->_loadTexture.width  = image->getWidth();
	(*rID)->_loadTexture.height = image->getHeight();
	(*rID)->_loadTexture.data   = image->getData();
	(*rID)->_loadTexture.format = image->getFormat();

	return rID;
}

RequestID RequestManager::isTexture(Graphics::TextureID id, bool *answer) {
	RequestID rID = newRequest(kITCEventIsTexture);

	(*rID)->_isTexture.id          = id;
	(*rID)->_isTexture.replyAnswer = answer;

	return rID;
}

RequestID RequestManager::newRequest(ITCEvent type) {
	Common::StackLock lock(_mutexUse);

	_requests.push_back(new Request(type));

	RequestID rID = --_requests.end();

	(*rID)->create();

	return rID;
}

void RequestManager::clearList() {
	Common::StackLock lock(_mutexUse);

	for (RequestList::iterator request = _requests.begin(); request != _requests.begin(); ++request) {
		warning("Clear request: %d", (*request)->_type);
		delete *request;
	}

	_requests.clear();
}

void RequestManager::collectGarbage() {
	Common::StackLock lock(_mutexUse);

	RequestList::iterator request = _requests.begin();
	while (request != _requests.end()) {
		if ((*request)->isGarbage()) {
			warning("Claiming request: %d", (*request)->_type);
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
