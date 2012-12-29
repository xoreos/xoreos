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

/** @file events/types.h
 *  Basic event types.
 */

#ifndef EVENTS_TYPES_H
#define EVENTS_TYPES_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <SDL_events.h>

namespace Events {

typedef SDL_Event Event;

/** Custom event types. */
enum EventType {
	kEventNone      = SDL_NOEVENT        , ///< Nothing.
	kEventKeyDown   = SDL_KEYDOWN        , ///< Keyboard key was pressed.
	kEventKeyUp     = SDL_KEYUP          , ///< Keyboard key was released.
	kEventMouseMove = SDL_MOUSEMOTION    , ///< Mouse was moved.
	kEventMouseDown = SDL_MOUSEBUTTONDOWN, ///< Mouse button was pressed.
	kEventMouseUp   = SDL_MOUSEBUTTONUP  , ///< Mouse button was released.
	kEventQuit      = SDL_QUIT           , ///< Application quit was requested.
	kEventResize    = SDL_VIDEORESIZE    , ///< Resize the window.
	kEventUserMIN   = SDL_USEREVENT - 1  , ///< For range checks.
	kEventITC       = SDL_USEREVENT      , ///< Inter-thread communication
	kEventUserMAX   = SDL_NUMEVENTS        ///< For range checks.
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

public:
	MainThreadFunctor(const boost::function<T ()> &f) : func(f), retVal(new T) { }
	void operator()() const { *retVal = func(); }

	T getReturnValue() const { return *retVal; }
};

/** Template specialization for a MainThreadFunctor returning void. */
template<> struct MainThreadFunctor<void> {
private:
	boost::function<void ()> func;

public:
	MainThreadFunctor(const boost::function<void ()> &f) : func(f) { }
	void operator()() const { func(); }

	void getReturnValue() const { (void) 0; }
};

typedef boost::function<void ()> MainThreadCallerFunctor;

} // End of namespace Events

#endif // EVENTS_TYPES_H
