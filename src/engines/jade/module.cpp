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
#include "src/engines/aurora/console.h"
#include "src/engines/aurora/flycamera.h"

#include "src/engines/jade/module.h"
#include "src/engines/jade/area.h"
#include "src/engines/jade/creature.h"
#include "src/engines/jade/gui/chargen/characterinfo.h"

namespace Engines {

namespace Jade {

bool Module::Action::operator<(const Action &s) const {
	return timestamp < s.timestamp;
}


Module::Module(::Engines::Console &console) : _console(&console), _hasModule(false),
	_running(false), _exit(false) {

}

Module::~Module() {
	try {
		clear();
	} catch (...) {
	}
}

void Module::clear() {
	unload(true);
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
	unload(false);

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

void Module::usePC(Creature *pc) {
	unloadPC();

	_pc.reset(pc);

	addObject(*_pc);
}

void Module::usePC(const CharacterInfo &info) {
	unloadPC();

	_pc = std::make_unique<Creature>();
	_pc->createPC(info);

	addObject(*_pc);
}

Creature *Module::getPC() {
	return _pc.get();
}

bool Module::isLoaded() const {
	return _hasModule && _area && _pc;
}

bool Module::isRunning() const {
	return !EventMan.quitRequested() && _running && !_exit;
}

void Module::exit() {
	_exit = true;
}

void Module::showMenu() {
	// TODO: Module::showMenu()
}

bool Module::startConversation (const Common::UString &UNUSED(conv), Creature &UNUSED(pc), Object &UNUSED(obj),
                                bool UNUSED(noWidescreen), bool UNUSED(resetZoom)) {
	// TODO
	return false;
}

void Module::load() {
	loadArea();
}

void Module::loadArea() {
	_area = std::make_unique<Area>(*this, _module);
}

void Module::unload(bool completeUnload) {
	leaveArea();
	unloadArea();

	if (completeUnload) {
		unloadPC();
	}

	_eventQueue.clear();
	_delayedActions.clear();

	_newModule.clear();
	_hasModule = false;

	_module.clear();
}

void Module::unloadArea() {
	_area.reset();
}

void Module::unloadPC() {
	if (!_pc)
		return;

	removeObject(*_pc);

	_pc.reset();
}

void Module::changeModule(const Common::UString &module) {
	if (module == getName()) {
		warning("Module \"%s\" is already currently loaded", module.c_str());
		return;
	}

	_newModule = module;
}

void Module::replaceModule() {
	if (_newModule.empty())
		return;

	_console->hide();

	const Common::UString newModule = _newModule;

	unload(false);

	_exit = true;

	loadModule(newModule);
	enter();
}

void Module::enter() {
	if (!_hasModule || !_area)
		throw Common::Exception("Module::enter(): Lacking a module?!?");

	if (!_pc)
		throw Common::Exception("Module::enter(): Lacking a PC?!?");

	_console->printf("Entering module \"%s\"", _module.c_str());

	CameraMan.reset();
	CameraMan.setOrientation(90.0f, 0.0f, 0.0f);
	CameraMan.update();

	enterArea();

	_running = true;
	_exit    = false;
}

void Module::leave() {
	leaveArea();

	_running = false;
	_exit    = true;
}

void Module::enterArea() {
	_area->show();

	_area->runScript(kScriptOnEnter, _area.get(), _pc.get());
}

void Module::leaveArea() {
	if (_area) {
		_area->runScript(kScriptOnExit, _area.get(), _pc.get());

		_area->hide();
	}
}

void Module::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Module::processEventQueue() {
	if (!isRunning())
		return;

	replaceModule();

	if (!isRunning())
		return;

	handleEvents();
	handleActions();
}

void Module::handleEvents() {
	for (EventQueue::const_iterator event = _eventQueue.begin(); event != _eventQueue.end(); ++event) {
		// Handle console
		if (_console->isVisible()) {
			_console->processEvent(*event);
			continue;
		}

		if (event->type == Events::kEventKeyDown) {
			// Menu
			if (event->key.keysym.sym == SDLK_ESCAPE) {
				showMenu();
				continue;
			}

			// Console
			if ((event->key.keysym.sym == SDLK_d) && (event->key.keysym.mod & KMOD_CTRL)) {
				_console->show();
				continue;
			}
		}

		// Camera
		if (!_console->isVisible())
			if (FlyCam.handleCameraInput(*event))
				continue;

		_area->addEvent(*event);
	}

	_eventQueue.clear();

	CameraMan.update();

	_area->processEventQueue();
}

void Module::handleActions() {
	uint32_t now = EventMan.getTimestamp();

	while (!_delayedActions.empty()) {
		ActionQueue::iterator action = _delayedActions.begin();

		if (now < action->timestamp)
			break;

		if (action->type == kActionScript)
			ScriptContainer::runScript(action->script, action->state,
			                           action->owner, action->triggerer);

		_delayedActions.erase(action);
	}
}

void Module::movePC(float x, float y, float z) {
	if (!_pc)
		return;

	_pc->setPosition(x, y, z);
	movedPC();
}

void Module::movePC(const Common::UString &module) {
	if (module.empty() || (module == _module))
		return;

	loadModule(module);
}

void Module::movedPC() {
	if (!_pc)
		return;

	float x, y, z;
	_pc->getPosition(x, y, z);

	// Roughly head position
	CameraMan.setPosition(x, y, z + 1.8f);
	CameraMan.update();
}

const Common::UString &Module::getName() const {
	return _module;
}

Area *Module::getCurrentArea() {
	return _area.get();
}

void Module::delayScript(const Common::UString &script,
                         const Aurora::NWScript::ScriptState &state,
                         Aurora::NWScript::Object *owner,
                         Aurora::NWScript::Object *triggerer, uint32_t delay) {
	Action action;

	action.type      = kActionScript;
	action.script    = script;
	action.state     = state;
	action.owner     = owner;
	action.triggerer = triggerer;
	action.timestamp = EventMan.getTimestamp() + delay;

	_delayedActions.insert(action);
}

} // End of namespace Jade

} // End of namespace Engines
