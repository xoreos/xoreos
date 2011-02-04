/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file events/requesttypes.cpp
 *  Inter-thread request event types.
 */

#include "common/error.h"
#include "common/util.h"

#include "events/requesttypes.h"
#include "events/events.h"

#include "graphics/images/decoder.h"

namespace Events {

Request::Request(ITCEvent type) : _type(type), _dispatched(false), _garbage(false) {
	_hasReply = new Common::Condition(_mutexReply);

	create();
}

Request::~Request() {
	if (_event.type == kITCEventDestroyLists)
		delete[] _destroyLists.listIDs;

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
	_event.type       = kEventITC;
	_event.user.code  = (int) _type;
	_event.user.data1 = (void *) this;

	if (_event.type == kITCEventDestroyLists)
		_destroyLists.listIDs = 0;
}

void Request::signalReply() {
	_dispatched = false;
	_hasReply->signal();
}

void Request::copyToReply() {
}

} // End of namespace Events
