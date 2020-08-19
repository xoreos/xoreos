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
 *  The context needed to run a Neverwinter Nights module.
 */

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/filepath.h"
#include "src/common/readfile.h"
#include "src/common/md5.h"

#include "src/events/events.h"

#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/erffile.h"
#include "src/aurora/resman.h"

#include "src/graphics/camera.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/tokenman.h"
#include "src/engines/aurora/console.h"
#include "src/engines/aurora/flycamera.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/version.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/game.h"
#include "src/engines/nwn/area.h"
#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/gui/ingame/ingame.h"

struct GenderToken {
	const char *token;
	uint32_t male;
	uint32_t female;
};

static const GenderToken kGenderTokens[] = {
	{"<Male/Female>"    ,  156,  157},
	{"<male/female>"    , 4924, 4925},
	{"<Boy/Girl>"       , 4860, 4861},
	{"<boy/girl>"       , 4862, 4863},
	{"<Brother/Sister>" , 4864, 4865},
	{"<brother/sister>" , 4866, 4867},
	{"<He/She>"         , 4869, 4870},
	{"<he/she>"         , 4871, 4872},
	{"<Him/Her>"        , 4873, 4874},
	{"<him/her>"        , 4875, 4876},
	{"<His/Her>"        , 4877, 4874},
	{"<his/her>"        , 4878, 4876},
	{"<His/Hers>"       , 4877, 4879},
	{"<his/hers>"       , 4878, 4880},
	{"<Lad/Lass>"       , 4881, 4882},
	{"<lad/lass>"       , 4883, 4884},
	{"<Lord/Lady>"      , 4885, 4886},
	{"<lord/lady>"      , 4887, 4888},
	{"<Man/Woman>"      , 4926, 4927},
	{"<man/woman>"      , 4928, 4929},
	{"<Master/Mistress>", 4930, 4931},
	{"<master/mistress>", 4932, 4933},
	{"<Mister/Missus>"  , 4934, 4935},
	{"<mister/missus>"  , 4936, 4937},
	{"<Sir/Madam>"      , 4939, 4940},
	{"<sir/madam>"      , 4941, 4942},
	{"<bitch/bastard>"  , 1757, 1739}
};

