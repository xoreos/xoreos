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
 *  A module.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/graphics/camera.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/camera.h"
#include "src/engines/aurora/console.h"

#include "src/engines/jade/module.h"
#include "src/engines/jade/area.h"

namespace Engines {

namespace Jade {

Module::Module(::Engines::Console &console) : _console(&console),
	_hasModule(false), _running(false), _exit(false), _area(0) {

}

Module::~Module() {
	try {
		clear();
	} catch (...) {
	}
}

void Module::clear() {
	unload();
}

void Module::load(const Common::UString &module) {
	if (isRunning()) {
		// We are currently running a module. Schedule a safe change instead

		changeModule(module);
		return;
	}

	// We are not currently running a module. Directly load the new module
	loadModule(module);
}

void Module::loadModule(const Common::UString &module) {
	unload();

	_module = module;

	try {

		load();

	} catch (Common::Exception &e) {
		_module.clear();

		e.add("Failed loading module \"%s\"", module.c_str());
		throw e;
	}

	_newModule.clear();

	_hasModule = true;
}

void Module::run() {
	enter();
	_running = true;

	EventMan.enableKeyRepeat();

	try {

		EventMan.flushEvents();

		while (!EventMan.quitRequested() && !_exit) {
			replaceModule();
			if (_exit)
				break;

			handleEvents();

			if (!EventMan.quitRequested() && !_exit)
				EventMan.delay(10);
		}

	} catch (Common::Exception &e) {
		_running = false;

		e.add("Failed running module \"%s\"", _module.c_str());
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

void Module::load() {
	loadArea();
}

void Module::loadArea() {
	_area = new Area;

	_area->load(_module);
}

void Module::unload() {
	leave();

	unloadArea();

	_newModule.clear();
	_hasModule = false;

	_module.clear();

	_areaName.clear();
}

void Module::unloadArea() {
	delete _area;
	_area = 0;
}

void Module::changeModule(const Common::UString &module) {
	_newModule = module;
}

void Module::replaceModule() {
	if (_newModule.empty())
		return;

	_console->hide();

	Common::UString newModule = _newModule;

	unload();

	_exit = true;

	loadModule(newModule);
	enter();
}

void Module::enter() {
	if (!_hasModule || !_area)
		throw Common::Exception("Module::enter(): Lacking a module?!?");

	_console->printf("Entering module \"%s\"", _module.c_str());

	_exit = false;

	CameraMan.reset();
	CameraMan.setOrientation(90.0f, 0.0f, 0.0f);
	CameraMan.update();

	_area->show();
}

void Module::leave() {
	if (!_area)
		return;

	_area->hide();
}

void Module::handleEvents() {
	Events::Event event;
	while (EventMan.pollEvent(event)) {
		// Handle console
		if (_console->processEvent(event)) {
			if (!_area)
				return;

			continue;
		}

		if (event.type == Events::kEventKeyDown) {
			// Console
			if ((event.key.keysym.sym == SDLK_d) && (event.key.keysym.mod & KMOD_CTRL)) {
				_console->show();
				continue;
			}
		}

		// Camera
		if (!_console->isVisible())
			if (handleCameraInput(event))
				continue;

		_area->addEvent(event);
	}

	CameraMan.update();
	_area->processEventQueue();
}

const Common::UString &Module::getName() const {
	return _module;
}

Area *Module::getCurrentArea() {
	return _area;
}

} // End of namespace Jade

} // End of namespace Engines
