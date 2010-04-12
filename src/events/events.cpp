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
#include "events/requests.h"

#include "graphics/types.h"
#include "graphics/graphics.h"

#include "sound/sound.h"

DECLARE_SINGLETON(Events::EventsManager)

namespace Events {

const EventsManager::RequestHandler EventsManager::_requestHandler[kITCEventMAX] = {
	&EventsManager::requestFullscreen,
	&EventsManager::requestWindowed,
	&EventsManager::requestResize,
	&EventsManager::requestCreateTextures,
	&EventsManager::requestDestroyTextures,
	&EventsManager::requestLoadTexture,
	&EventsManager::requestIsTexture
};


EventsManager::EventsManager() {
	_ready = false;

	_quitRequested = false;
}

void EventsManager::init() {
	if (!GfxMan.ready())
		throw Common::Exception("The GraphicsManager needs to be initialized first");

	RequestMan.init();

	_ready = true;

	_mainThreadID = SDL_ThreadID();
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

bool EventsManager::isMainThread() const {
	return SDL_ThreadID() == _mainThreadID;
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
	if (event.type == kEventKeyDown) {
		if ((event.key.keysym.mod & KMOD_ALT) && event.key.keysym.sym == SDLK_RETURN) {
			GfxMan.toggleFullScreen();
			return true;
		} else if ((event.key.keysym.mod & KMOD_ALT) && event.key.keysym.sym == SDLK_m) {
			GfxMan.toggleMouseGrab();
			return true;
		}
	}

	if (event.type == kEventResize) {
		GfxMan.changeSize(event.resize.w, event.resize.h);
		return true;
	}

	return false;
}

bool EventsManager::parseITC(const Event &event) {
	if (event.type != kEventITC)
		return false;

	// Get the specific ITC type
	ITCEvent itcEvent = (ITCEvent) event.user.code;

	Request &request = *((Request *) event.user.data1);

	if (request._type != itcEvent)
		throw Common::Exception("Request type does not match the ITC type");

	// Call its request handler
	if ((itcEvent >= 0) || (itcEvent < kITCEventMAX))
		(this->*_requestHandler[itcEvent])(request);

	request.signalReply();
	return true;
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

		if (parseITC(event))
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

	return SDL_PushEvent(&event) == 0;
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

void EventsManager::requestFullscreen(Request &request) {
	GfxMan.setFullScreen(true);
}

void EventsManager::requestWindowed(Request &request) {
	GfxMan.setFullScreen(false);
}

void EventsManager::requestResize(Request &request) {
	GfxMan.changeSize(request._resize.width, request._resize.height);
}

void EventsManager::requestCreateTextures(Request &request) {
	GfxMan.createTextures(request._createTextures.count, request._createTextures.ids);
}

void EventsManager::requestDestroyTextures(Request &request) {
	GfxMan.destroyTextures(request._destroyTextures.count, request._destroyTextures.ids);
}

void EventsManager::requestLoadTexture(Request &request) {
	GfxMan.loadTexture(request._loadTexture.id, request._loadTexture.data,
	                   request._loadTexture.width, request._loadTexture.height,
	                   request._loadTexture.format);
}

void EventsManager::requestIsTexture(Request &request) {
	request._isTexture.answer = GfxMan.isTexture(request._isTexture.id);
}

} // End of namespace Events
