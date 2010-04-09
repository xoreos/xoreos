/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file events/events.cpp
 *  The global events manager.
 */

#include <SDL_timer.h>

#include "common/util.h"
#include "common/error.h"

#include "events/events.h"

#include "graphics/graphics.h"

#include "sound/sound.h"

DECLARE_SINGLETON(Events::EventsManager)

namespace Events {

EventsManager::EventsManager() {
	_ready = false;

	_quitRequested = false;
}

void EventsManager::init() {
	if (!GfxMan.ready())
		throw Common::Exception("The GraphicsManager needs to be initialized first");

	_ready = true;
}

void EventsManager::deinit() {
	_ready = false;
}

void EventsManager::reset() {
	if (!_ready)
		return;

	// Clear the events queue
	while (SDL_PollEvent(0));
}

bool EventsManager::ready() const {
	return _ready;
}

void EventsManager::delay(uint32 ms) {
	SDL_Delay(ms);
}

uint32 EventsManager::getTimestamp() const {
	return SDL_GetTicks();
}

bool EventsManager::pollEvent(Event &event) {
	// Filter quit events
	bool isQuit   = false;
	bool hasEvent = SDL_PollEvent(&event);

	if (hasEvent) {

		if (event.type == SDL_KEYDOWN) {
			if ((event.key.keysym.mod & (KMOD_CTRL | KMOD_META)) && (event.key.keysym.sym == SDLK_q))
				isQuit = true;
		} else if (event.type == SDL_QUIT)
				isQuit = true;

	}

	if (isQuit) {
		requestQuit();
		hasEvent = SDL_PollEvent(&event);
	}

	return hasEvent;
}

bool EventsManager::pushEvent(Event &event) {
	return SDL_PushEvent(&event) != 0;
}

bool EventsManager::quitRequested() const {
	return _quitRequested;
}

void EventsManager::requestQuit() {
	_quitRequested = true;
}

void EventsManager::initMainLoop() {
	try {

		GfxMan.initSize(800, 600, false);
		GfxMan.setupScene();

	} catch (Common::Exception &e) {
		e.add("Failed setting up graphics");
		throw e;
	}

	status("Graphics set up");

	// Set the window title to our name
	GfxMan.setWindowTitle(PACKAGE_STRING);
}

void EventsManager::runMainLoop() {
	Event event;

	while (!quitRequested()) {
		while (pollEvent(event));

		GfxMan.renderScene();
	}
}

} // End of namespace Events
