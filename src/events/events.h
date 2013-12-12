/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file events/events.h
 *  The global events manager.
 */

#ifndef EVENTS_EVENTS_H
#define EVENTS_EVENTS_H

#include <list>
#include <vector>

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"

#include "events/types.h"

namespace Common {
	class UString;
}

namespace Events {

class Request;
class Joystick;

/** The events manager. */
class EventsManager : public Common::Singleton<EventsManager> {
public:
	EventsManager();

	// Initialization

	/** Initialize the events subsystem. */
	void init();
	/** Deinitialize the events subsystem. */
	void deinit();

	/** Completely reset the events manager. */
	void reset();

	/** Was the events subsystem successfully initialized? */
	bool ready() const;


	// Quitting

	/** Was an engine quit requested? */
	bool quitRequested() const;
	/** Request an engine quit. */
	void requestQuit();

	/** Initiate the actual quitting process. */
	void doQuit();


	// Timing

	/** Sleep that number of milliseconds. */
	void delay(uint32 ms);
	/** Return the number of milliseconds the application is running. */
	uint32 getTimestamp() const;


	// Events

	/** Clear the event queue, ignore all unhandled events. */
	void flushEvents();

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


	// Keyboard input

	/** Enable/Disable repeated key events. */
	void enableKeyRepeat(bool repeat = true);

	/** Return the unicode code point of the pressed key.
	 *
	 *  Requirements: enableUnicode must be enabled and the event must be a keydown event.
	 */
	const char *getPressedCharacter(const Event &event);


	// Joystick input

	/** Return the number of avaiable joysticks. */
	int getJoystickCount() const;

	/** Return the joystick with that index. */
	Joystick *getJoystickByIndex(int index) const;
	/** Return the first joystick with that name. */
	Joystick *getJoystickByName(const Common::UString &name) const;


	/** Is the event queue full? */
	bool isQueueFull() const;

	/** Run the main loop. */
	void runMainLoop();


private:
	typedef std::vector<Joystick *> Joysticks;

	typedef std::list<Event> EventQueue;
	typedef void (EventsManager::*RequestHandler)(Request &);

	/** Pointer to the request handler. */
	static const RequestHandler _requestHandler[kITCEventMAX];

	bool _ready; ///< Was the events subsystem successfully initialized?

	bool _quitRequested; ///< Was an engine quit requested?
	bool _doQuit;        ///< Are we currently in the process of quitting?

	Joysticks _joysticks;

	EventQueue _eventQueue;
	Common::Mutex _eventQueueMutex;

	int _queueSize;

	bool _fullQueue;
	Common::Condition _queueProcessed;

	bool _repeat;


	/** Initialize the available joysticks/gamepads. */
	void initJoysticks();
	/** Deinitialize the available joysticks/gamepads. */
	void deinitJoysticks();

	/** Look for quit events. */
	bool parseEventQuit(const Event &event);
	/** Look for graphics events. */
	bool parseEventGraphics(const Event &event);
	/** Look for inter-thread communication. */
	bool parseITC(const Event &event);

	// Request handler
	void requestCallInMainThread(Request &request);
	void requestRebuildGLContainer(Request &request);
	void requestDestroyGLContainer(Request &request);

	void processEvents();

	friend class RequestManager;
};

} // End of namespace Events

/** Shortcut for accessing the events manager. */
#define EventMan Events::EventsManager::instance()

#endif // EVENTS_EVENTS_H
