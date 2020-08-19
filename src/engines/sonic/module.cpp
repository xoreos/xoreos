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
 *  The context needed to run a Sonic Chronicles: The Dark Brotherhood module.
 */

#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/uuid.h"

#include "src/graphics/camera.h"

#include "src/events/events.h"

#include "src/aurora/nwscript/objectman.h"

#include "src/engines/aurora/console.h"

#include "src/engines/sonic/module.h"
#include "src/engines/sonic/area.h"

namespace Engines {

namespace Sonic {

Module::Module(::Engines::Console &console) : Object(kObjectTypeModule),
	_console(&console), _running(false), _exit(false), _newArea(-1) {
	_id = Common::generateIDNumber();
	ObjectMan.registerObject(this);
}

Module::~Module() {
	ObjectMan.unregisterObject(this);

	try {
		clear();
	} catch (...) {
	}
}

void Module::clear() {
	unload();
}

void Module::run() {
	_newArea = 3;
	_running = true;

	EventMan.enableKeyRepeat();

	try {

		EventMan.flushEvents();

		while (!EventMan.quitRequested() && !_exit) {
			loadArea();
			if (_exit)
				break;

			handleEvents();

			if (!EventMan.quitRequested() && !_exit)
				EventMan.delay(10);
		}

	} catch (Common::Exception &e) {
		_running = false;

		e.add("Failed running module");
		throw e;
	}

	EventMan.enableKeyRepeat(0);

	_running = false;
}

bool Module::isRunning() const {
	return _running;
}

void Module::exit() {
	_exit = true;
}

void Module::loadArea() {
	if (_area && (_area->getID() == (uint32_t)_newArea))
		return;

	unloadArea();

	if (_newArea < 0) {
		_exit = true;
		return;
	}

	_area = std::make_unique<Area>(*this, (uint32_t)_newArea);

	_area->enter();
	_area->show();
}

void Module::unloadArea() {
	if (_area) {
		_area->leave();
		_area->hide();
	}

	_area.reset();
}

void Module::unload() {
	unloadArea();
}

void Module::handleEvents() {
	Events::Event event;
	while (EventMan.pollEvent(event)) {
		// Handle console
		if (_console->isVisible()) {
			_console->processEvent(event);
			continue;
		}

		// Console
		if (event.type == Events::kEventKeyDown) {
			if ((event.key.keysym.sym == SDLK_d) && (event.key.keysym.mod & KMOD_CTRL)) {
				_console->show();
				continue;
			}
		}

		if (handleCameraEvents(event))
			continue;

		if (_area)
			_area->addEvent(event);
	}

	CameraMan.update();

	if (_area)
		_area->processEventQueue();
}

bool Module::handleCameraEvents(const Events::Event &event) {
	if (event.type == Events::kEventKeyDown) {
		float multiplier = 1.0f;
		if (event.key.keysym.mod & KMOD_SHIFT)
			multiplier = 5.0f;

		if      (event.key.keysym.sym      == SDLK_UP)
			CameraMan.move(0.0f, 0.0f, multiplier * -5.0f);
		else if (event.key.keysym.sym      == SDLK_DOWN)
			CameraMan.move(0.0f, 0.0f, multiplier *  5.0f);
		else if (event.key.keysym.sym      == SDLK_LEFT)
			CameraMan.move(multiplier * -5.0f, 0.0f, 0.0f);
		else if (event.key.keysym.sym      == SDLK_RIGHT)
			CameraMan.move(multiplier *  5.0f, 0.0f, 0.0f);
		else if (event.key.keysym.scancode == SDL_SCANCODE_W)
			CameraMan.move(0.0f, 0.0f, multiplier * -5.0f);
		else if (event.key.keysym.scancode == SDL_SCANCODE_S)
			CameraMan.move(0.0f, 0.0f, multiplier *  5.0f);
		else if (event.key.keysym.scancode == SDL_SCANCODE_A)
			CameraMan.move(multiplier * -5.0f, 0.0f, 0.0f);
		else if (event.key.keysym.scancode == SDL_SCANCODE_D)
			CameraMan.move(multiplier *  5.0f, 0.0f, 0.0f);
		else
			return false;
	} else if (event.type == Events::kEventMouseMove) {
		if (event.motion.state & SDL_BUTTON(2))
			CameraMan.move(1.0f * event.motion.xrel, 0.0f, 1.0f * event.motion.yrel);
		else
			return false;
	} else
		return false;

	return true;
}

Area *Module::getCurrentArea() {
	return _area.get();
}

void Module::movePC(int32_t area) {
	_newArea = area;
}

} // End of namespace Sonic

} // End of namespace Engines
