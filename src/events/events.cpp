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

#include <cstdlib>

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_freetype.h"
#include "external/imgui/imgui_impl_sdl.h"

#include "src/common/fallthrough.h"
START_IGNORE_IMPLICIT_FALLTHROUGH
#include <SDL_timer.h>
STOP_IGNORE_IMPLICIT_FALLTHROUGH

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/threads.h"
#include "src/common/configman.h"

#include "src/events/events.h"
#include "src/events/requests.h"
#include "src/events/notifications.h"
#include "src/events/timerman.h"
#include "src/events/joystick.h"
#include "src/events/gamecontroller.h"

#include "src/graphics/types.h"
#include "src/graphics/graphics.h"
#include "src/graphics/glcontainer.h"
#include "src/graphics/windowman.h"

DECLARE_SINGLETON(Events::EventsManager)

namespace Events {

const EventsManager::RequestHandler EventsManager::_requestHandler[kITCEventMAX] = {
	0,
	&EventsManager::requestCallInMainThread,
	&EventsManager::requestRebuildGLContainer,
	&EventsManager::requestDestroyGLContainer
};


EventsManager::EventsManager() : _ready(false), _quitRequested(false), _doQuit(false),
	_fatalError(false), _queueSize(0), _fullQueue(false), _repeat(false), _repeatCounter(0),
	_textInputCounter(0) {

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

	// Load game controller mapping database if available.
	if (ConfigMan.hasKey("gamecontrollerdb")) {
		Common::UString gameControllerDatabase = ConfigMan.getString("gamecontrollerdb");
		if (SDL_GameControllerAddMappingsFromFile(gameControllerDatabase.c_str()) == -1)
			warning("Could not load controller database: %s", SDL_GetError());
	}

	initJoysticks();

	SDL_RegisterEvents(1);

	// Forcing enableTextInput to be disabled requires _textInputCounter = 1 to not underrun the counter.
	_textInputCounter = 1;
	enableTextInput(false);

	_repeatCounter = 0;

	ImGuiIO &io = ImGui::GetIO();
	io.WantCaptureKeyboard = true;
	io.WantCaptureMouse = true;
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

	std::lock_guard<std::recursive_mutex> lock(_eventQueueMutex);

	// Clear the SDL event queue
	while (SDL_PollEvent(0));

	// Clear our event queue
	_eventQueue.clear();

	deinitJoysticks();
	initJoysticks();

	_repeatCounter = 0;

	_textInputCounter = 0;
}

bool EventsManager::ready() const {
	return _ready;
}

bool EventsManager::isQueueFull() const {
	return _fullQueue;
}

void EventsManager::delay(uint32_t ms) {
	if (!_quitRequested)
		std::this_thread::sleep_for(std::chrono::duration<uint32_t, std::milli>(ms));
}

uint32_t EventsManager::getTimestamp() const {
	return SDL_GetTicks();
}

bool EventsManager::parseEventQuit(const Event &event) {
	if ((event.type == kEventQuit) ||
			((event.type == kEventKeyDown) &&
			 (event.key.keysym.mod & (KMOD_CTRL | KMOD_GUI)) &&
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
			WindowMan.toggleFullScreen();
			return true;
		} else if ((event.key.keysym.mod & KMOD_ALT) && event.key.keysym.sym == SDLK_m) {
			WindowMan.toggleMouseGrab();
			return true;
		} else if ((event.key.keysym.mod & KMOD_ALT) && event.key.keysym.sym == SDLK_s) {
			GfxMan.takeScreenshot();
			return true;
		}
	}

	if (event.type == kEventWindow) {
		// If the window was restored, reassert the window size
		if (event.window.event == kEventWindowRestored)
			WindowMan.setWindowSize(WindowMan.getWindowWidth(), WindowMan.getWindowHeight());

		return true;
	}

	return false;
}

bool EventsManager::parseITC(const Event &event) {
	if (event.type != kEventITC)
		return false;

	// Get the specific ITC type
	ITCEvent itcEvent = (ITCEvent) event.user.code;

	Request &request = *static_cast<Request *>(event.user.data1);

	if (request._type != itcEvent)
		throw Common::Exception("Request type does not match the ITC type");

	// Call its request handler
	if ((itcEvent >= 0) && (itcEvent < kITCEventMAX)) {
		RequestHandler handler = _requestHandler[itcEvent];

		if (handler)
			(this->*handler)(request);
	}

	request.signalReply();
	return true;
}

void EventsManager::processEvents() {
	Common::enforceMainThread();

	std::lock_guard<std::recursive_mutex> lock(_eventQueueMutex);

	Event event;
	while (SDL_PollEvent(&event)) {
		// Check for ImGui commands.
		ImGui_ImplSDL2_ProcessEvent(&event);

		// Check repeated event.
		if (((event.type == kEventKeyDown) || (event.type == kEventKeyUp)) &&
				event.key.repeat && !_repeat)
			continue;

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
	std::lock_guard<std::recursive_mutex> lock(_eventQueueMutex);

	_eventQueue.clear();
}

bool EventsManager::pollEvent(Event &event) {
	std::lock_guard<std::recursive_mutex> lock(_eventQueueMutex);

	if (_eventQueue.empty())
		return false;

	// Return an event from the front of the list
	event = _eventQueue.front();
	_eventQueue.pop_front();

	return true;
}

bool EventsManager::pushEvent(Event &event) {
	if ((_queueSize >= 50) && !Common::isMainThread()) {
		std::unique_lock<std::recursive_mutex> lock(_queueProcessedMutex);
		_queueProcessed.wait_for(lock, std::chrono::duration<int, std::milli>(100));
	}

	std::lock_guard<std::recursive_mutex> lock(_eventQueueMutex);

	int result = SDL_PushEvent(&event);
	_queueSize++;

	return result == 1;
}

void EventsManager::enableKeyRepeat(bool repeat) {
	if (!repeat && _repeatCounter == 0)
		throw Common::Exception("EventsManager::enableKeyRepeat(): Counter underrun");

	if (repeat)
		_repeatCounter++;
	else
		_repeatCounter--;

	_repeat = _repeatCounter > 0;
}

void EventsManager::enableTextInput(bool textInput) {
	if (!textInput && _textInputCounter == 0)
		throw Common::Exception("EventsManager::enableTextInput(): Counter underrun");

	if (textInput) {
		if (_textInputCounter == 0)
			SDL_StartTextInput();
		_textInputCounter++;
	} else {
		if (_textInputCounter == 1)
			SDL_StopTextInput();
		_textInputCounter--;
	}
}

Common::UString EventsManager::getTextInput(const Event &event) {
	// TextInput events already contain UTF-8 encoded text
	if (event.type == kEventTextInput)
		return event.text.text;

	if (event.type == kEventKeyDown) {
		uint32_t sym = event.key.keysym.sym;

		// Mask out control characters
		if ((sym & SDLK_SCANCODE_MASK) || Common::UString::isCntrl(sym))
			return "";

		// Interpret this KeySym as an Unicode codepoint
		return Common::UString(sym, 1);
	}

	return "";
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

bool EventsManager::fatalErrorRaised() const {
	return _fatalError;
}

void EventsManager::raiseFatalError() {
	_fatalError    = true;
	_quitRequested = true;
	_doQuit        = true;
}

void EventsManager::runMainLoop() {
	while (!_doQuit) {
		// (Pre)Process all events
		processEvents();

		_queueProcessed.notify_one();

		// Render a frame
		GfxMan.renderScene();
	}
}

void EventsManager::initJoysticks() {
	deinitJoysticks();

	const int joyCount = SDL_NumJoysticks();
	if (joyCount <= 0)
		return;

	bool controllerFound = false;

	_joysticks.reserve(joyCount);
	for (int i = 0; i < joyCount; i++) {
		if (SDL_IsGameController(i)) {
			_joysticks.push_back(new GameController(i));
			controllerFound = true;
		} else {
			_joysticks.push_back(new Joystick(i));
		}
	}

	if (controllerFound)
		SDL_GameControllerEventState(SDL_ENABLE);

	SDL_JoystickEventState(SDL_ENABLE);
}

void EventsManager::deinitJoysticks() {
	SDL_JoystickEventState(SDL_DISABLE);

	_joysticks.clear();
}

size_t EventsManager::getJoystickCount() const {
	return _joysticks.size();
}

Joystick *EventsManager::getJoystickByIndex(size_t index) const {
	if (index >= _joysticks.size())
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
