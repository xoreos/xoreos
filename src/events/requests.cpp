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

void RequestManager::sync() {
	RequestID syncID = newRequest(kITCEventSync);

	dispatchAndWait(syncID);
}

RequestID RequestManager::fullscreen(bool fullscreen) {
	return newRequest(kITCEventFullscreen);
}

RequestID RequestManager::resize(int width, int height) {
	return newRequest(kITCEventWindowed);
}

RequestID RequestManager::loadTexture(Graphics::Texture *texture) {
	RequestID rID = newRequest(kITCEventLoadTexture);

	(*rID)->_loadTexture.texture = texture;

	return rID;
}

RequestID RequestManager::destroyTexture(Graphics::Texture *texture) {
	RequestID rID = newRequest(kITCEventDestroyTexture);

	(*rID)->_destroyTexture.texture   = texture;
	(*rID)->_destroyTexture.textureID = 0xFFFFFFFF;

	return rID;
}

RequestID RequestManager::destroyTexture(Graphics::TextureID textureID) {
	RequestID rID = newRequest(kITCEventDestroyTexture);

	(*rID)->_destroyTexture.texture   = 0;
	(*rID)->_destroyTexture.textureID = textureID;

	return rID;
}

RequestID RequestManager::buildLists(Graphics::ListContainer *lists) {
	RequestID rID = newRequest(kITCEventBuildLists);

	(*rID)->_buildLists.lists = lists;

	return rID;
}

RequestID RequestManager::destroyLists(Graphics::ListContainer *lists) {
	RequestID rID = newRequest(kITCEventDestroyLists);

	(*rID)->_destroyLists.lists = lists;

	return rID;
}

RequestID RequestManager::destroyLists(Graphics::ListID listID, uint32 count) {
	RequestID rID = newRequest(kITCEventDestroyLists);

	(*rID)->_destroyLists.lists   = 0;
	(*rID)->_destroyLists.count = count;
	(*rID)->_destroyLists.listIDs = new Graphics::ListID[count];
	for (uint32 i = 0; i < count; i++)
		(*rID)->_destroyLists.listIDs[i] = listID + i;

	return rID;
}

RequestID RequestManager::destroyLists(Graphics::ListID *listIDs, uint32 count) {
	RequestID rID = newRequest(kITCEventDestroyLists);

	(*rID)->_destroyLists.lists   = 0;
	(*rID)->_destroyLists.count = count;
	(*rID)->_destroyLists.listIDs = new Graphics::ListID[count];
	memcpy((*rID)->_destroyLists.listIDs, listIDs, count * sizeof(Graphics::ListID));

	return rID;
}

RequestID RequestManager::newRequest(ITCEvent type) {
	Common::StackLock lock(_mutexUse);

	_requests.push_back(new Request(type));

	return --_requests.end();
}

void RequestManager::clearList() {
	Common::StackLock lock(_mutexUse);

	for (RequestList::iterator request = _requests.begin(); request != _requests.begin(); ++request)
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
