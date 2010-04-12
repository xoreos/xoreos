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

#include "events/requests.h"
#include "events/events.h"

#include "graphics/images/decoder.h"

DECLARE_SINGLETON(Events::RequestManager);

namespace Events {

void RequestManager::init() {
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

	// We can now destroy the request
	delete *request;

	// And remove it from the list
	_requests.erase(request);

	// And finally give the use mutex free again
	_mutexUse.unlock();
}

void RequestManager::dispatchAndWait(RequestID request) {
	dispatch(request);
	waitReply(request);
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
	_requests.push_back(new Request(type));

	RequestID rID = --_requests.end();

	(*rID)->create();

	return rID;
}

} // End of namespace Events
