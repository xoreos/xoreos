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
#include "src/common/filepath.h"

#include "src/graphics/camera.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/camera.h"

#include "src/engines/jade/module.h"
#include "src/engines/jade/console.h"
#include "src/engines/jade/area.h"

namespace Engines {

namespace Jade {

Module::Module(Console &console) : _console(&console),
	_hasModule(false), _running(false), _exit(false), _area(0) {

}

Module::~Module() {
	clear();
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
	loadResources();

	findAreaName();
	loadArea();
}

/** Load all RIMs in $JADE_moduleDir/$Module/ */
void Module::loadResources() {
	Common::UString modulesDir = ConfigMan.getString("JADE_moduleDir");
	if (modulesDir.empty())
		throw Common::Exception("No module directory");

	Common::UString moduleDir = Common::FilePath::findSubDirectory(modulesDir, _module, true);
	if (modulesDir.empty())
		throw Common::Exception("No such module \"%s\"", _module.c_str());

	Common::FileList files;
	files.addDirectory(moduleDir);

	int resources = 0;
	for (Common::FileList::const_iterator f = files.begin(); f != files.end(); ++f) {
		Common::UString file = Common::FilePath::relativize(modulesDir, *f).toLower();
		if (!file.endsWith(".rim"))
			continue;

		Aurora::ResourceManager::ChangeID change;

		indexMandatoryArchive(Aurora::kArchiveRIM, file, 100 + resources++, &change);
		_resources.push_back(change);
	}
}

/** Find the one non-global area definition file hopefully added by the module resources. */
void Module::findAreaName() {
	std::list<Aurora::ResourceManager::ResourceID> areas;
	ResMan.getAvailableResources(Aurora::kFileTypeART, areas);

	// Go through all available areas and remove the global ones
	std::list<Aurora::ResourceManager::ResourceID>::iterator a = areas.begin();
	while (a != areas.end()) {
		if (a->name.toLower().beginsWith("aeg"))
			a = areas.erase(a);
		else
			++a;
	}

	if (areas.empty())
		throw Common::Exception("No area in module \"%s\"", _module.c_str());

	if (areas.size() > 1)
		throw Common::Exception("More than one area in module \"%s\"", _module.c_str());

	_areaName = areas.front().name;
}

void Module::loadArea() {
	_area = new Area;

	_area->load(_areaName);
}

void Module::unload() {
	leave();

	unloadArea();
	unloadResources();

	_newModule.clear();
	_hasModule = false;

	_module.clear();

	_areaName.clear();
}

void Module::unloadResources() {
	std::list<Aurora::ResourceManager::ChangeID>::reverse_iterator r;
	for (r = _resources.rbegin(); r != _resources.rend(); ++r)
		ResMan.undo(*r);

	_resources.clear();
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

	// Roughly head position
	CameraMan.setPosition(0.0, 1.8, 0.0);
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
