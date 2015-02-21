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

#ifndef EVENTS_REQUESTTYPES_H
#define EVENTS_REQUESTTYPES_H

#include "src/common/types.h"
#include "src/common/mutex.h"

#include "src/events/types.h"

#include "src/graphics/types.h"

namespace Graphics {
	class GLContainer;
}

namespace Events {

// Data structures for specific requests

struct RequestCallInMainThread {
	const MainThreadCallerFunctor *caller;
};

struct RequestDataGLContainer {
	Graphics::GLContainer *glContainer;
};

/** A request, carrying inter-thread communication. */
class Request {
public:
	Request(ITCEvent type);
	~Request();

	bool isGarbage() const;

private:
	ITCEvent _type;

	bool _dispatched; ///< Was the request dispatched?
	bool _garbage;

	Common::Semaphore _hasReply; ///< Do we have a reply?

	Event _event; ///< The actual event.

	/** Request data. */
	union {
		RequestCallInMainThread _callInMainThread;
		RequestDataGLContainer  _glContainer;
	};

	/** Create the empty request frame. */
	void create();

	/** Copy reply data to the reply address. */
	void copyToReply();

	/** Signal that the request was answered. */
	void signalReply();

	void setGarbage();

	friend class EventsManager;
	friend class RequestManager;
};

} // End of namespace Events

#endif // EVENTS_REQUESTTYPES_H
