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
 *  The context needed to run a Neverwinter Nights 2 module.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/filelist.h"
#include "src/common/readfile.h"

#include "src/aurora/resman.h"
#include "src/aurora/talkman.h"
#include "src/aurora/erffile.h"
#include "src/aurora/gff3file.h"

#include "src/graphics/camera.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/console.h"

#include "src/engines/nwn2/module.h"
#include "src/engines/nwn2/area.h"
#include "src/engines/nwn2/creature.h"
#include "src/engines/nwn2/faction.h"
#include "src/engines/nwn2/roster.h"
#include "src/engines/nwn2/journal.h"

namespace Engines {

namespace NWN2 {

bool Module::Action::operator<(const Action &s) const {
	return timestamp < s.timestamp;
}


Module::Module(::Engines::Console &console) : Object(kObjectTypeModule), _console(&console),
	_hasModule(false), _running(false), _exit(false), _pc(0), _currentArea(0), _ranPCSpawn(false) {

}

Module::Module() : Object(kObjectTypeModule), _console(nullptr), _hasModule(false),
	_running(false), _exit(false), _pc(0), _currentArea(0), _ranPCSpawn(false) {

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

Factions &Module::getFactions() {
	assert(_factions);

	return *_factions;
}

Roster &Module::getRoster() {
	return *_roster;
}

Journal &Module::getJournal() {
	return *_moduleJournal;
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

	if (module.empty())
		throw Common::Exception("Tried to load an empty module");

	try {
		indexMandatoryArchive(module, 1001, &_resModule);

		_ifo.load();

		if (_ifo.isSave())
			throw Common::Exception("This is a save");

		checkXPs();
		checkHAKs();

		_tag  = _ifo.getTag();
		_name = _ifo.getName().getString();

		readScripts(*_ifo.getGFF());
		readVarTable(*_ifo.getGFF());

	} catch (Common::Exception &e) {
		e.add("Can't load module \"%s\"", module.c_str());
		throw e;
	}

	_newModule.clear();

	_hasModule = true;
}

void Module::checkXPs() {
	uint16_t hasXP = 0;

	hasXP |= ConfigMan.getBool("NWN2_hasXP1") ? 1 : 0;
	hasXP |= ConfigMan.getBool("NWN2_hasXP2") ? 2 : 0;
	hasXP |= ConfigMan.getBool("NWN2_hasXP3") ? 4 : 0;

	uint16_t xp = _ifo.getExpansions();

	for (int i = 0; i < 16; i++, xp >>= 1, hasXP >>= 1)
		if ((xp & 1) && !(hasXP & 1))
			throw Common::Exception("Module requires expansion %d and we don't have it", i + 1);
}

void Module::checkHAKs() {
	const std::vector<Common::UString> &haks = _ifo.getHAKs();

	for (std::vector<Common::UString>::const_iterator h = haks.begin(); h != haks.end(); ++h)
		if (!ResMan.hasArchive(*h + ".hak"))
			throw Common::Exception("Required hak \"%s\" does not exist", h->c_str());
}

void Module::changeModule(const Common::UString &module) {
	_newModule = module;
}

void Module::replaceModule() {
	if (_newModule.empty())
		return;

	if (_console)
		_console->hide();

	assert(_pc);

	Common::UString newModule = _newModule;
	Creature *pc = _pc;

	unload();

	_exit = true;

	loadModule(newModule);
	enter(*pc, false);
}

void Module::enter(Creature &pc, bool isNewCampaign) {
	if (!isLoaded())
		throw Common::Exception("Module::enter(): Lacking a module?!?");

	try {

		loadTLK();
		loadHAKs();
		loadAreas();
		loadFactions();
		loadRoster();
		loadJournal();

	} catch (Common::Exception &e) {
		e.add("Can't initialize module \"%s\"", _name.c_str());
		throw e;
	}

	if (isNewCampaign)
		_ranPCSpawn = false;

	_pc = &pc;
	addObject(*_pc);

	float entryX, entryY, entryZ, entryDirX, entryDirY;
	_ifo.getEntryPosition(entryX, entryY, entryZ);
	_ifo.getEntryDirection(entryDirX, entryDirY);

	const float entryAngle = -Common::rad2deg(atan2(entryDirX, entryDirY));

	_pc->setPosition(entryX, entryY, entryZ);
	_pc->setOrientation(0.0f, 0.0f, 1.0f, entryAngle);

	_pc->loadModel();

	if (_console)
		_console->printf("Entering module \"%s\" with character \"%s\"", _name.c_str(), _pc->getName().c_str());
	else
		status("Entering module \"%s\" with character \"%s\"", _name.c_str(), _pc->getName().c_str());

	runScript(kScriptModuleLoad , this, _pc);
	runScript(kScriptModuleStart, this, _pc);
	runScript(kScriptEnter      , this, _pc);

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	_newArea = _ifo.getEntryArea();

	CameraMan.reset();

	// Roughly head position
	CameraMan.setPosition(entryX, entryY, entryZ + 1.8f);
	CameraMan.setOrientation(90.0f, 0.0f, entryAngle);
	CameraMan.update();

	_running = true;
	_exit    = false;
}

void Module::leave() {
	runScript(kScriptExit, this, _pc);

	_running = false;
	_exit    = true;

	unloadPC();
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
	handleActions();
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

	if (!_ranPCSpawn) {
		runScript(kScriptPCLoaded, this, _pc);

		_ranPCSpawn = true;
	}

	EventMan.flushEvents();

	if (_console)
		_console->printf("Entering area \"%s\" (\"%s\", \"%s\")", _currentArea->getResRef().c_str(),
				 _currentArea->getName().c_str(), _currentArea->getDisplayName().c_str());
	else
		status("Entering area \"%s\" (\"%s\", \"%s\")", _currentArea->getResRef().c_str(),
		       _currentArea->getName().c_str(), _currentArea->getDisplayName().c_str());
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
	unloadFactions();
	unloadRoster();
	unloadJournal();
	unloadHAKs();
	unloadTLK();
	unloadModule();
}

void Module::unloadModule() {
	_tag.clear();

	_ifo.unload();

	deindexResources(_resModule);

	_newModule.clear();

	_eventQueue.clear();
	_delayedActions.clear();

	_hasModule = false;
	_running   = false;
	_exit      = true;
}

void Module::loadTLK() {
	if (_ifo.getTLK().empty())
		return;

	TalkMan.addTable(_ifo.getTLK(), _ifo.getTLK() + "f", true, 0, &_resTLK);
}

void Module::unloadTLK() {
	TalkMan.removeTable(_resTLK);
}

void Module::loadHAKs() {
	const std::vector<Common::UString> &haks = _ifo.getHAKs();

	_resHAKs.resize(haks.size());

	for (size_t i = 0; i < haks.size(); i++)
		indexMandatoryArchive(haks[i] + ".hak", 1002 + i, &_resHAKs[i]);
}

void Module::unloadHAKs() {
	std::vector<Common::ChangeID>::iterator hak;
	for (hak = _resHAKs.begin(); hak != _resHAKs.end(); ++hak)
		deindexResources(*hak);

	_resHAKs.clear();
}

void Module::loadFactions() {
	// TODO: Load factions from module's 'repute.fac' file
	_factions = std::make_unique<Factions>();
}

void Module::loadRoster() {
	_roster = std::make_unique<Roster>();
}

void Module::loadJournal() {
	if (_campaignJournal) {
		// Copy the campaign journal
		_moduleJournal = std::make_unique<Journal>(*_campaignJournal);
	} else {
		// Load the module journal
		std::unique_ptr<Aurora::GFF3File> jrl;
		jrl.reset(loadOptionalGFF3("module", Aurora::kFileTypeJRL, MKTAG('J', 'R', 'L', ' ')));
		if (jrl) {
			const Aurora::GFF3Struct &top = jrl->getTopLevel();
			_moduleJournal = std::make_unique<Journal>(top);
		}
	}
}

void Module::loadCampaignJournal(const Aurora::GFF3Struct &journal) {
	_campaignJournal = std::make_unique<Journal>(journal);
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

void Module::unloadFactions() {
	_factions.reset();
}

void Module::unloadRoster() {
	_roster.reset();
}

void Module::unloadJournal() {
	_moduleJournal.reset();
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

const Common::UString &Module::getName() const {
	return NWN2::Object::getName();
}

const Common::UString &Module::getDescription() const {
	return NWN2::Object::getDescription();
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
		const Common::FileList modules(ConfigMan.getString("NWN2_moduleDir"));

		const Aurora::ERFFile mod(new Common::ReadFile(modules.findFirst(module + ".mod", true)));
		const uint32_t ifoIndex = mod.findResource("module", Aurora::kFileTypeIFO);

		const Aurora::GFF3File ifo(mod.getResource(ifoIndex), MKTAG('I', 'F', 'O', ' '));

		return ifo.getTopLevel().getString("Mod_Name");

	} catch (...) {
	}

	return "";
}

Common::UString Module::getDescription(const Common::UString &module) {
	try {
		const Common::FileList modules(ConfigMan.getString("NWN2_moduleDir"));

		const Aurora::ERFFile mod(new Common::ReadFile(modules.findFirst(module + ".mod", true)));
		const uint32_t ifoIndex = mod.findResource("module", Aurora::kFileTypeIFO);

		const Aurora::GFF3File ifo(mod.getResource(ifoIndex), MKTAG('I', 'F', 'O', ' '));

		return ifo.getTopLevel().getString("Mod_Description");

	} catch (...) {
	}

	return "";
}

} // End of namespace NWN2

} // End of namespace Engines
