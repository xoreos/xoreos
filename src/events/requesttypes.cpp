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

#include "events/requesttypes.h"
#include "events/events.h"

#include "graphics/images/decoder.h"

namespace Events {

Request::Request(ITCEvent type) : _type(type), _dispatched(false), _garbage(false) {
	_hasReply = new Common::Condition(_mutexReply);
}

Request::~Request() {
	switch (_type) {
		case kITCEventCreateTextures:
			delete[] _createTextures.ids;
			break;

		case kITCEventDestroyTextures:
			delete[] _destroyTextures.ids;
			break;

		default:
			break;
	}

	delete _hasReply;
}

bool Request::isGarbage() const {
	// Only "really" garbage if it hasn't got a pending answer
	return _garbage && !_dispatched;
}

void Request::setGarbage() {
	_garbage = true;
}

void Request::create() {
	switch (_type) {
		case kITCEventCreateTextures:
			_createTextures.ids = 0;
			break;

		case kITCEventDestroyTextures:
			_destroyTextures.ids = 0;
			break;

		default:
			break;
	}

	_event.type       = kEventITC;
	_event.user.code  = (int) _type;
	_event.user.data1 = (void *) this;
}

void Request::signalReply() {
	_dispatched = false;
	_hasReply->signal();
}

void Request::copyToReply() {
	switch (_type) {
		case kITCEventCreateTextures:
			memcpy(_createTextures.replyIDs, _createTextures.ids, _createTextures.count * sizeof(Graphics::TextureID));
			break;

		case kITCEventIsTexture:
			*_isTexture.replyAnswer = _isTexture.answer;
			break;

		default:
			break;
	}
}

} // End of namespace Events
