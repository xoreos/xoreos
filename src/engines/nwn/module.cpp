/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/module.cpp
 *  The context needed to run a NWN module.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/configman.h"
#include "common/filepath.h"
#include "common/filelist.h"

#include "events/events.h"

#include "aurora/2dareg.h"
#include "aurora/talkman.h"
#include "aurora/erffile.h"

#include "graphics/camera.h"

#include "graphics/aurora/textureman.h"
#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"
#include "engines/aurora/tokenman.h"
#include "engines/aurora/resources.h"

#include "engines/nwn/types.h"
#include "engines/nwn/module.h"
#include "engines/nwn/area.h"
#include "engines/nwn/console.h"

#include "engines/nwn/gui/ingame/ingame.h"

static const uint32 kStrMaleFemale =  156;
static const uint32 kStrmalefemale = 4924;
static const uint32 kStrBoyGirl    = 4860;
static const uint32 kStrboygirl    = 4862;
static const uint32 kStrBroSis     = 4864;
static const uint32 kStrbrosis     = 4866;
static const uint32 kStrHeShe      = 4869;
static const uint32 kStrheshe      = 4871;
static const uint32 kStrHimHer     = 4873;
static const uint32 kStrhimher     = 4875;
static const uint32 kStrHishis     = 4877;
static const uint32 kStrHershers   = 4879;
static const uint32 kStrLadLass    = 4881;
static const uint32 kStrladlass    = 4883;
static const uint32 kStrLordLady   = 4885;
static const uint32 kStrlordlady   = 4887;
static const uint32 kStrManWoman   = 4926;
static const uint32 kStrmanwoman   = 4928;
static const uint32 kStrBastard    = 1757;
static const uint32 kStrBitch      = 1739;

