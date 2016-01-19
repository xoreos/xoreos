/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Inter-thread request event types.
 */

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/events/requesttypes.h"
#include "src/events/events.h"

namespace Events {

Request::Request(ITCEvent type) : _type(type), _dispatched(false), _garbage(false),
	_hasReply(0) {

	create();
}

Request::~Request() {
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
	_event.user.data1 = static_cast<void *>(this);
}

void Request::signalReply() {
	_hasReply.unlock();
}

void Request::copyToReply() {
}

} // End of namespace Events
