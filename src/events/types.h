/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file events/types.h
 *  Basic event types.
 */

#ifndef EVENTS_TYPES_H
#define EVENTS_TYPES_H

namespace Events {

typedef SDL_Event Event;

/** Custom event types. */
enum EventType {
	kEventNone      = SDL_NOEVENT,         ///< Nothing.
	kEventKeyDown   = SDL_KEYDOWN,         ///< Keyboard key was pressed.
	kEventKeyUp     = SDL_KEYUP,           ///< Keyboard key was released.
	kEventMouseMove = SDL_MOUSEMOTION,     ///< Mouse was moved.
	kEventMouseDown = SDL_MOUSEBUTTONDOWN, ///< Mouse button was pressed.
	kEventMouseUp   = SDL_MOUSEBUTTONUP,   ///< Mouse button was released.
	kEventQuit      = SDL_QUIT,            ///< Application quit was requested.
	kEventUserMin   = SDL_USEREVENT - 1,   ///< For range checks.
	kEventGraphics  = SDL_USEREVENT    ,   ///< Inter-thread communications regarding graphics.
	kEventSound                        ,   ///< Inter-thread communications regarding sound.
	kEventUserMax   = SDL_NUMEVENTS        ///< For range checks.
};

enum EventTypeGraphics {
	kEventGraphicsFullScreen = 0,
	kEventGraphicsWindowed
};

} // End of namespace Events

#endif // EVENTS_TYPES_H
