/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
