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
 *  The context needed to run a Star Wars: Knights of the Old Republic II - The Sith Lords module.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/graphics/camera.h"

#include "src/graphics/aurora/textureman.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/camera.h"
#include "src/engines/aurora/console.h"

#include "src/engines/kotor2/module.h"
#include "src/engines/kotor2/area.h"
#include "src/engines/kotor2/creature.h"

namespace Engines {

namespace KotOR2 {

bool Module::Action::operator<(const Action &s) const {
	return timestamp < s.timestamp;
}


Module::Module(::Engines::Console &console) : Object(kObjectTypeModule),
	_console(&console), _hasModule(false), _running(false), _pc(0),
	_currentTexturePack(-1), _exit(false), _area(0) {

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
	delete _pc;
	_pc = pc;
}

Creature *Module::getPC() {
	return _pc;
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

void Module::load() {
	loadTexturePack();
	loadResources();
	loadIFO();
	loadArea();
}

void Module::loadResources() {
	// Add all available resource files for the module.
	// Apparently, the original game prefers ERFs over RIMs. This is
	// exploited by the KotOR2 TSL Restored Content Mod.

	// General module resources
	_resources.push_back(Common::ChangeID());
	if (!indexOptionalArchive (_module + ".erf", 1000, &_resources.back()))
		   indexMandatoryArchive(_module + ".rim", 1000, &_resources.back());

	// Scripts
	_resources.push_back(Common::ChangeID());
	if (!indexOptionalArchive (_module + "_s.erf", 1001, &_resources.back()))
		   indexMandatoryArchive(_module + "_s.rim", 1001, &_resources.back());

	// Dialogs, KotOR2 only
	_resources.push_back(Common::ChangeID());
	if (!indexOptionalArchive(_module + "_dlg.erf", 1002, &_resources.back()))
		   indexOptionalArchive(_module + "_dlg.rim", 1002, &_resources.back());

	// Layouts, Xbox only
	_resources.push_back(Common::ChangeID());
	indexOptionalArchive(_module + "_a.rim"  , 1003, &_resources.back());

	// Textures, Xbox only
	_resources.push_back(Common::ChangeID());
	indexOptionalArchive(_module + "_adx.rim", 1004, &_resources.back());
}

void Module::loadIFO() {
	_ifo.load();

	_tag  = _ifo.getTag();
	_name = _ifo.getName().getString();

	readScripts(*_ifo.getGFF());
}

void Module::loadArea() {
	_area = new Area(*this, _ifo.getEntryArea());
}

static const char *texturePacks[3] = {
	"swpc_tex_tpc.erf", // Worst
	"swpc_tex_tpb.erf", // Medium
	"swpc_tex_tpa.erf"  // Best
};

void Module::loadTexturePack() {
	int level = ConfigMan.getInt("texturepack", 2);
	if (_currentTexturePack == level)
		// Nothing to do
		return;

	unloadTexturePack();

	status("Loading texture pack %d", level);
	indexOptionalArchive(texturePacks[level], 400, &_textures);

	// If we already had a texture pack loaded, reload all textures
	if (_currentTexturePack != -1)
		TextureMan.reloadAll();

	_currentTexturePack = level;
}

void Module::unload(bool completeUnload) {
	leaveArea();
	unloadArea();

	if (completeUnload) {
		unloadPC();
		unloadTexturePack();
	}

	unloadIFO();
	unloadResources();

	_eventQueue.clear();
	_delayedActions.clear();

	_newModule.clear();
	_hasModule = false;

	_module.clear();
}

void Module::unloadResources() {
	std::list<Common::ChangeID>::reverse_iterator r;
	for (r = _resources.rbegin(); r != _resources.rend(); ++r)
		deindexResources(*r);

	_resources.clear();
}

void Module::unloadIFO() {
	_ifo.unload();
}

void Module::unloadArea() {
	delete _area;
	_area = 0;
}

void Module::unloadPC() {
	delete _pc;
	_pc = 0;
}

void Module::unloadTexturePack() {
	deindexResources(_textures);
	_currentTexturePack = -1;
}

void Module::changeModule(const Common::UString &module) {
	_newModule = module;
}

void Module::replaceModule() {
	if (_newModule.empty())
		return;

	_console->hide();

	Common::UString newModule = _newModule;

	unload(false);

	_exit = true;

	loadModule(newModule);
	enter();
}

void Module::enter() {
	if (!isLoaded())
		throw Common::Exception("Module::enter(): Lacking a module?!?");

	_console->printf("Entering module \"%s\"", _name.c_str());

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	float entryX, entryY, entryZ, entryDirX, entryDirY;
	_ifo.getEntryPosition(entryX, entryY, entryZ);
	_ifo.getEntryDirection(entryDirX, entryDirY);

	const float entryAngle = -Common::rad2deg(atan2(entryDirX, entryDirY));

	// Roughly head position
	CameraMan.setPosition(entryX, entryY, entryZ + 1.8f);
	CameraMan.setOrientation(90.0f, 0.0f, entryAngle);
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

	runScript(kScriptModuleLoad , this, _pc);
	runScript(kScriptModuleStart, this, _pc);
	runScript(kScriptEnter      , this, _pc);

	_area->runScript(kScriptEnter, _area, _pc);
}

void Module::leaveArea() {
	if (_area) {
		_area->runScript(kScriptExit, _area, _pc);

		_area->hide();
	}

	runScript(kScriptExit, this, _pc);
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
			if (handleCameraInput(*event))
				continue;

		_area->addEvent(*event);
	}

	_eventQueue.clear();

	CameraMan.update();

	_area->processEventQueue();
}

void Module::handleActions() {
	uint32 now = EventMan.getTimestamp();

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

const Aurora::IFOFile &Module::getIFO() const {
	return _ifo;
}

Area *Module::getCurrentArea() {
	return _area;
}

void Module::delayScript(const Common::UString &script,
                         const Aurora::NWScript::ScriptState &state,
                         Aurora::NWScript::Object *owner,
                         Aurora::NWScript::Object *triggerer, uint32 delay) {
	Action action;

	action.type      = kActionScript;
	action.script    = script;
	action.state     = state;
	action.owner     = owner;
	action.triggerer = triggerer;
	action.timestamp = EventMan.getTimestamp() + delay;

	_delayedActions.insert(action);
}

} // End of namespace KotOR2

} // End of namespace Engines
