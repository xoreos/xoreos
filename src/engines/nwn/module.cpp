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
 *  The context needed to run a NWN module.
 */

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/configman.h"

#include "src/events/events.h"

#include "src/aurora/2dareg.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/erffile.h"
#include "src/aurora/resman.h"

#include "src/graphics/camera.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/tokenman.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/camera.h"
#include "src/engines/aurora/console.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/version.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/area.h"

#include "src/engines/nwn/script/container.h"

#include "src/engines/nwn/gui/ingame/ingame.h"

struct GenderToken {
	const char *token;
	uint32 male;
	uint32 female;
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


Module::Module(::Engines::Console &console, const Version &gameVersion) :
	_console(&console), _gameVersion(&gameVersion),
	_hasModule(false), _running(false), _pc(0),
	_currentTexturePack(-1), _exit(false), _currentArea(0) {

	_ingameGUI = new IngameGUI(*this, _console);
}

Module::~Module() {
	try {
		clear();
	} catch (...) {
	}

	delete _ingameGUI;
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

void Module::loadModule(const Common::UString &module) {
	unload(false);

	if (module.empty())
		throw Common::Exception("Tried to load an empty module");

	try {
		indexMandatoryArchive(module, 1000, &_resModule);

		_ifo.load();

		if (_ifo.isSave())
			throw Common::Exception("This is a save");

		checkXPs();
		checkHAKs();

		_tag = _ifo.getTag();

		readScripts(*_ifo.getGFF());

	} catch (Common::Exception &e) {
		e.add("Can't load module \"%s\"", module.c_str());
		throw e;
	}

	_newModule.clear();

	_hasModule = true;
}

void Module::checkXPs() {
	uint16 hasXP = 0;

	hasXP |= ConfigMan.getBool("NWN_hasXP1") ? 1 : 0;
	hasXP |= ConfigMan.getBool("NWN_hasXP2") ? 2 : 0;
	hasXP |= ConfigMan.getBool("NWN_hasXP3") ? 4 : 0;

	uint16 xp = _ifo.getExpansions();

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
		const uint32 strRef = _pc->isFemale() ? kGenderTokens[i].female : kGenderTokens[i].male;

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
		_pc = new Creature(bic, local);
	} catch (Common::Exception &e) {
		e.add("Can't load PC \"%s\"", bic.c_str());
		throw e;
	}

	setPCTokens();
	LangMan.setCurrentGender(_pc->isFemale() ? Aurora::kLanguageGenderFemale : Aurora::kLanguageGenderMale);

	addObject(*_pc);
}

Creature *Module::getPC() {
	return _pc;
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

	runScript(kScriptModuleLoad , this, _pc);
	runScript(kScriptModuleStart, this, _pc);
	runScript(kScriptEnter      , this, _pc);

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	_exit    = false;
	_newArea = _ifo.getEntryArea();

	CameraMan.reset();

	// Roughly head position
	CameraMan.setPosition(entryX, entryY, entryZ + 1.8f);
	CameraMan.setOrientation(90.0f, 0.0f, entryAngle);
	CameraMan.update();
}

void Module::enterArea() {
	if (_currentArea && (_currentArea->getResRef() == _newArea))
		return;

	_ingameGUI->stopConversation();

	if (_currentArea) {
		_pc->hide();

		_currentArea->runScript(kScriptExit, _currentArea, _pc);
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

	_currentArea->runScript(kScriptEnter, _currentArea, _pc);

	_console->printf("Entering area \"%s\"", _currentArea->getResRef().c_str());
}

void Module::exit() {
	_ingameGUI->abortMain();

	_exit = true;
}

bool Module::isRunning() const {
	return _running;
}

void Module::run() {
	enter();
	_running = true;

	EventMan.enableKeyRepeat();

	_ingameGUI->show();

	try {

		EventMan.flushEvents();

		while (!EventMan.quitRequested() && !_exit && !_newArea.empty()) {
			replaceModule();
			enterArea();
			if (_exit)
				break;

			handleEvents();
			handleActions();

			_ingameGUI->updatePartyMember(0, *_pc);

			if (!EventMan.quitRequested() && !_exit && !_newArea.empty())
				EventMan.delay(10);
		}

	} catch (Common::Exception &e) {
		_running = false;

		e.add("Failed running module \"%s\"", _ifo.getName().getString().c_str());
		throw e;
	}

	_ingameGUI->stopConversation();
	_ingameGUI->hide();

	EventMan.enableKeyRepeat(0);

	_running = false;
}

void Module::handleEvents() {
	Events::Event event;
	while (EventMan.pollEvent(event)) {
		// Handle console
		if (_console->processEvent(event)) {
			if (!_currentArea)
				return;

			continue;
		}

		if (event.type == Events::kEventKeyDown) {
			// Menu
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				// But only if we're not in a conversation, where ESC should abort that
				if (!_ingameGUI->hasRunningConversation()) {
					showMenu();
					continue;
				}
			}

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

		_ingameGUI->addEvent(event);
		_currentArea->addEvent(event);
	}

	CameraMan.update();
	_currentArea->processEventQueue();
	_ingameGUI->processEventQueue();
}

void Module::handleActions() {
	uint32 now = EventMan.getTimestamp();

	while (!_delayedActions.empty()) {
		std::multiset<Action>::iterator action = _delayedActions.begin();

		if (now < action->timestamp)
			break;

		if (action->type == kActionScript)
			ScriptContainer::runScript(action->script, action->state,
			                           action->owner, action->triggerer);

		_delayedActions.erase(action);
	}
}

void Module::unload(bool completeUnload) {
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
	runScript(kScriptExit, this, _pc);
	handleActions();

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

	delete _pc;
	_pc = 0;
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
		indexMandatoryArchive(haks[i] + ".hak", 1001 + i, &_resHAKs[i]);
}

void Module::unloadHAKs() {
	std::vector<Common::ChangeID>::iterator hak;
	for (hak = _resHAKs.begin(); hak != _resHAKs.end(); ++hak)
		deindexResources(*hak);

	_resHAKs.clear();
}

static const char *texturePacks[4][4] = {
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

void Module::unloadAreas() {
	_ingameGUI->stopConversation();

	for (AreaMap::iterator a = _areas.begin(); a != _areas.end(); ++a)
		delete a->second;

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
                               Engines::NWN::Object &obj, bool playHello) {

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

const Common::UString &Module::getName() const {
	return _ifo.getName().getString();
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

Common::UString Module::getDescription(const Common::UString &module) {
	try {
		Common::UString moduleDir = ConfigMan.getString("NWN_extraModuleDir");
		Common::UString modFile   = module;

		return Aurora::ERFFile::getDescription(moduleDir + "/" + modFile + ".mod").getString();
	} catch (...) {
	}

	return "";
}

} // End of namespace NWN

} // End of namespace Engines