namespace Engines {

namespace NWN {

bool Module::Action::operator<(const Action &s) const {
	return timestamp < s.timestamp;
}


Module::Module(::Engines::Console &console, const Version &gameVersion) : Object(kObjectTypeModule),
	_console(&console), _gameVersion(&gameVersion), _hasModule(false),
	_running(false), _currentTexturePack(-1), _exit(false), _currentArea(0) {

	_ingameGUI = std::make_unique<IngameGUI>(*this, _console);
}

Module::~Module() {
	try {
		clear();
	} catch (...) {
	}
}

const Version &Module::getGameVersion() const {
	return *_gameVersion;
}

void Module::clear() {
	unload();
}

void Module::load(const Common::UString &module) {
	if (isRunning()) {
		// We are currently running a module. Schedule a safe change instead

		_ingameGUI->abortMain();
		changeModule(module);
		return;
	}

	// We are not currently running a module. Directly load the new module
	loadModule(module);
}

void Module::preparePremiumModule(const Common::UString &module) {
	if (!Game::isPremiumModule(module))
		return;

	try {
		/* If this is a premium module, we need to calculate the MD5 sum of this
		 * module file, and then load the main HAK for this module, using the
		 * MD5 has part of the decryption password. This has to be done before
		 * loading the IFO, because the HAK comes with the proper IFO. The one
		 * in the module is a stub that exists the game, assuming you haven't
		 * purchased the premium module in question. */

		Common::ReadFile mod(ResMan.findResourceFile(module));

		std::vector<byte> md5;
		Common::hashMD5(mod, md5);

		indexMandatoryArchive(Common::FilePath::changeExtension(module, ".hak"), 1001, md5, _resHAKs);
	} catch (...) {
	}
}

void Module::loadModule(const Common::UString &module) {
	unload(false);

	if (module.empty())
		throw Common::Exception("Tried to load an empty module");

	try {
		indexMandatoryArchive(module, 1000, &_resModule);
		preparePremiumModule(module);

		_ifo.load(true);

		if (_ifo.isSave())
			throw Common::Exception("This is a save");

		checkXPs();
		checkHAKs();
		loadSurfaceTypes();

		_tag  = _ifo.getTag();
		_name = _ifo.getName().getString();

		readScripts(*_ifo.getGFF());

	} catch (Common::Exception &e) {
		e.add("Can't load module \"%s\"", module.c_str());
		throw e;
	}

	_newModule.clear();

	_hasModule = true;
}

void Module::checkXPs() {
	uint16_t hasXP = 0;

	hasXP |= ConfigMan.getBool("NWN_hasXP1") ? 1 : 0;
	hasXP |= ConfigMan.getBool("NWN_hasXP2") ? 2 : 0;
	hasXP |= ConfigMan.getBool("NWN_hasXP3") ? 4 : 0;

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

void Module::setPCTokens() {
	TokenMan.set("<FullName>" , _pc->getName());
	TokenMan.set("<FirstName>", _pc->getFirstName());
	TokenMan.set("<LastName>" , _pc->getLastName());

	TokenMan.set("<Race>" , _pc->getConvRace());
	TokenMan.set("<race>" , _pc->getConvrace());
	TokenMan.set("<Races>", _pc->getConvRaces());

	TokenMan.set("<Subrace>", _pc->getSubRace());

	TokenMan.set("<Class>"  , _pc->getConvClass());
	TokenMan.set("<class>"  , _pc->getConvclass());
	TokenMan.set("<Classes>", _pc->getConvClasses());

	TokenMan.set("<Deity>", _pc->getDeity());

	for (size_t i = 0; i < ARRAYSIZE(kGenderTokens); i++) {
		const uint32_t strRef = _pc->isFemale() ? kGenderTokens[i].female : kGenderTokens[i].male;

		TokenMan.set(kGenderTokens[i].token, TalkMan.getString(strRef));
	}

	// TODO: <Level>
	// TODO: <Alignment>, <alignment>
	// TODO: <Good/Evil>, <good/evil>
	// TODO: <Lawful/Chaotic>, <lawful/chaotic>, <Law/Chaos>, <law/chaos>,
	// TODO: <GameTime>. <GameYear>, <Day/Night>, <day/night>, <QuarterDay>, <quarterday>
}

void Module::removePCTokens() {
	TokenMan.remove("<FullName>");
	TokenMan.remove("<FirstName>");
	TokenMan.remove("<LastName>");

	TokenMan.remove("<Race>");
	TokenMan.remove("<race>");
	TokenMan.remove("<Races>");

	TokenMan.remove("<Subrace>");

	TokenMan.remove("<Class>");
	TokenMan.remove("<class>");
	TokenMan.remove("<Classes>");

	TokenMan.remove("<Deity>");

	for (size_t i = 0; i < ARRAYSIZE(kGenderTokens); i++)
		TokenMan.remove(kGenderTokens[i].token);
}

void Module::usePC(const Common::UString &bic, bool local) {
	unloadPC();

	if (bic.empty())
		throw Common::Exception("Tried to load an empty PC");

	try {
		_pc = std::make_unique<Creature>(bic, local);
	} catch (Common::Exception &e) {
		e.add("Can't load PC \"%s\"", bic.c_str());
		throw e;
	}

	setPCTokens();
	LangMan.setCurrentGender(_pc->isFemale() ? Aurora::kLanguageGenderFemale : Aurora::kLanguageGenderMale);

	addObject(*_pc);
}

void Module::usePC(Creature *creature) {
	unloadPC();

	_pc.reset(creature);

	setPCTokens();
	LangMan.setCurrentGender(_pc->isFemale() ? Aurora::kLanguageGenderFemale : Aurora::kLanguageGenderMale);

	addObject(*_pc);
}

Creature *Module::getPC() {
	return _pc.get();
}

void Module::changeModule(const Common::UString &module) {
	_newModule = module;
}

const std::vector<bool> &Module::getWalkableSurfaces() const {
	return _walkableSurfaces;
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
	if (!_hasModule)
		throw Common::Exception("Module::enter(): Lacking a module?!?");

	if (!_pc)
		throw Common::Exception("Module::enter(): Lacking a PC?!?");

	_pc->clearVariables();

	loadTexturePack();

	_console->printf("Entering module \"%s\" with character \"%s\"",
			_ifo.getName().getString().c_str(), _pc->getName().c_str());

	_ingameGUI->updatePartyMember(0, *_pc);

	try {

		loadTLK();
		loadHAKs();
		loadAreas();

	} catch (Common::Exception &e) {
		e.add("Can't initialize module \"%s\"", _ifo.getName().getString().c_str());
		throw e;
	}

	float entryX, entryY, entryZ, entryDirX, entryDirY;
	_ifo.getEntryPosition(entryX, entryY, entryZ);
	_ifo.getEntryDirection(entryDirX, entryDirY);

	const float entryAngle = -Common::rad2deg(atan2(entryDirX, entryDirY));

	_pc->setPosition(entryX, entryY, entryZ);
	_pc->setOrientation(0.0f, 0.0f, 1.0f, entryAngle);

	_pc->loadModel();

	_running = true;
	_exit    = false;

	runScript(kScriptModuleLoad , this, _pc.get());
	runScript(kScriptModuleStart, this, _pc.get());
	runScript(kScriptEnter      , this, _pc.get());

	// The entry scripts might have already determined that we should quit
	if (_exit)
		return;

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	_newArea = _ifo.getEntryArea();

	CameraMan.reset();

	// Roughly head position
	CameraMan.setPosition(entryX, entryY, entryZ + 1.8f);
	CameraMan.setOrientation(90.0f, 0.0f, entryAngle);
	CameraMan.update();

	_ingameGUI->show();

	GfxMan.resumeAnimations();
}

void Module::leave() {
	_ingameGUI->stopConversation();
	_ingameGUI->hide();

	_running = false;
	_exit    = true;
}

void Module::enterArea() {
	if (_currentArea && (_currentArea->getResRef() == _newArea))
		return;

	_ingameGUI->stopConversation();

	if (_currentArea) {
		_pc->hide();

		_currentArea->runScript(kScriptExit, _currentArea, _pc.get());
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

	EventMan.flushEvents();

	_ingameGUI->setArea(_currentArea->getName());

	_pc->setArea(_currentArea);

	_currentArea->runScript(kScriptEnter, _currentArea, _pc.get());

	_console->printf("Entering area \"%s\"", _currentArea->getResRef().c_str());
}

void Module::exit() {
	_ingameGUI->abortMain();

	_exit = true;
}

bool Module::isLoaded() const {
	return _hasModule && _pc;
}

bool Module::isRunning() const {
	return !EventMan.quitRequested() && _running && !_exit && !_newArea.empty();
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

	_ingameGUI->updatePartyMember(0, *_pc);
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
				// But only if we're not in a conversation, where ESC should abort that
				if (!_ingameGUI->hasRunningConversation()) {
					showMenu();
					continue;
				}
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

		_ingameGUI->addEvent(*event);
		_currentArea->addEvent(*event);
	}

	_eventQueue.clear();

	CameraMan.update();

	_currentArea->processEventQueue();
	_ingameGUI->processEventQueue();
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

void Module::unload(bool completeUnload) {
	GfxMan.pauseAnimations();

	unloadAreas();
	unloadHAKs();
	unloadTLK();
	unloadModule();

	if (!completeUnload)
		return;

	unloadPC();
	unloadTexturePack();
}

void Module::unloadModule() {
	runScript(kScriptExit, this, _pc.get());
	handleActions();

	_eventQueue.clear();
	_delayedActions.clear();

	TwoDAReg.clear();

	clearVariables();
	clearScripts();

	_tag.clear();

	_ifo.unload();

	deindexResources(_resModule);

	_newModule.clear();
	_hasModule = false;
}

void Module::unloadPC() {
	if (!_pc)
		return;

	removeObject(*_pc);

	removePCTokens();

	_pc.reset();
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

	for (size_t i = 0; i < haks.size(); i++)
		indexMandatoryArchive(haks[i] + ".hak", 1002 + i, _resHAKs);
}

void Module::unloadHAKs() {
	deindexResources(_resHAKs);
}

static const char * const texturePacks[4][4] = {
	{ "textures_tpc.erf", "tiles_tpc.erf", "xp1_tex_tpc.erf", "xp2_tex_tpc.erf" }, // Worst
	{ "textures_tpa.erf", "tiles_tpc.erf", "xp1_tex_tpc.erf", "xp2_tex_tpc.erf" }, // Bad
	{ "textures_tpa.erf", "tiles_tpb.erf", "xp1_tex_tpb.erf", "xp2_tex_tpb.erf" }, // Okay
	{ "textures_tpa.erf", "tiles_tpa.erf", "xp1_tex_tpa.erf", "xp2_tex_tpa.erf" }  // Best
};

void Module::loadTexturePack() {
	int level = ConfigMan.getInt("texturepack", 1);
	if (_currentTexturePack == level)
		// Nothing to do
		return;

	const int oldTexturePack = _currentTexturePack;

	unloadTexturePack();

	status("Loading texture pack %d", level);
	indexMandatoryArchive(texturePacks[level][0], 400, &_resTP[0]);
	indexMandatoryArchive(texturePacks[level][1], 401, &_resTP[1]);
	indexOptionalArchive (texturePacks[level][2], 402, &_resTP[2]);
	indexOptionalArchive (texturePacks[level][3], 403, &_resTP[3]);

	// If we already had a texture pack loaded, reload all textures
	if (oldTexturePack != -1)
		TextureMan.reloadAll();

	_currentTexturePack = level;
}

void Module::unloadTexturePack() {
	for (int i = 0; i < 4; i++)
		deindexResources(_resTP[i]);

	_currentTexturePack = -1;
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

void Module::loadSurfaceTypes() {
	_walkableSurfaces.clear();

	const Aurora::TwoDAFile &surfacematTwoDA = TwoDAReg.get2DA("surfacemat");
	for (uint32_t s = 0; s < surfacematTwoDA.getRowCount(); ++s) {
		const Aurora::TwoDARow &row = surfacematTwoDA.getRow(s);
		_walkableSurfaces.push_back(static_cast<bool>(row.getInt("Walk")));
	}
}

void Module::unloadAreas() {
	_ingameGUI->stopConversation();

	_areas.clear();
	_newArea.clear();

	_currentArea = 0;
}

void Module::showMenu() {
	_currentArea->removeFocus();

	if (_ingameGUI->showMain() == 2) {
		_exit = true;
		return;
	}

	// In case we changed the texture pack settings, reload it
	loadTexturePack();
}

bool Module::startConversation(const Common::UString &conv, Creature &pc,
                               NWN::Object &obj, bool playHello) {

	return _ingameGUI->startConversation(conv, pc, obj, playHello);
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
	if (_pc->getArea()) {
		_ingameGUI->abortMain();

		_newArea = _pc->getArea()->getResRef();
	}
}

const Aurora::IFOFile &Module::getIFO() const {
	return _ifo;
}

Area *Module::getCurrentArea() {
	return _currentArea;
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

Common::UString Module::getDescriptionExtra(Common::UString module) {
	if (!Common::FilePath::getExtension(module).equalsIgnoreCase(".mod"))
		module += ".mod";

	try {
		Common::UString moduleDir = ConfigMan.getString("NWN_extraModuleDir");
		Common::UString modFile   = module;

		return Aurora::ERFFile::getDescription(moduleDir + "/" + modFile).getString();
	} catch (...) {
	}

	return "";
}

Common::UString Module::getDescriptionCampaign(Common::UString module) {
	if (!Common::FilePath::getExtension(module).equalsIgnoreCase(".nwm"))
		module += ".nwm";

	try {
		Common::UString moduleDir = ConfigMan.getString("NWN_campaignDir");
		Common::UString modFile   = module;

		return Aurora::ERFFile::getDescription(moduleDir + "/" + modFile).getString();
	} catch (...) {
	}

	return "";
}

Common::UString Module::getDescription(const Common::UString &module) {
	Common::UString description;

	description = getDescriptionCampaign(module);
	if (!description.empty())
		return description;
	description = getDescriptionExtra   (module);
	if (!description.empty())
		return description;

	return "";
}

void Module::toggleWalkmesh() {
	_currentArea->toggleWalkmesh();
}

} // End of namespace NWN

} // End of namespace Engines
