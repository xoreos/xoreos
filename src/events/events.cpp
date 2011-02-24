/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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
#include "common/threads.h"
#include "common/configman.h"

#include "events/events.h"
#include "events/requests.h"

#include "graphics/types.h"
#include "graphics/graphics.h"
#include "graphics/texture.h"
#include "graphics/listcontainer.h"

DECLARE_SINGLETON(Events::EventsManager)

namespace Events {

const EventsManager::RequestHandler EventsManager::_requestHandler[kITCEventMAX] = {
	0,
	&EventsManager::requestFullscreen,
	&EventsManager::requestWindowed,
	&EventsManager::requestResize,
	&EventsManager::requestChangeFSAA,
	&EventsManager::requestChangeVSync,
	&EventsManager::requestRebuildGLContainer,
	&EventsManager::requestDestroyGLContainer
};


EventsManager::EventsManager() {
	_ready = false;

	_quitRequested = false;
	_doQuit        = false;
}

void EventsManager::init() {
	if (!GfxMan.ready())
		throw Common::Exception("The GraphicsManager needs to be initialized first");

	RequestMan.init();

	_fullQueue = false;
	_queueSize = 0;

	_ready = true;
}

void EventsManager::deinit() {
	if (!_ready)
		return;

	RequestMan.deinit();

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

bool EventsManager::isQueueFull() const {
	return _fullQueue;
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
		} else if ((event.key.keysym.mod & KMOD_ALT) && event.key.keysym.sym == SDLK_s) {
			GfxMan.takeScreenshot();
			return true;
		}
	}

	if (event.type == kEventResize) {
		GfxMan.setScreenSize(event.resize.w, event.resize.h);
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
	if ((itcEvent >= 0) || (itcEvent < kITCEventMAX)) {
		RequestHandler handler = _requestHandler[itcEvent];

		if (handler)
			(this->*handler)(request);
	}

	request.signalReply();
	return true;
}

void EventsManager::processEvents() {
	Common::enforceMainThread();

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

	_queueSize = 0;
	_fullQueue = false;
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
	_eventQueueMutex.lock();

	int result = SDL_PushEvent(&event);
	_queueSize++;

	if (_queueSize >= 50)
		_fullQueue = true;

	// If we can't push the event, wait up to a second for a queue flush
	if (result == -1) {
		_fullQueue = true;

		_eventQueueMutex.unlock();
		_queueProcessed.wait(1000);
		_eventQueueMutex.lock();

		result = SDL_PushEvent(&event);
	}

	_eventQueueMutex.unlock();
	return result == 0;
}

void EventsManager::enableUnicode(bool enable) {
	Common::StackLock lock(_eventQueueMutex);

	SDL_EnableUNICODE(enable ? 1 : 0);
}

void EventsManager::enableKeyRepeat(int delayTime, int interval) {
	SDL_EnableKeyRepeat(delayTime, interval);
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

void EventsManager::doQuit() {
	_doQuit = true;
}

void EventsManager::runMainLoop() {
	while (!_doQuit) {
		// (Pre)Process all events
		processEvents();

		_queueProcessed.signal();

		if (!_quitRequested)
			// Render a frame
			GfxMan.renderScene();
	}
}

void EventsManager::requestFullscreen(Request &request) {
	GfxMan.setFullScreen(true);
}

void EventsManager::requestWindowed(Request &request) {
	GfxMan.setFullScreen(false);
}

void EventsManager::requestResize(Request &request) {
	GfxMan.setScreenSize(request._resize.width, request._resize.height);
}

void EventsManager::requestChangeFSAA(Request &request) {
	GfxMan.setFSAA(request._fsaa.level);
}

void EventsManager::requestChangeVSync(Request &request) {
	// TODO
}

void EventsManager::requestRebuildGLContainer(Request &request) {
	request._glContainer.glContainer->rebuild();
}

void EventsManager::requestDestroyGLContainer(Request &request) {
	request._glContainer.glContainer->destroy();
}

} // End of namespace Events
