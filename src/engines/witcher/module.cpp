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
 *  The context needed to run a The Witcher module.
 */

#include <cassert>

#include <memory>

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/readfile.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"

#include "src/aurora/resman.h"
#include "src/aurora/erffile.h"
#include "src/aurora/gff3file.h"

#include "src/graphics/camera.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/console.h"

#include "src/engines/witcher/module.h"
#include "src/engines/witcher/area.h"
#include "src/engines/witcher/creature.h"

namespace Engines {

namespace Witcher {

bool Module::Action::operator<(const Action &s) const {
	return timestamp < s.timestamp;
}


Module::Module(::Engines::Console &console) : Object(kObjectTypeModule), _console(&console),
	_hasModule(false), _running(false), _exit(false), _pc(0), _currentArea(0) {

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

Area *Module::getCurrentArea() {
	return _currentArea;
}

Creature *Module::getPC() {
	return _pc;
}

bool Module::isLoaded() const {
	return _hasModule;
}

bool Module::isRunning() const {
	return !EventMan.quitRequested() && _running && !_exit && !_newArea.empty();
}

void Module::load(const Common::UString &module, const Common::UString &entryLocation) {
	if (isRunning()) {
		// We are currently running a module. Schedule a safe change instead

		changeModule(module, entryLocation);
		return;
	}

	// We are not currently running a module. Directly load the new module
	loadModule(module, entryLocation);
}

void Module::loadModule(const Common::UString &module, const Common::UString &entryLocation) {
	unload();

	if (module.empty())
		throw Common::Exception("Tried to load an empty module");

	_module        = module;
	_entryLocation = entryLocation;

	try {
		indexMandatoryArchive(module, 1001, &_resModule);

		_ifo.load();

		if (_ifo.isSave())
			throw Common::Exception("This is a save");

		_tag  = _ifo.getTag();
		_name = _ifo.getName();

		readScripts(*_ifo.getGFF());

	} catch (Common::Exception &e) {
		e.add("Can't load module \"%s\"", module.c_str());
		throw e;
	}

	_newModule.clear();

	_hasModule = true;
}

void Module::changeModule(const Common::UString &module, const Common::UString &entryLocation) {
	_newModule     = module;
	_entryLocation = entryLocation;
}

void Module::replaceModule() {
	if (_newModule.empty())
		return;

	_console->hide();

	assert(_pc);

	const Common::UString newModule     = _newModule;
	const Common::UString entryLocation = _entryLocation;

	Creature *pc = _pc;

	unload();

	_exit = true;

	loadModule(newModule, entryLocation);
	enter(*pc);
}

void Module::enter(Creature &pc) {
	if (!isLoaded())
		throw Common::Exception("Module::enter(): Lacking a module?!?");

	try {

		loadAreas();

	} catch (Common::Exception &e) {
		e.add("Can't initialize module \"%s\"", _name.getString().c_str());
		throw e;
	}

	_pc = &pc;
	addObject(*_pc);

	float entryX, entryY, entryZ, entryAngle;
	if (!getEntryObjectLocation(_newArea, entryX, entryY, entryZ, entryAngle))
		getEntryIFOLocation(_newArea, entryX, entryY, entryZ, entryAngle);

	_pc->setPosition(entryX, entryY, entryZ);
	_pc->setOrientation(0.0f, 0.0f, 1.0f, entryAngle);

	_pc->loadModel();

	// Roughly head position
	CameraMan.setPosition(entryX, entryY, entryZ + 1.8f);
	CameraMan.setOrientation(90.0f, 0.0f, entryAngle);
	CameraMan.update();

	_console->printf("Entering module \"%s\" with character \"%s\"",
	                 _name.getString().c_str(), _pc->getName().getString().c_str());

	runScript(kScriptModuleLoad , this, _pc);
	runScript(kScriptModuleStart, this, _pc);
	runScript(kScriptEnter      , this, _pc);

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	CameraMan.reset();

	// Roughly head position
	CameraMan.setPosition(entryX, entryY, entryZ + 1.8f);
	CameraMan.setOrientation(90.0f, 0.0f, entryAngle);
	CameraMan.update();

	_running = true;
	_exit    = false;
}

bool Module::getObjectLocation(const Common::UString &object, Common::UString &area,
                               float &x, float &y, float &z, float &angle) {

	if (object.empty())
		return false;

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(findObjectsByTag(object));

	Witcher::Object *witcherObject = 0;
	while (!witcherObject && search->get()) {
		witcherObject = Witcher::ObjectContainer::toObject(search->next());
		if (!witcherObject || (witcherObject->getType() != kObjectTypeWaypoint))
			witcherObject = 0;
	}

	if (!witcherObject)
		return false;

	// TODO: Entry orientation

	if (witcherObject->getArea())
		area = witcherObject->getArea()->getResRef();

	witcherObject->getPosition(x, y, z);
	angle = 0.0f;

	return true;
}

bool Module::getEntryObjectLocation(Common::UString &entryArea,
                                    float &entryX, float &entryY, float &entryZ, float &entryAngle) {

	return getObjectLocation(_entryLocation, entryArea, entryX, entryY, entryZ, entryAngle);
}

void Module::getEntryIFOLocation(Common::UString &entryArea,
                                 float &entryX, float &entryY, float &entryZ, float &entryAngle) {

	entryArea = _ifo.getEntryArea();

	_ifo.getEntryPosition(entryX, entryY, entryZ);

	float entryDirX, entryDirY;
	_ifo.getEntryDirection(entryDirX, entryDirY);

	entryAngle = -Common::rad2deg(atan2(entryDirX, entryDirY));
}

void Module::leave() {
	runScript(kScriptExit, this, _pc);

	_running = false;
	_exit    = true;
}

void Module::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Module::processEventQueue() {
	if (!isRunning())
		return;

	replaceModule();
	enterArea();

	if (!isRunning())
		return;

	handleEvents();
}

void Module::enterArea() {
	if (_currentArea && (_currentArea->getResRef() == _newArea))
		return;

	if (_currentArea) {
		_currentArea->runScript(kScriptExit, _currentArea, _pc);
		_pc->setArea(0);

		_pc->hide();

		_currentArea->hide();

		_currentArea = 0;
	}

	if (_newArea.empty()) {
		_exit = true;
		return;
	}

	AreaMap::iterator area = _areas.find(_newArea);
	if (area == _areas.end() || !area->second) {
		warning("Failed entering area \"%s\": No such area", _newArea.c_str());
		_exit = true;
		return;
	}

	_currentArea = area->second;

	_currentArea->show();
	_pc->show();

	_pc->setArea(_currentArea);

	_currentArea->runScript(kScriptEnter, _currentArea, _pc);

	EventMan.flushEvents();

	_console->printf("Entering area \"%s\" (\"%s\")", _currentArea->getResRef().c_str(),
			_currentArea->getName().getString().c_str());
}

void Module::handleEvents() {
	for (EventQueue::const_iterator event = _eventQueue.begin(); event != _eventQueue.end(); ++event)
		_currentArea->addEvent(*event);

	_eventQueue.clear();

	_currentArea->processEventQueue();
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

void Module::unload() {
	unloadPC();
	unloadAreas();
	unloadModule();
}

void Module::unloadModule() {
	_tag.clear();

	_ifo.unload();

	deindexResources(_resModule);

	_module.clear();
	_newModule.clear();

	_entryLocation.clear();

	_eventQueue.clear();
	_delayedActions.clear();

	_hasModule = false;
	_running   = false;
	_exit      = true;
}

void Module::loadAreas() {
	status("Loading areas...");

	const std::vector<Common::UString> &areas = _ifo.getAreas();
	for (size_t i = 0; i < areas.size(); i++) {
		status("Loading area \"%s\" (%d / %d)", areas[i].c_str(), (int)i, (int)areas.size() - 1);

		std::pair<AreaMap::iterator, bool> result;

		result = _areas.insert(std::make_pair(areas[i], (Area *) 0));
		if (!result.second)
			throw Common::Exception("Area tag collision: \"%s\"", areas[i].c_str());

		try {
			result.first->second = new Area(*this, areas[i].c_str());
		} catch (Common::Exception &e) {
			e.add("Can't load area \"%s\"", areas[i].c_str());
			throw;
		}
	}
}

void Module::unloadAreas() {
	_areas.clear();
	_newArea.clear();

	_currentArea = 0;
}

void Module::unloadPC() {
	if (!_pc)
		return;

	removeObject(*_pc);

	_pc->hide();
	_pc = 0;
}

void Module::movePC(const Common::UString &area) {
	if (!_pc)
		return;

	float x, y, z;
	_pc->getPosition(x, y, z);

	movePC(area, x, y, z);
}

void Module::movePC(float x, float y, float z) {
	if (!_pc)
		return;

	movePC(_currentArea, x, y, z);
}

void Module::movePC(const Common::UString &area, float x, float y, float z) {
	if (!_pc)
		return;

	Area *pcArea = 0;

	AreaMap::iterator a = _areas.find(area);
	if (a != _areas.end())
		pcArea = a->second;

	movePC(pcArea, x, y, z);
}

void Module::movePC(Area *area, float x, float y, float z) {
	if (!_pc)
		return;

	_pc->setArea(area);
	_pc->setPosition(x, y, z);

	movedPC();
}

void Module::movePC(const Common::UString &module, const Common::UString &object) {
	if (module.empty() || (module == _module)) {
		float x, y, z, angle;
		Common::UString area;

		if (getObjectLocation(object, area, x, y, z, angle))
			movePC(area, x, y, z);

		return;
	}

	load(module, object);
}

void Module::movedPC() {
	if (!_pc)
		return;

	float x, y, z;
	_pc->getPosition(x, y, z);

	// Roughly head position
	CameraMan.setPosition(x, y, z + 1.8f);
	CameraMan.update();

	_newArea.clear();
	if (_pc->getArea())
		_newArea = _pc->getArea()->getResRef();
}

const Aurora::IFOFile &Module::getIFO() const {
	return _ifo;
}

const Aurora::LocString &Module::getName() const {
	return Witcher::Object::getName();
}

const Aurora::LocString &Module::getDescription() const {
	return Witcher::Object::getDescription();
}

void Module::refreshLocalized() {
	for (AreaMap::iterator a = _areas.begin(); a != _areas.end(); ++a)
		a->second->refreshLocalized();
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

Common::UString Module::getName(const Common::UString &module) {
	try {
		const Aurora::ERFFile mod(new Common::ReadFile(findModule(module, false)));
		const uint32_t ifoIndex = mod.findResource("module", Aurora::kFileTypeIFO);

		const Aurora::GFF3File ifo(mod.getResource(ifoIndex), MKTAG('I', 'F', 'O', ' '));

		return ifo.getTopLevel().getString("Mod_Name");

	} catch (...) {
	}

	return "";
}

Common::UString Module::getDescription(const Common::UString &module) {
	try {
		const Aurora::ERFFile mod(new Common::ReadFile(findModule(module, false)));
		const uint32_t ifoIndex = mod.findResource("module", Aurora::kFileTypeIFO);

		const Aurora::GFF3File ifo(mod.getResource(ifoIndex), MKTAG('I', 'F', 'O', ' '));

		return ifo.getTopLevel().getString("Mod_Description");

	} catch (...) {
	}

	return "";
}

Common::UString Module::findModule(const Common::UString &module, bool relative) {
	const Common::FileList modFiles(ConfigMan.getString("WITCHER_moduleDir"), -1);

	for (Common::FileList::const_iterator m = modFiles.begin(); m != modFiles.end(); ++m) {
		if (!Common::FilePath::getFile(*m).equalsIgnoreCase(module + ".mod") &&
		    !Common::FilePath::getFile(*m).equalsIgnoreCase(module + ".adv"))
			continue;

		if (!relative)
			return *m;

		return Common::FilePath::relativize(ResMan.getDataBase(), *m);
	}

	return "";
}

} // End of namespace Witcher

} // End of namespace Engines
