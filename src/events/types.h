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
	kEvenTypeMin       = SDL_USEREVENT - 1, ///< For range checks.
	kEventTypeGraphics = SDL_USEREVENT    , ///< Inter-thread communications regarding graphics.
	kEventTypeSound                       , ///< Inter-thread communications regarding sound.
	kEventTypeMax      = SDL_NUMEVENTS      ///< For range checks.
};

enum EventTypeGraphics {
	kEventTypeGraphicsFullScreen = 0,
	kEventTypeGraphicsWindowed
};

} // End of namespace Events

#endif // EVENTS_TYPES_H
