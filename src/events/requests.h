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

#ifndef EVENTS_REQUESTS_H
#define EVENTS_REQUESTS_H

#include <list>

#include "common/types.h"
#include "common/singleton.h"
#include "common/thread.h"

#include "graphics/types.h"

#include "events/requesttypes.h"

namespace Graphics {
	class ImageDecoder;
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
 *  @note: As soon as waitReply(), forget(), dispatchAndWait() or
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

	// Screen mode
	/** Request that the display shall be switched to fullscreen or windowed mode. */
	RequestID fullscreen(bool fullscreen);
	/** Request that the display shall be resized. */
	RequestID resize(int width, int height);

	// Textures
	/** Request the creation of textures. */
	RequestID createTextures(uint32 n, Graphics::TextureID *ids);
	/** Request the destruction of textures. */
	RequestID destroyTextures(uint32 n, Graphics::TextureID *ids);
	/** Request the loading of texture image data. */
	RequestID loadTexture(Graphics::TextureID id, const Graphics::ImageDecoder *image);
	/** Ask if a texture ID is a real, working texture. */
	RequestID isTexture(Graphics::TextureID id, bool *answer);

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
