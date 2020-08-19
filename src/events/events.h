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
 *  The global events manager.
 */

#ifndef EVENTS_EVENTS_H
#define EVENTS_EVENTS_H

#include <list>
#include <vector>

#include "src/common/types.h"
#include "src/common/ptrvector.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"

#include "src/events/types.h"
#include "src/events/joystick.h"

namespace Common {
	class UString;
}

namespace Events {

class Request;

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

	// For the game thread to signal the main thread that the engine code threw

	/** Was a fatal engine error raised? */
	bool fatalErrorRaised() const;
	/** Raise a fatal engine error. */
	void raiseFatalError();


	// Timing

	/** Sleep that number of milliseconds. */
	void delay(uint32_t ms);
	/** Return the number of milliseconds the application is running. */
	uint32_t getTimestamp() const;


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

	/** Enable/Disable the text input.
	 *
	 *  While text input is enabled, we will receive TextInput event containing
	 *  UTF-8 encoded strings containing text the user has entered.
	 *  See getTextInput().
	 *
	 *  Enabling text input will also activate the screen keyboard on some systems.
	 */
	void enableTextInput(bool textInput = true);

	/** Return the text that was input with keyboard, in UTF-8 encoding.
	 *
	 *  The event must be a TextInput or KeyDown event.
	 *  For better i18n support, TextInput is preferred.
	 */
	Common::UString getTextInput(const Event &event);


	// Joystick input

	/** Return the number of available joysticks. */
	size_t getJoystickCount() const;

	/** Return the joystick with that index. */
	Joystick *getJoystickByIndex(size_t index) const;
	/** Return the first joystick with that name. */
	Joystick *getJoystickByName(const Common::UString &name) const;


	/** Is the event queue full? */
	bool isQueueFull() const;

	/** Run the main loop. */
	void runMainLoop();


private:
	typedef Common::PtrVector<Joystick> Joysticks;

	typedef std::list<Event> EventQueue;
	typedef void (EventsManager::*RequestHandler)(Request &);

	/** Pointer to the request handler. */
	static const RequestHandler _requestHandler[kITCEventMAX];

	bool _ready; ///< Was the events subsystem successfully initialized?

	bool _quitRequested; ///< Was an engine quit requested?
	bool _doQuit;        ///< Are we currently in the process of quitting?

	bool _fatalError;

	Joysticks _joysticks;

	EventQueue _eventQueue;
	std::recursive_mutex _eventQueueMutex;

	size_t _queueSize;

	bool _fullQueue;
	std::condition_variable_any _queueProcessed;
	std::recursive_mutex _queueProcessedMutex;

	bool _repeat;
	uint _repeatCounter;

	uint _textInputCounter;


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