namespace Engines {

namespace NWN {

Module::Module(Console &console) : _console(&console), _hasModule(false), _hasPC(false),
	_currentTexturePack(-1), _exit(false), _area(0) {

	_ingameGUI = new IngameGUI(*this);
}

Module::~Module() {
	clear();

	delete _ingameGUI;
}

void Module::clear() {
	unload();
}

bool Module::loadModule(const Common::UString &module) {
	unloadModule();

	if (module.empty())
		return false;

	try {
		indexMandatoryArchive(Aurora::kArchiveERF, module, 100, &_resModule);

		_ifo.load();

		if (_ifo.isSave())
			throw Common::Exception("This is a save");

		checkXPs();
		checkHAKs();

		_ifo.loadTLK();

		_tag = _ifo.getTag();

	} catch (Common::Exception &e) {
		e.add("Can't load module \"%s\"", module.c_str());
		printException(e, "WARNING: ");
		return false;
	}

	_hasModule = true;
	return true;
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
		if (!ResMan.hasArchive(Aurora::kArchiveERF, *h + ".hak"))
			throw Common::Exception("Required hak \"%s\" does not exist", h->c_str());
}

void Module::setPCTokens() {
	TokenMan.set("<FullName>" , _pc.getName());
	TokenMan.set("<FirstName>", _pc.getFirstName());
	TokenMan.set("<LastName>" , _pc.getLastName());

	TokenMan.set("<Race>" , _pc.getConvRace());
	TokenMan.set("<race>" , _pc.getConvrace());
	TokenMan.set("<Races>", _pc.getConvRaces());

	TokenMan.set("<Subrace>", _pc.getSubRace());

	TokenMan.set("<Class>"  , _pc.getConvClass());
	TokenMan.set("<class>"  , _pc.getConvclass());
	TokenMan.set("<Classes>", _pc.getConvClasses());

	TokenMan.set("<Deity>", _pc.getDeity());

	TokenMan.set("<Male/Female>"   , TalkMan.getString(kStrMaleFemale + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<male/female>"   , TalkMan.getString(kStrmalefemale + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<Boy/Girl>"      , TalkMan.getString(kStrBoyGirl    + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<boy/girl>"      , TalkMan.getString(kStrboygirl    + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<Brother/Sister>", TalkMan.getString(kStrBroSis     + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<brother/sister>", TalkMan.getString(kStrbrosis     + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<He/She>"        , TalkMan.getString(kStrHeShe      + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<he/she>"        , TalkMan.getString(kStrheshe      + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<Him/Her>"       , TalkMan.getString(kStrHimHer     + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<him/her>"       , TalkMan.getString(kStrhimher     + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<His/his>"       , TalkMan.getString(kStrHishis     + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<Hers/hers>"     , TalkMan.getString(kStrHershers   + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<Lad/Lass>"      , TalkMan.getString(kStrLadLass    + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<lad/lass>"      , TalkMan.getString(kStrladlass    + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<Lord/Lady>"     , TalkMan.getString(kStrLordLady   + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<lord/lady>"     , TalkMan.getString(kStrlordlady   + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<Man/Woman>"     , TalkMan.getString(kStrManWoman   + (_pc.isFemale() ? 1 : 0)));
	TokenMan.set("<man/woman>"     , TalkMan.getString(kStrmanwoman   + (_pc.isFemale() ? 1 : 0)));

	TokenMan.set("<bitch/bastard>", TalkMan.getString(_pc.isFemale() ? kStrBitch : kStrBastard));

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

	TokenMan.remove("<Male/Female>");
	TokenMan.remove("<male/female>");
	TokenMan.remove("<Boy/Girl>");
	TokenMan.remove("<boy/girl>");
	TokenMan.remove("<Brother/Sister>");
	TokenMan.remove("<brother/sister>");
	TokenMan.remove("<He/She>");
	TokenMan.remove("<he/she>");
	TokenMan.remove("<Him/Her>");
	TokenMan.remove("<him/her>");
	TokenMan.remove("<His/his>");
	TokenMan.remove("<Hers/hers>");
	TokenMan.remove("<Lad/Lass>");
	TokenMan.remove("<lad/lass>");
	TokenMan.remove("<Lord/Lady>");
	TokenMan.remove("<lord/lady>");

	TokenMan.remove("<bitch/bastard>");
}

bool Module::usePC(const CharacterID &c) {
	unloadPC();

	if (c.empty())
		return false;

	_pc = *c;

	setPCTokens();
	TalkMan.setGender((Aurora::Gender) _pc.getGender());

	_hasPC = true;
	return true;
}

Creature *Module::getPC() {
	if (!_pc.loaded())
		return 0;

	return &_pc;
}

bool Module::replaceModule(const Common::UString &module) {
	_exit = true;

	unloadArea();
	unloadHAKs();
	unloadModule();

	if (!loadModule(module))
		return false;

	return enter();
}

bool Module::enter() {
	if (!_hasModule) {
		warning("Module::enter(): Lacking a module?!?");
		return false;
	}

	if (!_hasPC) {
		warning("Module::enter(): Lacking a PC?!?");
		return false;
	}

	loadTexturePack();

	_console->printf("Entering module \"%s\" with character \"%s\"",
			_ifo.getName().getString().c_str(), _pc.getName().c_str());

	_ingameGUI->updatePartyMember(0, _pc);

	try {

		loadHAKs();

	} catch (Common::Exception &e) {
		e.add("Can't initialize module \"%s\"", _ifo.getName().getString().c_str());
		printException(e, "WARNING: ");
		return false;
	}

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	_exit    = false;
	_newArea = _ifo.getEntryArea();

	CameraMan.reset();

	float entryX, entryY, entryZ;
	_ifo.getEntryPosition(entryX, entryY, entryZ);

	// Roughly head position
	CameraMan.setPosition(entryX, entryZ + 2.0, entryY);

	float entryDirX, entryDirY;
	_ifo.getEntryDirection(entryDirX, entryDirY);

	CameraMan.setOrientation(entryDirX, entryDirY);

	return true;
}

void Module::run() {
	if (!enter())
		return;

	EventMan.enableUnicode(true);
	EventMan.enableKeyRepeat();

	_ingameGUI->show();

	try {

		EventMan.flushEvents();

		while (!EventMan.quitRequested() && !_exit && !_newArea.empty()) {
			loadArea();
			if (_exit)
				break;

			handleEvents();

			_ingameGUI->updatePartyMember(0, _pc);

			if (!EventMan.quitRequested() && !_exit && !_newArea.empty())
				EventMan.delay(10);
		}

	} catch (Common::Exception &e) {
		e.add("Failed running module \"%s\"", _ifo.getName().getString().c_str());
		printException(e, "WARNING: ");
	}

	_ingameGUI->stopConversation();
	_ingameGUI->hide();

	EventMan.enableUnicode(false);
	EventMan.enableKeyRepeat(0);
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
		if (handleCamera(event))
			continue;

		_ingameGUI->addEvent(event);
		_area->addEvent(event);
	}

	_area->processEventQueue();
	_ingameGUI->processEventQueue();
}

bool Module::handleCamera(const Events::Event &e) {
	if (e.type != Events::kEventKeyDown)
		return false;

	if (e.key.keysym.sym == SDLK_UP)
		CameraMan.move( 0.5);
	else if (e.key.keysym.sym == SDLK_DOWN)
		CameraMan.move(-0.5);
	else if (e.key.keysym.sym == SDLK_RIGHT)
		CameraMan.turn( 0.0,  5.0, 0.0);
	else if (e.key.keysym.sym == SDLK_LEFT)
		CameraMan.turn( 0.0, -5.0, 0.0);
	else if (e.key.keysym.sym == SDLK_w)
		CameraMan.move( 0.5);
	else if (e.key.keysym.sym == SDLK_s)
		CameraMan.move(-0.5);
	else if (e.key.keysym.sym == SDLK_d)
		CameraMan.turn( 0.0,  5.0, 0.0);
	else if (e.key.keysym.sym == SDLK_a)
		CameraMan.turn( 0.0, -5.0, 0.0);
	else if (e.key.keysym.sym == SDLK_e)
		CameraMan.strafe( 0.5);
	else if (e.key.keysym.sym == SDLK_q)
		CameraMan.strafe(-0.5);
	else if (e.key.keysym.sym == SDLK_INSERT)
		CameraMan.move(0.0,  0.5, 0.0);
	else if (e.key.keysym.sym == SDLK_DELETE)
		CameraMan.move(0.0, -0.5, 0.0);
	else if (e.key.keysym.sym == SDLK_PAGEUP)
		CameraMan.turn( 5.0,  0.0, 0.0);
	else if (e.key.keysym.sym == SDLK_PAGEDOWN)
		CameraMan.turn(-5.0,  0.0, 0.0);
	else if (e.key.keysym.sym == SDLK_END) {
		const float *orient = CameraMan.getOrientation();

		CameraMan.setOrientation(0.0, orient[1], orient[2]);
	} else
		return false;

	return true;
}

void Module::unload() {
	unloadArea();
	unloadTexturePack();
	unloadHAKs();
	unloadPC();
	unloadModule();
}

void Module::unloadModule() {
	TwoDAReg.clear();

	_tag.clear();

	_ifo.unload();

	ResMan.undo(_resModule);

	_hasModule = false;
}

void Module::unloadPC() {
	removePCTokens();

	_pc.clear();

	_hasPC = false;
}

void Module::loadHAKs() {
	const std::vector<Common::UString> &haks = _ifo.getHAKs();

	_resHAKs.resize(haks.size());

	for (uint i = 0; i < haks.size(); i++)
		indexMandatoryArchive(Aurora::kArchiveERF, haks[i] + ".hak", 100, &_resHAKs[i]);
}

void Module::unloadHAKs() {
	std::vector<Aurora::ResourceManager::ChangeID>::iterator hak;
	for (hak = _resHAKs.begin(); hak != _resHAKs.end(); ++hak)
		ResMan.undo(*hak);

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

	unloadTexturePack();

	status("Loading texture pack %d", level);
	indexMandatoryArchive(Aurora::kArchiveERF, texturePacks[level][0], 13, &_resTP[0]);
	indexMandatoryArchive(Aurora::kArchiveERF, texturePacks[level][1], 14, &_resTP[1]);
	indexOptionalArchive (Aurora::kArchiveERF, texturePacks[level][2], 15, &_resTP[2]);
	indexOptionalArchive (Aurora::kArchiveERF, texturePacks[level][3], 16, &_resTP[3]);

	// If we already had a texture pack loaded, reload all textures
	if (_currentTexturePack != -1)
		TextureMan.reloadAll();

	_currentTexturePack = level;
}

void Module::unloadTexturePack() {
	for (int i = 0; i < 4; i++)
		ResMan.undo(_resTP[i]);
}

void Module::loadArea() {
	if (_area && (_area->getResRef() == _newArea))
		return;

	_ingameGUI->stopConversation();

	delete _area;
	if (_newArea.empty()) {
		_exit = true;
		return;
	}

	_area = new Area(_newArea);

	_area->show();

	EventMan.flushEvents();

	_ingameGUI->setArea(_area->getName());

	_pc.setArea(_area);

	_console->printf("Entering area \"%s\"", _area->getResRef().c_str());
}

void Module::unloadArea() {
	_ingameGUI->stopConversation();

	delete _area;
	_area = 0;
}

void Module::showMenu() {
	_area->removeFocus();

	if (_ingameGUI->showMain() == 2) {
		_exit = true;
		return;
	}

	// In case we changed the texture pack settings, reload it
	loadTexturePack();
}

Aurora::NWScript::Object *Module::findObject(const Common::UString &tag) {
	if (!_area)
		return 0;

	return _area->findObject(tag);
}

const Aurora::NWScript::Object *Module::findObject(const Common::UString &tag) const {
	if (!_area)
		return 0;

	return _area->findObject(tag);
}

void Module::startConversation(const Common::UString &conv, Creature &pc,
                               Engines::NWN::Object &obj) {

	_ingameGUI->startConversation(conv, pc, obj);
}

void Module::getModules(std::vector<Common::UString> &modules) {
	modules.clear();

	Common::UString moduleDir = ConfigMan.getString("NWN_extraModuleDir");
	if (moduleDir.empty())
		return;

	Common::FileList moduleDirList;
	moduleDirList.addDirectory(moduleDir);

	std::list<Common::UString> mods;
	uint n = moduleDirList.getFileNames(mods);

	mods.sort(Common::UString::iless());

	modules.reserve(n);
	for (std::list<Common::UString>::const_iterator m = mods.begin(); m != mods.end(); ++m) {
		if (!Common::FilePath::getExtension(*m).equalsIgnoreCase(".mod"))
			continue;

		modules.push_back(Common::FilePath::getStem(*m));
	}
}

Common::UString Module::getDescription(const Common::UString &module) {
	try {
		Common::UString moduleDir = ConfigMan.getString("NWN_extraModuleDir");
		Common::UString modFile   = module;

		Aurora::ERFFile mod(moduleDir + "/" + modFile + ".mod", true);

		return mod.getDescription().getString();
	} catch (...) {
	}

	return "";
}

} // End of namespace NWN

} // End of namespace Engines
