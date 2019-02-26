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
 *  The context needed to run a module in KotOR games.
 */

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

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/textureman.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/console.h"
#include "src/engines/aurora/freeroamcamera.h"
#include "src/engines/aurora/satellitecamera.h"

#include "src/engines/kotorbase/creature.h"
#include "src/engines/kotorbase/placeable.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/area.h"

#include "src/engines/kotorbase/gui/partyselection.h"

#include "src/engines/kotor/gui/dialog.h"

#include "src/engines/kotor/gui/ingame/ingame.h"

#include "src/engines/kotor/gui/loadscreen/loadscreen.h"

#include "src/engines/kotor/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotORBase {

bool Module::Action::operator<(const Action &s) const {
	return timestamp < s.timestamp;
}


Module::Module(::Engines::Console &console) :
		Object(kObjectTypeModule),
		_console(&console),
		_hasModule(false),
		_running(false),
		_pc(0),
		_currentTexturePack(-1),
		_exit(false),
		_entryLocationType(kObjectTypeAll),
		_fade(new Graphics::Aurora::FadeQuad()),
		_freeCamEnabled(false),
		_prevTimestamp(0),
		_frameTime(0),
		_inDialog(false),
		_partyLeaderController(this),
		_partyController(this),
		_runScriptVar(-1),
		_soloMode(false),
		_lastHeartbeatTimestamp(0) {

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
	_ingame->hide();

	Common::ScopedPtr<KotORBase::LoadScreen> loadScreen(createLoadScreen(module));
	loadScreen->show();

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

	int northAxis;
	float mapPt1X, mapPt1Y, mapPt2X, mapPt2Y;
	float worldPt1X, worldPt1Y, worldPt2X, worldPt2Y;

	northAxis = _area->getNorthAxis();
	_area->getMapPoint1(mapPt1X, mapPt1Y);
	_area->getMapPoint2(mapPt2X, mapPt2Y);
	_area->getWorldPoint1(worldPt1X, worldPt1Y);
	_area->getWorldPoint2(worldPt2X, worldPt2Y);

	Common::UString mapId;
	if (_module.contains('_'))
		mapId = _module.substr(++_module.findFirst("_"), _module.end());
	else
		mapId = _module.substr(_module.getPosition(3), _module.end());
	_ingame->setMinimap(mapId, northAxis,
	                    worldPt1X, worldPt1Y, worldPt2X, worldPt2Y,
	                    mapPt1X, mapPt1Y, mapPt2X, mapPt2Y);

	_newModule.clear();

	_hasModule = true;

	loadScreen->hide();

	_ingame->show();
}

void Module::usePC(const CharacterGenerationInfo &info) {
	_chargenInfo.reset(createCharGenInfo(info));
}

Creature *Module::getPC() {
	return _pc;
}

const std::vector<bool> &Module::getWalkableSurfaces() const {
	return _walkableSurfaces;
}

