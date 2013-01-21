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

/** @file events/events.cpp
 *  The global events manager.
 */

#include <cstdlib>

#include <SDL_timer.h>

#include "common/util.h"
#include "common/error.h"
#include "common/threads.h"
#include "common/configman.h"

#include "events/events.h"
#include "events/requests.h"
#include "events/notifications.h"
#include "events/timerman.h"
#include "events/joystick.h"

#include "graphics/types.h"
#include "graphics/graphics.h"
#include "graphics/glcontainer.h"

DECLARE_SINGLETON(Events::EventsManager)

namespace Events {

const EventsManager::RequestHandler EventsManager::_requestHandler[kITCEventMAX] = {
	0,
	&EventsManager::requestCallInMainThread,
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
	NotificationMan.init();
	TimerMan.init();

	_fullQueue = false;
	_queueSize = 0;

	_ready = true;

	initJoysticks();

	std::srand(getTimestamp());
}

void EventsManager::deinit() {
	if (!_ready)
		return;

	deinitJoysticks();

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

	deinitJoysticks();
	initJoysticks();
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

#ifdef WIN32
	if ((event.type == kEventKeyDown) && (event.key.keysym.mod & KMOD_ALT) &&
			 (event.key.keysym.sym == SDLK_F4)) {
		requestQuit();
		return true;
	}
#endif

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

void EventsManager::flushEvents() {
	Common::StackLock lock(_eventQueueMutex);

	_eventQueue.clear();
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
	if (_queueSize >= 50)
		if (!Common::isMainThread())
			_queueProcessed.wait(100);

	Common::StackLock lock(_eventQueueMutex);

	int result = SDL_PushEvent(&event);
	_queueSize++;

	return result == 0;
}

void EventsManager::enableUnicode(bool enable) {
	Common::StackLock lock(_eventQueueMutex);

	SDL_EnableUNICODE(enable ? 1 : 0);
}

void EventsManager::enableKeyRepeat(int delayTime, int interval) {
	SDL_EnableKeyRepeat(delayTime, interval);
}

uint32 EventsManager::getPressedCharacter(const Event &event) {
	uint32 c = event.key.keysym.unicode;
	if ((event.type != kEventKeyDown) || (Common::UString::isCntrl(c)))
		return 0;

	return c;
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

void EventsManager::initJoysticks() {
	deinitJoysticks();

	const int joyCount = SDL_NumJoysticks();
	if (joyCount <= 0)
		return;

	_joysticks.reserve(joyCount);
	for (int i = 0; i < joyCount; i++)
		_joysticks.push_back(new Joystick(i));

	SDL_JoystickEventState(SDL_ENABLE);
}

void EventsManager::deinitJoysticks() {
	SDL_JoystickEventState(SDL_DISABLE);

	for (Joysticks::iterator j = _joysticks.begin(); j != _joysticks.end(); ++j)
		delete *j;

	_joysticks.clear();
}

int EventsManager::getJoystickCount() const {
	return _joysticks.size();
}

Joystick *EventsManager::getJoystickByIndex(int index) const {
	if ((index < 0) || (((uint) index) >= _joysticks.size()))
		return 0;

	return _joysticks[index];
}

Joystick *EventsManager::getJoystickByName(const Common::UString &name) const {
	for (Joysticks::const_iterator j = _joysticks.begin(); j != _joysticks.end(); ++j)
		if ((*j)->getName() == name)
			return *j;

	return 0;
}

void EventsManager::requestCallInMainThread(Request &request) {
	(*request._callInMainThread.caller)();
}

void EventsManager::requestRebuildGLContainer(Request &request) {
	request._glContainer.glContainer->rebuild();
}

void EventsManager::requestDestroyGLContainer(Request &request) {
	request._glContainer.glContainer->destroy();
}

} // End of namespace Events
