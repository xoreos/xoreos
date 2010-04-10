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

	Common::StackLock lock(_eventQueueMutex);

	// Clear the SDL event queue
	while (SDL_PollEvent(0));

	// Clear our event queue
	_eventQueue.clear();
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

bool EventsManager::parseEventQuit(const Event &event) {
	if ((event.type == kEventQuit) ||
			((event.type == kEventKeyDown) &&
			 (event.key.keysym.mod & (KMOD_CTRL | KMOD_META)) &&
			 (event.key.keysym.sym == SDLK_q))) {

		requestQuit();
		return true;
	}

	return false;
}

bool EventsManager::parseEventGraphics(const Event &event) {
	if (event.type == kEventKeyDown && (event.key.keysym.mod & KMOD_ALT)
			&& event.key.keysym.sym == SDLK_RETURN) {

		GfxMan.toggleFullScreen();
		return true;
	}

	if (event.type == kEventGraphics) {
		if      (event.user.code == kEventGraphicsFullScreen)
			GfxMan.setFullScreen(true);
		else if (event.user.code == kEventGraphicsWindowed)
			GfxMan.setFullScreen(false);

		return true;
	}

	if (event.type == kEventResize) {
		GfxMan.changeSize(event.resize.w, event.resize.h);
		return true;
	}

	return false;
}

void EventsManager::processEvents() {
	Common::StackLock lock(_eventQueueMutex);

	Event event;
	while (SDL_PollEvent(&event)) {

		// Check for quit events
		if (parseEventQuit(event))
			continue;

		// Check for graphics events
		if (parseEventGraphics(event))
			continue;

		// Push the event to the back of the list
		_eventQueue.push_back(event);
	}
}

bool EventsManager::pollEvent(Event &event) {
	Common::StackLock lock(_eventQueueMutex);

	if (_eventQueue.empty())
		return false;

	// Return an event from the front of the list
	event = _eventQueue.front();
	_eventQueue.pop_front();

	return true;
}

bool EventsManager::pushEvent(Event &event) {
	Common::StackLock lock(_eventQueueMutex);

	return SDL_PushEvent(&event) != 0;
}

void EventsManager::enableUnicode(bool enable) {
	Common::StackLock lock(_eventQueueMutex);

	SDL_EnableUNICODE(enable ? 1 : 0);
}

char EventsManager::getPressedCharacter(const Event &event) {
	if ((event.type != kEventKeyDown) && (event.type != kEventKeyUp))
		return 0;

	return event.key.keysym.unicode & 0x7F;
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
	uint32 lastFPS = getTimestamp();

	while (!quitRequested()) {
		// (Pre)Process all events
		processEvents();

		// Render a frame
		GfxMan.renderScene();

		// Display a FPS counter every second
		uint32 now = getTimestamp();
		if ((now - lastFPS) >= 1000) {
			status("%d fps", GfxMan.getFPS());

			lastFPS = now;
		}
	}
}

} // End of namespace Events
