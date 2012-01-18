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

/** @file events/requests.h
 *  Inter-thread request events.
 */

#ifndef EVENTS_REQUESTS_H
#define EVENTS_REQUESTS_H

#include <list>

#include "common/types.h"
#include "common/singleton.h"
#include "common/thread.h"

#include "graphics/types.h"

#include "events/requesttypes.h"

namespace Graphics {
	class Texture;
	class ListContainer;
}

namespace Events {

typedef std::list<Request *>  RequestList;
typedef RequestList::iterator RequestID;

/** The request manager, handling all requests.
 *
 *  Requests are the main means of communication between the game thread and
 *  the main thread, which handles all low-level event and graphics operations.
 *
 *  The idea behind requests is that, as soon as the game thread knows exactly
 *  what it needs, it creates and dispatches a specific request. Then it does
 *  other things, until it needs what it requested for, issuing a waitReply on
 *  the request. That way, the actual fulfilling of the request can happen
 *  asynchronously, without it unnecessarily blocking further execution of the
 *  game thread.
 *
 *  @note As soon as waitReply(), forget(), dispatchAndWait() or
 *         dispatchAndForget() was called, the RequestID expires.
 */
class RequestManager : public Common::Singleton<RequestManager>, public Common::Thread {
public:
	~RequestManager();

	void init();
	void deinit();

	/** Dispatch a request. */
	void dispatch(RequestID request);
	/** Wait for a request to be answered. */
	void waitReply(RequestID request);
	/** Ignore any answer we get. */
	void forget(RequestID request);

	/** Dispatch a request and wait for the answer. */
	void dispatchAndWait(RequestID request);
	/** Dispatch a request and ignore the answer. */
	void dispatchAndForget(RequestID request);

	/** Request a sync, letting all prior requests finish. */
	void sync();

	// Screen mode
	/** Request that the display shall be switched to fullscreen or windowed mode. */
	RequestID fullscreen(bool fs);
	/** Request that the display shall be resized. */
	RequestID resize(int width, int height);
	/** Request that the FSAA level shall be changed. */
	RequestID changeFSAA(int level);
	/** Request that the vsync settings shall be changed. */
	RequestID changeVSync(bool vsync);
	/** Request that the gamma settings shall be changed. */
	RequestID changeGamma(float gamma);

	/** Request that a GL container shall be rebuilt. */
	RequestID rebuild(Graphics::GLContainer &glContainer);
	/** Request that a GL container shall be destroyed. */
	RequestID destroy(Graphics::GLContainer &glContainer);

	// Singleton
	static void destroy();

private:
	Common::Mutex _mutexUse; ///< The mutex locking the use of the manager.

	RequestList _requests; ///< All currently active requests.

	/** Create a new, empty request of that type. */
	RequestID newRequest(ITCEvent type);

	void clearList();

	void collectGarbage();

	void threadMethod();
};

} // End of namespace Events

/** Shortcut for accessing the request manager. */
#define RequestMan Events::RequestManager::instance()

#endif // EVENTS_REQUESTS_H
