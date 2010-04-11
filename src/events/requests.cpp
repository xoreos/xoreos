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

namespace Events {

Request::Request() {
	_event.type = kEventNone;

	_dispatched = false;

	_hasReply = new Common::Condition(_mutexReply);
}

Request::~Request() {
	delete _hasReply;
}

void Request::dispatch() {
	Common::StackLock lock(_mutexUse);

	if (_dispatched)
		// We are already waiting for an answer
		return;

	// Set state
	_dispatched = true;

	// And send the event
	if (!EventMan.pushEvent(_event))
		throw Common::Exception("Failed dispatching request");

	if (EventMan.isMainThread())
		// If we're currently in the main thread, to avoid a dead-lock, process events now
		EventMan.processEvents();
}

void Request::waitReply() {
	// Locking our use mutex, to prevent race conditions
	_mutexUse.lock();

	if (!_dispatched) {
		// The request either wasn't yet dispatched, or we've already gotten a reply
		_mutexUse.unlock();
		return;
	}

	// Lock the mutex for the condition
	_mutexReply.lock();

	// We don't need our use mutex now
	_mutexUse.unlock();

	// Wait for a reply
	_hasReply->wait();

	// Got a reply

	// Now we need the use mutex again
	_mutexUse.lock();

	// Unlock the relocked reply mutex
	_mutexReply.unlock();

	// And finally give the use mutex free again
	_mutexUse.unlock();
}

void Request::dispatchAndWait() {
	dispatch();
	waitReply();
}

void Request::signalReply() {
	Common::StackLock lock(_mutexUse);

	// Set state
	_dispatched = false;

	// Signaling a reply
	_hasReply->signal();
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


RequestCreateTextures::RequestCreateTextures(uint32 n, Graphics::TextureID *ids) :
	_count(n), _ids(ids) {

	createEvent(kITCEventCreateTextures);
}

uint32 RequestCreateTextures::getCount() const {
	return _count;
}

Graphics::TextureID *RequestCreateTextures::getIDs() {
	return _ids;
}


RequestDestroyTextures::RequestDestroyTextures(uint32 n, const Graphics::TextureID *ids) :
	_count(n) , _ids(ids) {

	createEvent(kITCEventDestroyTextures);
}

uint32 RequestDestroyTextures::getCount() const {
	return _count;
}

const Graphics::TextureID *RequestDestroyTextures::getIDs() const {
	return _ids;
}


RequestLoadTexture::RequestLoadTexture(Graphics::TextureID id, const byte *data,
		int width, int height, Graphics::PixelFormat format) :
		_id(id), _data(data), _width(width), _height(height), _format(format) {

	if ((_width <= 0) || (_height <= 0) || !_data)
		throw Common::Exception("Invalid image data (%dx%d %d)", _width, _height, _data != 0);

	createEvent(kITCEventLoadTextures);
}

RequestLoadTexture::RequestLoadTexture(Graphics::TextureID id,
		const Graphics::ImageDecoder *image) : _id(id) {

	if (!image)
		throw Common::Exception("image == 0");

	_width  = image->getWidth();
	_height = image->getHeight();
	_data   = image->getData();
	_format = image->getFormat();

	if ((_width <= 0) || (_height <= 0) || !_data)
		throw Common::Exception("Invalid image data (%dx%d %d)", _width, _height, _data != 0);

	createEvent(kITCEventLoadTextures);
}

Graphics::TextureID RequestLoadTexture::getID() const {
	return _id;
}

const byte *RequestLoadTexture::getData() const {
	return _data;
}

int RequestLoadTexture::getWidth() const {
	return _width;
}

int RequestLoadTexture::getHeight() const {
	return _height;
}

Graphics::PixelFormat RequestLoadTexture::getFormat() const {
	return _format;
}

} // End of namespace Events
