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
#include "src/aurora/dlgfile.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/graphics/camera.h"
#include "src/graphics/graphics.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/model.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/console.h"
#include "src/engines/aurora/freeroamcamera.h"
#include "src/engines/aurora/satellitecamera.h"

#include "src/engines/kotor2/module.h"
#include "src/engines/kotor2/area.h"
#include "src/engines/kotor2/creature.h"
#include "src/engines/kotor2/gui/ingame/ingame.h"

namespace Engines {

namespace KotOR2 {

static const float kPCMovementSpeed = 5;

bool Module::Action::operator<(const Action &s) const {
	return timestamp < s.timestamp;
}


Module::Module(::Engines::Console &console)
		: Object(KotOR::kObjectTypeModule),
		  _console(&console),
		  _hasModule(false),
		  _running(false),
		  _currentTexturePack(-1),
		  _exit(false),
		  _entryLocationType(KotOR::kObjectTypeAll),
		  _dialog(new DialogGUI(*this)),
		  _freeCamEnabled(false),
		  _prevTimestamp(0),
		  _frameTime(0),
		  _forwardBtnPressed(false),
		  _backwardsBtnPressed(false),
		  _pcRunning(false),
		  _inDialog(false),
		  _cameraHeight(0.0f),
		  _ingame(new IngameGUI(*this, _console)) {

	loadTexturePack();
	loadSurfaceTypes();
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
                  KotOR::ObjectType entryLocationType) {

	if (isRunning()) {
		// We are currently running a module. Schedule a safe change instead

		changeModule(module, entryLocation, entryLocationType);
		return;
	}

	// We are not currently running a module. Directly load the new module
	loadModule(module, entryLocation, entryLocationType);
}

void Module::loadModule(const Common::UString &module, const Common::UString &entryLocation,
                        KotOR::ObjectType entryLocationType) {

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

const std::vector<bool> &Module::getWalkableSurfaces() const {
	return _walkableSurfaces;
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

void Module::loadSurfaceTypes() {
	_walkableSurfaces.clear();

	const Aurora::TwoDAFile &surfacematTwoDA = TwoDAReg.get2DA("surfacemat");
	for (size_t s = 0; s < surfacematTwoDA.getRowCount(); ++s) {
		const Aurora::TwoDARow &row = surfacematTwoDA.getRow(s);
		_walkableSurfaces.push_back(static_cast<bool>(row.getInt("Walk")));
	}
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
	GfxMan.pauseAnimations();

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
	_entryLocationType = KotOR::kObjectTypeAll;
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
                          KotOR::ObjectType entryLocationType) {

	_newModule = module;

	_entryLocation     = entryLocation;
	_entryLocationType = entryLocationType;
}

void Module::replaceModule() {
	if (_newModule.empty())
		return;

	_console->hide();

	const Common::UString   newModule         = _newModule;
	const Common::UString   entryLocation     = _entryLocation;
	const KotOR::ObjectType entryLocationType = _entryLocationType;

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

	if (_pc) {
		_pc->setPosition(entryX, entryY, entryZ);
		_pc->show();
	}

	_cameraHeight = _pc->getCameraHeight();

	float cameraDistance, cameraPitch, cameraHeight;
	_area->getCameraStyle(cameraDistance, cameraPitch, cameraHeight);

	SatelliteCam.setTarget(entryX, entryY, entryZ + _cameraHeight);
	SatelliteCam.setDistance(cameraDistance);
	SatelliteCam.setPitch(cameraPitch);
	SatelliteCam.setHeight(cameraHeight);
	SatelliteCam.update(0);

	_ingame->show();

	enterArea();

	_area->notifyPCMoved();

	GfxMan.resumeAnimations();

	_running = true;
	_exit    = false;
}

bool Module::getObjectLocation(const Common::UString &object, KotOR::ObjectType location,
                               float &entryX, float &entryY, float &entryZ, float &entryAngle) {

	if (object.empty())
		return false;

	Common::ScopedPtr<Aurora::NWScript::ObjectSearch> search(findObjectsByTag(object));


	KotOR::Object *kotorObject = 0;
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
	_ingame->hide();

	leaveArea();

	_running = false;
	_exit    = true;
}

void Module::clickObject(Object *object) {
	Creature *creature = ObjectContainer::toCreature(object);
	if (creature && !creature->getConversation().empty())
		startConversation(creature->getConversation(), creature);
}

void Module::enterArea() {
	_area->show();

	runScript(KotOR::kScriptModuleLoad , this, _pc.get());
	runScript(KotOR::kScriptModuleStart, this, _pc.get());
	runScript(KotOR::kScriptEnter      , this, _pc.get());

	_area->runScript(KotOR::kScriptEnter, _area.get(), _pc.get());
}

void Module::leaveArea() {
	if (_area) {
		_area->runScript(KotOR::kScriptExit, _area.get(), _pc.get());

		_area->hide();
	}

	runScript(KotOR::kScriptExit, this, _pc.get());
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

	uint32 now = SDL_GetTicks();
	_frameTime = (now - _prevTimestamp) / 1000.f;
	_prevTimestamp = now;

	handleEvents();
	handleActions();

	if (!_freeCamEnabled) {
		GfxMan.lockFrame();
		handlePCMovement();
		SatelliteCam.update(_frameTime);
		GfxMan.unlockFrame();
	}
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

		// Conversation/cutscene
		if (_inDialog) {
			_dialog->addEvent(*event);
			continue;
		}

		// PC movement
		switch (event->type) {
			case Events::kEventKeyDown:
			case Events::kEventKeyUp:
				if (event->key.keysym.scancode == SDL_SCANCODE_W) {
					_forwardBtnPressed = event->type == Events::kEventKeyDown;
				} else if (event->key.keysym.scancode == SDL_SCANCODE_S) {
					_backwardsBtnPressed = event->type == Events::kEventKeyDown;
				}
				break;
		}

		// Camera
		if (!_console->isVisible()) {
			if (_freeCamEnabled) {
				if (FreeRoamCam.handleCameraInput(*event))
					continue;
			} else if (SatelliteCam.handleCameraInput(*event))
				continue;
		}

		_area->addEvent(*event);
		_ingame->addEvent(*event);
	}

	_eventQueue.clear();

	if (_freeCamEnabled)
		CameraMan.update();

	_area->processEventQueue();
	_dialog->processEventQueue();
	_ingame->processEventQueue();

	if (_inDialog && !_dialog->isConversationActive()) {
		_dialog->hide();
		_ingame->show();
		_inDialog = false;
	}
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

void Module::handlePCMovement() {
	if (!_pc)
		return;

	bool haveMovement = false;

	if (_forwardBtnPressed || _backwardsBtnPressed) {
		float x, y, z;
		_pc->getPosition(x, y, z);
		float yaw = SatelliteCam.getYaw();
		float newX, newY;

		if (_forwardBtnPressed && !_backwardsBtnPressed) {
			_pc->setOrientation(0, 0, 1, Common::rad2deg(yaw));
			newX = x - kPCMovementSpeed * sin(yaw) * _frameTime;
			newY = y + kPCMovementSpeed * cos(yaw) * _frameTime;
			haveMovement = true;
		} else if (_backwardsBtnPressed && !_forwardBtnPressed) {
			_pc->setOrientation(0, 0, 1, 180 + Common::rad2deg(yaw));
			newX = x + kPCMovementSpeed * sin(yaw) * _frameTime;
			newY = y - kPCMovementSpeed * cos(yaw) * _frameTime;
			haveMovement = true;
		}

		if (haveMovement) {
			z = _area->evaluateElevation(newX, newY);
			if (z != FLT_MIN) {
				if (_area->walkable(glm::vec3(x, y, z + 0.1f),
				                    glm::vec3(newX, newY, z + 0.1f)))
					movePC(newX, newY, z);
			}
		}
	}

	if (haveMovement && !_pcRunning) {
		_pc->playAnimation(Common::UString("run"), false, -1);
		_pcRunning = true;
	} else if (!haveMovement && _pcRunning) {
		_pc->playDefaultAnimation();
		_pcRunning = false;
	}
}

void Module::movePC(float x, float y, float z) {
	if (!_pc)
		return;

	_pc->setPosition(x, y, z);
	movedPC();
}

void Module::movePC(const Common::UString &module, const Common::UString &object, KotOR::ObjectType type) {
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

	SatelliteCam.setTarget(x, y, z + _cameraHeight);

	if (_freeCamEnabled) {
		CameraMan.setPosition(x, y, z + _cameraHeight);
		CameraMan.update();
	}

	_area->evaluateTriggers(x, y);

	if (!_freeCamEnabled)
		_area->notifyPCMoved();
}

const Aurora::IFOFile &Module::getIFO() const {
	return _ifo;
}

const Common::UString &Module::getName() const {
	return KotOR::Object::getName();
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

void Module::toggleFreeRoamCamera() {
	_freeCamEnabled = !_freeCamEnabled;
	if (_freeCamEnabled && ConfigMan.getBool("flycamallrooms", true))
		_area->showAllRooms();
}

void Module::toggleWalkmesh() {
	_area->toggleWalkmesh();
}

void Module::toggleTriggers() {
	_area->toggleTriggers();
}

void Module::startConversation(const Common::UString &name, Aurora::NWScript::Object *owner) {
	if (_inDialog)
		return;

	Common::UString finalName(name);

	if (finalName.empty() && owner) {
		Creature *creature = ObjectContainer::toCreature(owner);
		if (creature)
			finalName = creature->getConversation();
	}

	if (finalName.empty())
		return;

	_dialog->startConversation(finalName, owner);

	if (_dialog->isConversationActive()) {
		_ingame->hide();
		_forwardBtnPressed = false;
		_backwardsBtnPressed = false;
		SatelliteCam.clearInput();
		_dialog->show();
		_inDialog = true;
	}
}

void Module::playAnimationOnActiveObject(const Common::UString &baseAnim,
                                         const Common::UString &headAnim) {
	KotOR::Object *o = _area->getActiveObject();
	if (!o)
		return;

	o->playAnimation(baseAnim, true, -1.0f);

	Creature *creature = ObjectContainer::toCreature(o);
	if (creature) {
		if (headAnim.empty())
			creature->playDefaultHeadAnimation();
		else
			creature->playHeadAnimation(headAnim, true, -1.0f, 0.5f);
	}
}

} // End of namespace KotOR2

} // End of namespace Engines
