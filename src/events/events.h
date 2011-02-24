/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file events/events.h
 *  The global events manager.
 */

#ifndef EVENTS_EVENTS_H
#define EVENTS_EVENTS_H

#include <string>
#include <list>

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"

#include "events/types.h"

namespace Events {

class Request;

/** The events manager. */
class EventsManager : public Common::Singleton<EventsManager> {
public:
	EventsManager();

	/** Initialize the events subsystem. */
	void init();
	/** Deinitialize the events subsystem. */
	void deinit();

	/** Completely reset the events manager. */
	void reset();

	/** Was the events subsystem successfully initialized? */
	bool ready() const;

	/** Sleep that number of milliseconds. */
	void delay(uint32 ms);
	/** Return the number of milliseconds the application is running. */
	uint32 getTimestamp() const;

	/** Was an engine quit requested? */
	bool quitRequested() const;
	/** Request an engine quit. */
	void requestQuit();

	/** Initiate the actual quitting process. */
	void doQuit();

	/** Get an event from the events queue.
	 *
	 *  @param  event Where to store the polled event.
	 *  @return true if there was an event to poll, false if not.
	 */
	bool pollEvent(Event &event);

	/** Push an event onto the events queue.
	 *
	 *  @param  event The event to push.
	 *  @return true on success, false otherwise.
	 */
	bool pushEvent(Event &event);

	/** Enable/Disable translating unicode translation for key events. */
	void enableUnicode(bool enable);

	/** Enable/Disable repeated key events.
	 *
	 *  @param delayTime Delay between the start of repeated events in ms.
	 *         0 disables repeating.
	 *  @param interval Interval between repeated events in ms.
	 */
	void enableKeyRepeat(int delayTime = SDL_DEFAULT_REPEAT_DELAY, int interval = SDL_DEFAULT_REPEAT_INTERVAL);

	/** Return the ASCII character of the pressed key.
	 *
	 *  Requirements: enableUnicode must be enabled and the event must
	 *                be a keydown or keyup event.
	 */
	char getPressedCharacter(const Event &event);

	/** Is the event queue full? */
	bool isQueueFull() const;

	/** Run the main loop. */
	void runMainLoop();

private:
	typedef std::list<Event> EventQueue;
	typedef void (EventsManager::*RequestHandler)(Request &);

	/** Pointer to the request handler. */
	static const RequestHandler _requestHandler[kITCEventMAX];

	bool _ready; ///< Was the events subsystem successfully initialized?

	bool _quitRequested; ///< Was an engine quit requested?
	bool _doQuit;        ///< Are we currently in the process of quitting?

	EventQueue _eventQueue;
	Common::Mutex _eventQueueMutex;

	int _queueSize;

	bool _fullQueue;
	Common::Condition _queueProcessed;

	/** Look for quit events. */
	bool parseEventQuit(const Event &event);
	/** Look for graphics events. */
	bool parseEventGraphics(const Event &event);
	/** Look for inter-thread communication. */
	bool parseITC(const Event &event);

	// Request handler
	void requestFullscreen(Request &request);
	void requestWindowed(Request &request);
	void requestResize(Request &request);
	void requestChangeFSAA(Request &request);
	void requestChangeVSync(Request &request);
	void requestRebuildGLContainer(Request &request);
	void requestDestroyGLContainer(Request &request);

	void processEvents();

	friend class RequestManager;
};

} // End of namespace Events

/** Shortcut for accessing the events manager. */
#define EventMan Events::EventsManager::instance()

#endif // EVENTS_EVENTS_H
