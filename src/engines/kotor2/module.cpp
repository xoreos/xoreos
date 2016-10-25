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

#include "src/common/scopedptr.h"
#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/readfile.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"
#include "src/common/configman.h"

#include "src/aurora/types.h"
#include "src/aurora/rimfile.h"
#include "src/aurora/gff3file.h"

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
	_console(&console), _hasModule(false), _running(false),
	_currentTexturePack(-1), _exit(false), _entryLocationType(kObjectTypeAll) {

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

void Module::load(const Common::UString &module, const Common::UString &entryLocation,
                  ObjectType entryLocationType) {

	if (isRunning()) {
		// We are currently running a module. Schedule a safe change instead

		changeModule(module, entryLocation, entryLocationType);
		return;
	}

	// We are not currently running a module. Directly load the new module
	loadModule(module, entryLocation, entryLocationType);
}

void Module::loadModule(const Common::UString &module, const Common::UString &entryLocation,
                        ObjectType entryLocationType) {

	unload(false);

	_module = module;

	_entryLocation     = entryLocation;
	_entryLocationType = entryLocationType;

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
	_pc.reset(pc);
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
	_area.reset(new Area(*this, _ifo.getEntryArea()));
}

static const char * const texturePacks[3] = {
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

	_entryLocation.clear();
	_entryLocationType = kObjectTypeAll;
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
	_area.reset();
}

void Module::unloadPC() {
	_pc.reset();
}

void Module::unloadTexturePack() {
	deindexResources(_textures);
	_currentTexturePack = -1;
}

void Module::changeModule(const Common::UString &module, const Common::UString &entryLocation,
                          ObjectType entryLocationType) {

	_newModule = module;

	_entryLocation     = entryLocation;
	_entryLocationType = entryLocationType;
}

void Module::replaceModule() {
	if (_newModule.empty())
		return;

	_console->hide();

	const Common::UString newModule         = _newModule;
	const Common::UString entryLocation     = _entryLocation;
	const ObjectType      entryLocationType = _entryLocationType;

	unload(false);

	_exit = true;

	loadModule(newModule, entryLocation, entryLocationType);
	enter();
}

void Module::enter() {
	if (!_hasModule)
		throw Common::Exception("Module::enter(): Lacking a module?!?");

	if (!_pc)
		throw Common::Exception("Module::enter(): Lacking a PC?!?");

	_console->printf("Entering module \"%s\"", _name.c_str());

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	float entryX, entryY, entryZ, entryAngle;
	if (!getEntryObjectLocation(entryX, entryY, entryZ, entryAngle))
		getEntryIFOLocation(entryX, entryY, entryZ, entryAngle);

	// Roughly head position
	CameraMan.setPosition(entryX, entryY, entryZ + 1.8f);
	CameraMan.setOrientation(90.0f, 0.0f, entryAngle);
	CameraMan.update();

	enterArea();

	_running = true;
	_exit    = false;
}

bool Module::getObjectLocation(const Common::UString &object, ObjectType location,
                               float &entryX, float &entryY, float &entryZ, float &entryAngle) {

	if (object.empty())
		return false;

	Common::ScopedPtr<Aurora::NWScript::ObjectSearch> search(findObjectsByTag(object));


	KotOR2::Object *kotorObject = 0;
	while (!kotorObject && search->get()) {
		kotorObject = KotOR2::ObjectContainer::toObject(search->next());
		if (!kotorObject || !(kotorObject->getType() & location))
			kotorObject = 0;
	}

	if (!kotorObject)
		return false;

	// TODO: Entry orientation

	kotorObject->getPosition(entryX, entryY, entryZ);
	entryAngle = 0.0f;

	return true;
}

bool Module::getEntryObjectLocation(float &entryX, float &entryY, float &entryZ, float &entryAngle) {
	return getObjectLocation(_entryLocation, _entryLocationType, entryX, entryY, entryZ, entryAngle);
}

void Module::getEntryIFOLocation(float &entryX, float &entryY, float &entryZ, float &entryAngle) {
	_ifo.getEntryPosition(entryX, entryY, entryZ);

	float entryDirX, entryDirY;
	_ifo.getEntryDirection(entryDirX, entryDirY);

	entryAngle = -Common::rad2deg(atan2(entryDirX, entryDirY));
}

void Module::leave() {
	leaveArea();

	_running = false;
	_exit    = true;
}

void Module::enterArea() {
	_area->show();

	runScript(kScriptModuleLoad , this, _pc.get());
	runScript(kScriptModuleStart, this, _pc.get());
	runScript(kScriptEnter      , this, _pc.get());

	_area->runScript(kScriptEnter, _area.get(), _pc.get());
}

void Module::leaveArea() {
	if (_area) {
		_area->runScript(kScriptExit, _area.get(), _pc.get());

		_area->hide();
	}

	runScript(kScriptExit, this, _pc.get());
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

void Module::movePC(float x, float y, float z) {
	if (!_pc)
		return;

	_pc->setPosition(x, y, z);
	movedPC();
}

void Module::movePC(const Common::UString &module, const Common::UString &object, ObjectType type) {
	if (module.empty() || (module == _module)) {
		float x, y, z, angle;
		if (getObjectLocation(object, type, x, y, z, angle))
			movePC(x, y, z);

		return;
	}

	load(module, object, type);
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

const Aurora::IFOFile &Module::getIFO() const {
	return _ifo;
}

const Common::UString &Module::getName() const {
	return KotOR2::Object::getName();
}

Area *Module::getCurrentArea() {
	return _area.get();
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

Common::UString Module::getName(const Common::UString &module) {
	/* Return the localized name of the first (and only) area of the module,
	 * which is the closest thing to the name of the module.
	 *
	 * To do that, if looks through the module directory for a matching RIM file
	 * (case-insensitively) and opens it without indexing into the ResourceManager.
	 * It then opens the module.ifo, grabs the name of the area, opens its ARE file
	 * and returns the localized "Name" field.
	 *
	 * If there's any error while doing all this, an empty string is returned.
	 */

	try {
		const Common::FileList modules(ConfigMan.getString("KOTOR2_moduleDir"));

		const Aurora::RIMFile rim(new Common::ReadFile(modules.findFirst(module + ".rim", true)));
		const uint32 ifoIndex = rim.findResource("module", Aurora::kFileTypeIFO);

		const Aurora::GFF3File ifo(rim.getResource(ifoIndex), MKTAG('I', 'F', 'O', ' '));

		const Aurora::GFF3List &areas = ifo.getTopLevel().getList("Mod_Area_list");
		if (areas.empty())
			return "";

		const uint32 areIndex = rim.findResource((*areas.begin())->getString("Area_Name"), Aurora::kFileTypeARE);

		const Aurora::GFF3File are(rim.getResource(areIndex), MKTAG('A', 'R', 'E', ' '));

		return are.getTopLevel().getString("Name");

	} catch (...) {
	}

	return "";
}

} // End of namespace KotOR2

} // End of namespace Engines
