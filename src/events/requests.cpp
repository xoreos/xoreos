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

namespace Events {

Request::Request() {
	_event.type = kEventNone;

	_dispatched = false;
}

void Request::dispatch() {
	Common::StackLock lock(_mutexUse);

	if (_dispatched)
		// We are already waiting for an answer
		return;

	// Lock the mutex, so that it can be waited upon
	_mutexReply.lock();

	// And send the event
	if (!EventMan.pushEvent(_event))
		throw Common::Exception("Failed dispatching request");

	// Set state
	_dispatched = true;
}

void Request::waitReply() {
	Common::StackLock lock(_mutexUse);

	if (!_dispatched)
		// The request either wasn't yet dispatched, or we've already gotten a reply
		return;

	// Lock the mutex, to wait for its release
	_mutexReply.lock();

	// Could lock the mutex, so it was released => We had a reply

	// No unlock the mutex again, as we're not actually using it
	_mutexReply.unlock();
}

void Request::signalReply() {
	Common::StackLock lock(_mutexUse);

	// Unlock the mutex, signaling a reply
	_mutexReply.unlock();

	// Set state
	_dispatched = false;
}

void Request::createEvent(ITCEvent itcType) {
	_event.type       = kEventITC;
	_event.user.code  = (int) itcType;
	_event.user.data1 = (void *) this;
}


RequestFullscreen::RequestFullscreen(bool fullscreen) {
	if (fullscreen)
		createEvent(kITCEventFullscreen);
	else
		createEvent(kITCEventWindowed);
}


RequestResize::RequestResize(int width, int height) : _width(width), _height(height) {
	createEvent(kITCEventResize);
}

int RequestResize::getWidth() const {
	return _width;
}

int RequestResize::getHeight() const {
	return _height;
}


RequestCreateTextures::RequestCreateTextures(uint32 n) : _count(n) {
	_ids = new uint32[_count];

	createEvent(kITCEventCreateTextures);
}

RequestCreateTextures::~RequestCreateTextures() {
	delete[] _ids;
}

const uint32 *RequestCreateTextures::getIDs() const {
	return _ids;
}

uint32 RequestCreateTextures::getCount() const {
	return _count;
}

uint32 *RequestCreateTextures::getIDs() {
	return _ids;
}


RequestDestroyTextures::RequestDestroyTextures(uint32 n, uint32 *ids) : _count(n) {
	_ids = new uint32[_count];

	memcpy(_ids, ids, _count * sizeof(uint32));

	createEvent(kITCEventDestroyTextures);
}

RequestDestroyTextures::~RequestDestroyTextures() {
	delete[] _ids;
}

uint32 RequestDestroyTextures::getCount() const {
	return _count;
}

const uint32 *RequestDestroyTextures::getIDs() const {
	return _ids;
}


RequestLoadTextures::RequestLoadTextures(uint32 id, const byte *data,
		int width, int height, bool hasAlpha) :
		_id(id), _data(data), _width(width), _height(height), _hasAlpha(hasAlpha) {

	createEvent(kITCEventLoadTextures);
}

RequestLoadTextures::RequestLoadTextures(uint32 id, const Graphics::ImageDecoder *image) : _id(id) {
	_width    = image->getWidth();
	_height   = image->getHeight();
	_data     = image->getData();
	_hasAlpha = image->hasAlpha();

	createEvent(kITCEventLoadTextures);
}

uint32 RequestLoadTextures::getID() const {
	return _id;
}

const byte *RequestLoadTextures::getData() const {
	return _data;
}

int RequestLoadTextures::getWidth() const {
	return _width;
}

int RequestLoadTextures::getHeight() const {
	return _height;
}

bool RequestLoadTextures::hasAlpha() const {
	return _hasAlpha;
}

} // End of namespace Events
