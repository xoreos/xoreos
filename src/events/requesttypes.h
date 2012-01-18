/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file events/requesttypes.h
 *  Inter-thread request event types.
 */

#ifndef EVENTS_REQUESTTYPES_H
#define EVENTS_REQUESTTYPES_H

#include "common/types.h"
#include "common/mutex.h"

#include "events/types.h"

#include "graphics/types.h"

namespace Graphics {
	class GLContainer;
}

namespace Events {

// Data structures for specific requests

struct RequestDataResize {
	int width;
	int height;
};

struct RequestDataChangeFSAA {
	int level;
};

struct RequestDataChangeVSync {
	bool vsync;
};

struct RequestDataChangeGamma {
	float gamma;
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
		RequestDataResize      _resize;
		RequestDataChangeFSAA  _fsaa;
		RequestDataChangeVSync _vsync;
		RequestDataChangeGamma _gamma;
		RequestDataGLContainer _glContainer;
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