Graphics::Aurora::FadeQuad &Module::getFadeQuad() {
	return *_fade;
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
	loadPC();
	loadParty();
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

void Module::loadPC() {
	_pc = createCreature();

	if (_chargenInfo)
		_pc->createPC(*_chargenInfo.get());
	else
		_pc->createFakePC();

	_area->addCreature(_pc);
}

void Module::loadParty() {
	std::vector<int> partyMembers = _partyController.getPartyMembers();

	if (partyMembers.empty()) {
		_partyController.addPartyMember(-1, _pc);
	} else {
		_partyController.clearCurrentParty();

		for (auto npc : partyMembers) {
			if (npc == -1) {
				_partyController.addPartyMember(-1, _pc);
				continue;
			}

			Common::UString templ = _partyController.getAvailableNPCTemplate(npc);

			// Special case: Trask exists only on the Endar Spire
			if (templ == "end_trask")
				continue;

			Creature *creature = createCreature(templ);
			addPartyMember(npc, creature);
		}
	}

	updateCurrentPartyGUI();
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
	const int level = ConfigMan.getInt("texturepack");
	if (_currentTexturePack == level)
		// Nothing to do
		return;

	const int oldTexturePack = _currentTexturePack;

	unloadTexturePack();

	status("Loading texture pack %d", level);
	indexOptionalArchive(texturePacks[level], 400, &_textures);

	// If we already had a texture pack loaded, reload all textures
	if (oldTexturePack != -1)
		TextureMan.reloadAll();

	_currentTexturePack = level;
}

void Module::unload(bool completeUnload) {
	GfxMan.pauseAnimations();

	leaveArea();
	unloadArea();

	if (completeUnload) {
		unloadTexturePack();

		_globalNumbers.clear();
		_globalBooleans.clear();

		_partyController.clearCurrentParty();
		_partyController.clearAvailableParty();
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

	_console->printf("Entering module \"%s\"", _name.c_str());

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	float entryX, entryY, entryZ, _;
	if (!getEntryObjectLocation(entryX, entryY, entryZ, _))
		getEntryIFOLocation(entryX, entryY, entryZ, _);

	moveParty(entryX, entryY, entryZ);
	enterArea();

	GfxMan.resumeAnimations();

	_running = true;
	_exit    = false;

	_ingame->show();
}

bool Module::getObjectLocation(const Common::UString &object, ObjectType location,
                               float &entryX, float &entryY, float &entryZ, float &entryAngle) {

	if (object.empty())
		return false;

	Common::ScopedPtr<Aurora::NWScript::ObjectSearch> search(findObjectsByTag(object));


	Object *kotorObject = 0;
	while (!kotorObject && search->get()) {
		kotorObject = ObjectContainer::toObject(search->next());
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

void Module::enterArea() {
	GfxMan.lockFrame();

	_area->show();

	runScript(kScriptModuleLoad , this, _pc);
	runScript(kScriptModuleStart, this, _pc);
	runScript(kScriptEnter      , this, _pc);

	GfxMan.unlockFrame();

	_area->runScript(kScriptEnter, _area.get(), _pc);
}

void Module::leaveArea() {
	if (_area) {
		_area->runScript(kScriptExit, _area.get(), _pc);

		_area->hide();

		runScript(kScriptExit, this, _pc);
	}
}

void Module::clickObject(Object *object) {
	Creature *creature = ObjectContainer::toCreature(object);
	if (creature && !creature->getConversation().empty()) {
		startConversation(creature->getConversation(), creature);
		return;
	}

	Situated *situated = ObjectContainer::toSituated(object);
	if (situated && !situated->getConversation().empty()) {
		startConversation(situated->getConversation(), situated);
		return;
	}

	Placeable *placeable = ObjectContainer::toPlaceable(object);
	if (placeable) {
		if (placeable->hasInventory()) {
			stopCameraMovement();
			_partyLeaderController.stopMovement();

			_ingame->hideSelection();
			_ingame->showContainer(placeable->getInventory());
			resetSelection();

			placeable->close(_pc);
			placeable->runScript(kScriptDisturbed, placeable, _pc);

			_prevTimestamp = EventMan.getTimestamp();
		}
	}
}

void Module::enterObject(Object *object) {
	_ingame->showSelection(object);
}

void Module::leaveObject(Object *UNUSED(object)) {
	_ingame->hideSelection();
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
	handleHeartbeat();

	GfxMan.lockFrame();

	_area->processCreaturesActions(_frameTime);

	if (!_freeCamEnabled) {
		SatelliteCam.update(_frameTime);
		_partyLeaderController.processMovement(_frameTime);
		updateMinimap();
	}

	updateSoundListener();

	if (!_inDialog)
		_ingame->updateSelection();

	GfxMan.unlockFrame();
}

void Module::handleEvents() {
	for (EventQueue::const_iterator event = _eventQueue.begin(); event != _eventQueue.end(); ++event) {
		// Handle console
		if (_console->isVisible()) {
			_console->processEvent(*event);
			continue;
		}

		// Conversation/cutscene
		if (_inDialog) {
			_dialog->addEvent(*event);
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

		_partyLeaderController.handleEvent(*event);

		// Camera
		if (!_console->isVisible()) {
			if (_freeCamEnabled) {
				if (FreeRoamCam.handleCameraInput(*event))
					continue;
			}
			else if (SatelliteCam.handleCameraInput(*event))
				continue;
		}

		_area->addEvent(*event);
		_ingame->addEvent(*event);
	}

	_eventQueue.clear();

	if (_freeCamEnabled)
		CameraMan.update();

	if (_inDialog) {
		_dialog->processEventQueue();
		if (!_dialog->isConversationActive()) {
			_dialog->hide();
			_ingame->show();
			_inDialog = false;
			resetSelection();
		}
	} else {
		_area->processEventQueue();
		_ingame->processEventQueue();
	}
}

void Module::updateMinimap() {
	float x, y, _;
	_partyController.getPartyLeader()->getPosition(x, y, _);

	_ingame->setPosition(x, y);
	_ingame->setRotation(Common::rad2deg(SatelliteCam.getYaw()));
}

void Module::updateSoundListener() {
	const float *position = CameraMan.getPosition();
	SoundMan.setListenerPosition(position[0], position[1], position[2]);
	const float *orientation = CameraMan.getOrientation();
	SoundMan.setListenerOrientation(orientation[0], orientation[1], orientation[2], 0.0f, 1.0f, 0.0f);
}

void Module::resetSelection() {
	GfxMan.lockFrame();

	_ingame->showSelection(_area->getActiveObject());
	NotificationMan.cameraMoved();

	GfxMan.unlockFrame();
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

void Module::handleHeartbeat() {
	const int kHeartbeatInterval = 6000; // ms

	uint32 now = EventMan.getTimestamp();
	if (now >= _lastHeartbeatTimestamp + kHeartbeatInterval) {
		_runScriptVar = 2001;
		_partyController.raiseHeartbeatEvent();
		_lastHeartbeatTimestamp = now;
	}
}

void Module::moveParty(float x, float y, float z) {
	int partySize = static_cast<int>(_partyController.getPartyMemberCount());
	for (int i = 0; i < partySize; ++i) {
		Creature *creature = _partyController.getPartyMemberByIndex(i).second;
		creature->setPosition(x, y, z);

		if (i == 0)
			movedPartyLeader();
		else
			_area->notifyObjectMoved(*creature);
	}
}

void Module::moveParty(const Common::UString &module, const Common::UString &object, ObjectType type) {
	if (module.empty() || (module == _module)) {
		float x, y, z, angle;
		if (getObjectLocation(object, type, x, y, z, angle))
			moveParty(x, y, z);

		return;
	}

	load(module, object, type);
}

void Module::movedPartyLeader() {
	float x, y, _;
	_partyController.getPartyLeader()->getPosition(x, y, _);

	_area->evaluateTriggers(x, y);
	_area->notifyPartyLeaderMoved();

	setupSatelliteCamera();
}

Creature *Module::getPartyLeader() const {
	return _partyController.getPartyLeader();
}

Creature *Module::getPartyMemberByIndex(int index) const {
	return _partyController.getPartyMemberByIndex(index).second;
}

bool Module::isObjectPartyMember(Creature *object) const {
	return _partyController.isObjectPartyMember(object);
}

bool Module::isAvailableCreature(int npc) const {
	return _partyController.isAvailableCreature(npc);
}

bool Module::isSoloMode() const {
	return _soloMode;
}

void Module::setPartyLeader(int npc) {
	_partyController.setPartyLeader(npc);
	onPartyLeaderChanged();
}

void Module::setPartyLeaderByIndex(int index) {
	if (index == 0)
		return;

	_partyController.setPartyLeaderByIndex(index);
	onPartyLeaderChanged();
}

void Module::showPartySelectionGUI(int forceNPC1, int forceNPC2) {
	if (_inDialog) {
		_dialog->hide();
	} else {
		_ingame->hide();
		_ingame->hideSelection();
	}

	PartyConfiguration config;

	for (int i = 0; i < 10; ++i) {
		if (_partyController.isAvailableCreature(i))
			config.slotTemplate[i] = _partyController.getAvailableNPCTemplate(i);
	}

	config.forceNPC1 = forceNPC1;
	config.forceNPC2 = forceNPC2;
	config.canCancel = false;

	_partySelection->loadConfiguration(config);

	_partySelection->show();
	_partySelection->run();
	_partySelection->hide();

	if (_inDialog) {
		_dialog->show();
	} else {
		_ingame->show();
		resetSelection();
	}

	int npc1 = config.forceNPC1;
	int npc2 = config.forceNPC2;

	if ((npc1 == -1) || (npc2 == -1)) {
		for (int i = 0; i < 10; ++i) {
			if (config.slotSelected[i]) {
				if (npc1 == -1) {
					npc1 = i;
				} else {
					npc2 = i;
					break;
				}
			}
		}
	}

	int partySize = static_cast<int>(_partyController.getPartyMemberCount());
	for (int i = 0; i < partySize; ++i) {
		auto partyMember = _partyController.getPartyMemberByIndex(i);
		if (partyMember.first != -1)
			_area->removeObject(partyMember.second);
	}

	_partyController.clearCurrentParty();
	_partyController.addPartyMember(-1, _pc);

	if (npc1 != -1) {
		Creature *creature = createCreature(config.slotTemplate[npc1]);
		addPartyMember(npc1, creature);
	}

	if (npc2 != -1) {
		Creature *creature = createCreature(config.slotTemplate[npc2]);
		addPartyMember(npc2, creature);
	}

	updateCurrentPartyGUI();
}

void Module::addAvailableNPCByTemplate(int npc, const Common::UString &templ) {
	_partyController.addAvailableNPCByTemplate(npc, templ);
}

void Module::setReturnStrref(uint32 id) {
	_ingame->setReturnStrref(id);
}

void Module::setReturnQueryStrref(uint32 id) {
	_ingame->setReturnQueryStrref(id);
}

void Module::setReturnEnabled(bool enabled) {
	_ingame->setReturnEnabled(enabled);
}

void Module::setGlobalBoolean(const Common::UString &id, bool value) {
	_globalBooleans[id] = value;
}

bool Module::getGlobalBoolean(const Common::UString &id) const {
	std::map<Common::UString, bool>::const_iterator iter = _globalBooleans.find(id);
	if (iter != _globalBooleans.end())
		return iter->second;
	else
		return false;
}

void Module::setGlobalNumber(const Common::UString &id, int value) {
	_globalNumbers[id] = value;
}

int Module::getGlobalNumber(const Common::UString &id) const {
	std::map<Common::UString, int>::const_iterator iter = _globalNumbers.find(id);
	if (iter != _globalNumbers.end())
		return iter->second;
	else
		return 0;
}

const Aurora::IFOFile &Module::getIFO() const {
	return _ifo;
}

const Common::UString &Module::getName() const {
	return Object::getName();
}

Area *Module::getCurrentArea() {
	return _area.get();
}

int Module::getRunScriptVar() const {
	return _runScriptVar;
}

void Module::setRunScriptVar(int runScriptVar) {
	_runScriptVar = runScriptVar;
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

Common::UString Module::getName(const Common::UString &module, const Common::UString &moduleDirOptionName) {
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
		const Common::FileList modules(ConfigMan.getString(moduleDirOptionName));

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
	if (_freeCamEnabled && ConfigMan.getBool("flycamallrooms"))
		_area->showAllRooms();
}

void Module::toggleWalkmesh() {
	_area->toggleWalkmesh();
}

void Module::toggleTriggers() {
	_area->toggleTriggers();
}

void Module::loadSavedGame(SavedGame *save) {
	try {
		Common::ScopedPtr<CharacterGenerationInfo> info(save->createCharGenInfo());
		usePC(*info.get());
		load(save->getModuleName());
	} catch (...) {
		Common::exceptionDispatcherWarning();
	}
}

bool Module::isConversationActive() const {
	return _inDialog;
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
		stopCameraMovement();
		_partyLeaderController.stopMovement();

		_ingame->hide();
		_ingame->hideSelection();
		_dialog->show();
		_inDialog = true;
	}
}

void Module::playAnimationOnActiveObject(const Common::UString &baseAnim,
                                         const Common::UString &headAnim) {
	Object *o = _area->getActiveObject();
	if (!o)
		return;

	o->playAnimation(baseAnim, true, -1.0f);

	Creature *creature = ObjectContainer::toCreature(o);
	if (creature) {
		if (headAnim.empty())
			creature->playDefaultHeadAnimation();
		else
			creature->playHeadAnimation(headAnim, true, -1.0f, 0.25f);
	}
}

void Module::addItemToActiveObject(const Common::UString &item, int count) {
	Inventory *inv = 0;

	Object *o = _area->getActiveObject();
	if (!o) {
		if (_pc)
			inv = &_pc->getInventory();
	} else {
		Placeable *placeable = ObjectContainer::toPlaceable(o);
		if (placeable && placeable->hasInventory())
			inv = &placeable->getInventory();

		if (!inv) {
			Creature *creature = ObjectContainer::toCreature(o);
			if (creature)
				inv = &creature->getInventory();
		}
	}

	if (!inv)
		return;

	if (count > 0)
		inv->addItem(item, count);
	else if (count < 0)
		inv->removeItem(item, -count);
}

void Module::setupSatelliteCamera() {
	float x, y, z, hookHeight, distance, pitch, height;
	Creature *partyLeader = _partyController.getPartyLeader();
	partyLeader->getPosition(x, y, z);
	hookHeight = partyLeader->getCameraHeight();
	_area->getCameraStyle(distance, pitch, height);

	SatelliteCam.setTarget(x, y, z + hookHeight);
	SatelliteCam.setDistance(distance);
	SatelliteCam.setPitch(pitch);
	SatelliteCam.setHeight(height);
	SatelliteCam.update(0.0f);
}

void Module::stopCameraMovement() {
	SatelliteCam.clearInput();
}

void Module::addPartyMember(int npc, Creature *creature) {
	Creature *partyLeader = _partyController.getPartyLeader();

	float x, y, z;
	partyLeader->getPosition(x, y, z);
	creature->setPosition(x, y, z);

	_area->addCreature(creature);
	_partyController.addPartyMember(npc, creature);

	creature->show();
}

void Module::onPartyLeaderChanged() {
	setupSatelliteCamera();
	updateCurrentPartyGUI();
}

void Module::updateCurrentPartyGUI() {
	size_t partySize = _partyController.getPartyMemberCount();

	if (partySize > 0)
		_ingame->setPartyLeader(_partyController.getPartyLeader());

	if (partySize > 1)
		_ingame->setPartyMember1(_partyController.getPartyMemberByIndex(1).second);

	if (partySize > 2)
		_ingame->setPartyMember2(_partyController.getPartyMemberByIndex(2).second);
}

} // End of namespace KotORBase

} // End of namespace Engines
