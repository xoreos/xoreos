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
 *  Basic event types.
 */

#ifndef EVENTS_TYPES_H
#define EVENTS_TYPES_H

#include <stdexcept>

#include "src/common/error.h"

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <SDL_events.h>

namespace Events {

typedef SDL_Event Event;

/** Custom event types. */
enum EventType {
	kEventKeyDown   = SDL_KEYDOWN        , ///< Keyboard key was pressed.
	kEventKeyUp     = SDL_KEYUP          , ///< Keyboard key was released.
	kEventMouseMove = SDL_MOUSEMOTION    , ///< Mouse was moved.
	kEventMouseDown = SDL_MOUSEBUTTONDOWN, ///< Mouse button was pressed.
	kEventMouseUp   = SDL_MOUSEBUTTONUP  , ///< Mouse button was released.
	kEventMouseWheel= SDL_MOUSEWHEEL     , ///< Mouse wheel was used.
	kEventTextInput = SDL_TEXTINPUT      , ///< Text was written.
	kEventQuit      = SDL_QUIT           , ///< Application quit was requested.
	kEventWindow    = SDL_WINDOWEVENT    , ///< Resize the window.
	kEventUserMIN   = SDL_USEREVENT - 1  , ///< For range checks.
	kEventITC       = SDL_USEREVENT      , ///< Inter-thread communication.
	kEventUserMAX   = SDL_LASTEVENT        ///< For range checks.
};

/** Sub events for kEventWindow. */
enum EventWindowType {
	kEventWindowResized     = SDL_WINDOWEVENT_RESIZED     , ///< Window has been resized.
	kEventWindowSizeChanged = SDL_WINDOWEVENT_SIZE_CHANGED, ///< Window's size was otherwise changed.
	kEventWindowMinimized   = SDL_WINDOWEVENT_MINIMIZED   , ///< Window was minimized.
	kEventWindowMaximized   = SDL_WINDOWEVENT_MAXIMIZED   , ///< Window was maximized.
	kEventWindowRestored    = SDL_WINDOWEVENT_RESTORED      ///< Window was restored.
};

/** Specific type of the inter-thread communication. */
enum ITCEvent {
	kITCEventSync               = 0, ///< Request a sync, letting all prior requests finish.
	kITCEventCallInMainThread      , ///< Request to call a function in the main thread.
	kITCEventRebuildGLContainer    , ///< Request the rebuilding of a GL container.
	kITCEventDestroyGLContainer    , ///< Request the destruction of a GL container.
	kITCEventMAX                     ///< For range checks.
};

/** A functor for a function that needs to be called in the main thread. */
template<typename T> struct MainThreadFunctor {
private:
	boost::function<T ()> func;
	boost::shared_ptr<T> retVal;
	boost::shared_ptr<Common::Exception> error;

public:
	MainThreadFunctor(const boost::function<T ()> &f) : func(f), retVal(new T), error(new Common::Exception) { }
	void operator()() const {
		try {
			*retVal = func();
		} catch (Common::Exception &e) {
			*error = e;
		} catch (std::exception &e) {
			*error = Common::Exception(e);
		} catch (...) {
			*error = Common::Exception("Unknown exception thrown in MainThreadFunctor");
		}
	}

	T getReturnValue() const { return *retVal; }
	const Common::Exception &getError() const { return *error; }
};

/** Template specialization for a MainThreadFunctor returning void. */
template<> struct MainThreadFunctor<void> {
private:
	boost::function<void ()> func;
	boost::shared_ptr<Common::Exception> error;

public:
	MainThreadFunctor(const boost::function<void ()> &f) : func(f), error(new Common::Exception) { }
	void operator()() const {
		try {
			func();
		} catch (Common::Exception &e) {
			*error = e;
		} catch (std::exception &e) {
			*error = Common::Exception(e);
		} catch (...) {
			*error = Common::Exception("Unknown exception thrown in MainThreadFunctor");
		}
	}

	void getReturnValue() const { (void) 0; }
	const Common::Exception &getError() const { return *error; }
};

typedef boost::function<void ()> MainThreadCallerFunctor;

} // End of namespace Events

#endif // EVENTS_TYPES_H
