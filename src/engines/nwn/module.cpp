/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
#include "aurora/erffile.h"

#include "graphics/graphics.h"
#include "graphics/camera.h"

#include "graphics/aurora/textureman.h"
#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"

#include "engines/nwn/types.h"
#include "engines/nwn/module.h"
#include "engines/nwn/area.h"
#include "engines/nwn/console.h"

#include "engines/nwn/gui/ingame/ingame.h"

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

bool Module::usePC(const CharacterID &c) {
	unloadPC();

	if (c.empty())
		return false;

	_pc = *c;

	_hasPC = true;
	return true;
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
			_ifo.getName().getString().c_str(), _pc.getFullName().c_str());

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

	_ingameGUI->updateCompass();

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
	_ingameGUI->updateCompass();

	return true;
}

Graphics::Aurora::Model *Module::getModelAt(float x, float y) {
	Graphics::Aurora::Model *model =
		dynamic_cast<Graphics::Aurora::Model *>(GfxMan.getObjectAt(x, y));

	return model;
}

void Module::run() {
	if (!enter())
		return;

	EventMan.enableUnicode(true);
	EventMan.enableKeyRepeat();

	_ingameGUI->show();

	try {

		EventMan.flushEvents();

		Graphics::Aurora::Model *activeModel = 0;

		while (!EventMan.quitRequested() && !_exit && !_newArea.empty()) {
			loadArea();
			if (_exit)
				break;

			Events::Event event;
			while (EventMan.pollEvent(event)) {
				_ingameGUI->evaluateEvent(event);

				if (_console->processEvent(event))
					continue;

				if (event.type == Events::kEventKeyDown) {
					if      (event.key.keysym.sym == SDLK_ESCAPE)
						showMenu();
					else if ((event.key.keysym.sym == SDLK_d) && (event.key.keysym.mod & KMOD_CTRL))
						_console->show();
					else if (event.key.keysym.sym == SDLK_UP)
						CameraMan.move( 0.5);
					else if (event.key.keysym.sym == SDLK_DOWN)
						CameraMan.move(-0.5);
					else if (event.key.keysym.sym == SDLK_RIGHT)
						CameraMan.turn( 0.0,  5.0, 0.0);
					else if (event.key.keysym.sym == SDLK_LEFT)
						CameraMan.turn( 0.0, -5.0, 0.0);
					else if (event.key.keysym.sym == SDLK_w)
						CameraMan.move( 0.5);
					else if (event.key.keysym.sym == SDLK_s)
						CameraMan.move(-0.5);
					else if (event.key.keysym.sym == SDLK_d)
						CameraMan.turn( 0.0,  5.0, 0.0);
					else if (event.key.keysym.sym == SDLK_a)
						CameraMan.turn( 0.0, -5.0, 0.0);
					else if (event.key.keysym.sym == SDLK_e)
						CameraMan.strafe( 0.5);
					else if (event.key.keysym.sym == SDLK_q)
						CameraMan.strafe(-0.5);
					else if (event.key.keysym.sym == SDLK_INSERT)
						CameraMan.move(0.0,  0.5, 0.0);
					else if (event.key.keysym.sym == SDLK_DELETE)
						CameraMan.move(0.0, -0.5, 0.0);
					else if (event.key.keysym.sym == SDLK_PAGEUP)
						CameraMan.turn( 5.0,  0.0, 0.0);
					else if (event.key.keysym.sym == SDLK_PAGEDOWN)
						CameraMan.turn(-5.0,  0.0, 0.0);
					else if (event.key.keysym.sym == SDLK_END) {
						const float *orient = CameraMan.getOrientation();

						CameraMan.setOrientation(0.0, orient[1], orient[2]);
					}
				} else if (event.type == Events::kEventMouseMove) {
					Graphics::Aurora::Model *model = getModelAt(event.motion.x, event.motion.y);

					if (model != activeModel) {
						if (activeModel)
							activeModel->drawBound(false);

						activeModel = model;

						if (activeModel) {
							warning("Now in \"%s\" (%d)", activeModel->getTag().c_str(),
									activeModel->getID());
							activeModel->drawBound(true);
						}
					}

				}
			}

			_ingameGUI->updatePartyMember(0, _pc);
			_ingameGUI->updateCompass();

			EventMan.delay(10);
		}

	} catch (Common::Exception &e) {
		e.add("Failed running module \"%s\"", _ifo.getName().getString().c_str());
		printException(e, "WARNING: ");
	}

	_ingameGUI->hide();

	EventMan.enableUnicode(false);
	EventMan.enableKeyRepeat(0);
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

	_ifo.unload();

	ResMan.undo(_resModule);

	_hasModule = false;
}

void Module::unloadPC() {
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

	delete _area;
	if (_newArea.empty()) {
		_exit = true;
		return;
	}

	_area = new Area(*this, _newArea);

	_area->show();

	EventMan.flushEvents();

	_ingameGUI->setArea(_area->getName());

	_console->printf("Entering area \"%s\"", _area->getResRef().c_str());
}

void Module::unloadArea() {
	delete _area;
	_area = 0;
}

void Module::showMenu() {
	if (_ingameGUI->showMain() == 2) {
		_exit = true;
		return;
	}

	// In case we changed the texture pack settings, reload it
	loadTexturePack();
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
