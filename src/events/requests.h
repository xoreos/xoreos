/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file events/requests.h
 *  Inter-thread request events.
 */

#include "common/mutex.h"

#include "events/types.h"

#ifndef EVENTS_REQUESTS_H
#define EVENTS_REQUESTS_H

namespace Events {

/** Base class for request events. */
class Request {
// To be used by the game thread
public:
	Request();

	/** Dispatch the request. */
	void dispatch();

	/** Wait until the request has been answered. */
	void waitReply();

protected:
	/** Create the event for this request. */
	void createEvent(ITCEvent itcType);

private:
	Common::Mutex _mutexUse;   ///< The mutex preventing race condition in the use.
	Common::Mutex _mutexReply; ///< The mutex signaling a reply.

	Event _event; ///< The actual event.

	volatile bool _dispatched; ///< Was the event dispatched?


// To be used by the event thread
public:
	/** Signal that the request has been answered. */
	void signalReply();
};

/** Requesting fullscreen/windowed mode. */
class RequestFullscreen : public Request {
public:
	RequestFullscreen(bool fullscreen);
};

/** Requesting display size change. */
class RequestResize: public Request {
public:
	RequestResize(int width, int height);

private:
	int _width;
	int _height;


// To be used by the event thread
public:
	int getWidth()  const;
	int getHeight() const;
};

} // End of namespace Events

#endif // EVENTS_REQUESTS_H
