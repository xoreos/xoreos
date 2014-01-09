/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/module.cpp
 *  The context needed to run a NWN module.
 */

#include "common/util.h"
#include "common/maths.h"
#include "common/error.h"
#include "common/configman.h"

#include "events/events.h"

#include "aurora/2dareg.h"
#include "aurora/talkman.h"
#include "aurora/erffile.h"

#include "graphics/graphics.h"
#include "graphics/cameraman.h"
#include "graphics/textureman.h"

#include "graphics/aurora/model_nwn.h"

#include "engines/aurora/util.h"
#include "engines/aurora/tokenman.h"
#include "engines/aurora/resources.h"

#include "engines/nwn/types.h"
#include "engines/nwn/module.h"
#include "engines/nwn/area.h"

namespace Engines {

namespace NWN {

Module::Module() : _hasModule(false), _currentTexturePack(-1), _exit(false), _currentArea(0) {
}

Module::~Module() {
	clear();
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

	_newModule.clear();

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

bool Module::replaceModule() {
	if (_newModule.empty())
		return true;

	Common::UString newModule = _newModule;

	unloadAreas();
	unloadHAKs();
	unloadModule();

	_exit = true;

	if (!loadModule(newModule))
		return false;

	return enter();
}

bool Module::enter() {
	if (!_hasModule) {
		warning("Module::enter(): Lacking a module?!?");
		return false;
	}

	loadTexturePack();

	try {

		loadHAKs();
		loadAreas();

	} catch (Common::Exception &e) {
		e.add("Can't initialize module \"%s\"", _ifo.getName().getString().c_str());
		printException(e, "WARNING: ");
		return false;
	}

	float entryX, entryY, entryZ, entryDirX, entryDirY;
	_ifo.getEntryPosition(entryX, entryY, entryZ);
	_ifo.getEntryDirection(entryDirX, entryDirY);

	float oRadian, oX, oY, oZ;
	convertVector2Orientation(entryDirX, entryDirY, oRadian, oX, oY, oZ);

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	_exit    = false;
	_newArea = _ifo.getEntryArea();

	CameraMan.reset();

	// Roughly head position
	CameraMan.setPosition(entryX, entryZ + 2.0, -entryY);
	CameraMan.setOrientation(oRadian, oX, oY, oZ);

	return true;
}

void Module::enterArea() {
	if (_currentArea && (_currentArea->getResRef() == _newArea))
		return;

	if (_currentArea) {
		_currentArea->setVisible(false);

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

	_currentArea->setVisible(true);

	EventMan.flushEvents();
}

void Module::run() {
	if (!enter())
		return;

	EventMan.enableKeyRepeat();

	try {

		EventMan.flushEvents();

		while (!EventMan.quitRequested() && !_exit && !_newArea.empty()) {
			replaceModule();
			enterArea();
			if (_exit)
				break;

			handleEvents();

			if (!EventMan.quitRequested() && !_exit && !_newArea.empty())
				EventMan.delay(10);
		}

	} catch (Common::Exception &e) {
		e.add("Failed running module \"%s\"", _ifo.getName().getString().c_str());
		printException(e, "WARNING: ");
	}

	EventMan.enableKeyRepeat(0);
}

void Module::handleEvents() {
	Events::Event event;
	while (EventMan.pollEvent(event)) {
		// Camera
		if (SDL_IsTextInputActive() == SDL_FALSE)
			if (handleCamera(event))
				continue;

		_currentArea->addEvent(event);
	}

	_currentArea->processEventQueue();
}

bool Module::handleCamera(const Events::Event &e) {
	if (e.type != Events::kEventKeyDown)
		return false;

	float speed = (e.key.keysym.mod & KMOD_SHIFT) ? 1.0 : 0.5;

	if      (e.key.keysym.sym == SDLK_UP)
		CameraMan.moveRelative(0.0, 0.0, -speed);
	else if (e.key.keysym.sym == SDLK_DOWN)
		CameraMan.moveRelative(0.0, 0.0,  speed);
	else if (e.key.keysym.sym == SDLK_RIGHT)
		CameraMan.rotate(Common::deg2rad(-5), 0.0, 1.0, 0.0);
	else if (e.key.keysym.sym == SDLK_LEFT)
		CameraMan.rotate(Common::deg2rad( 5), 0.0, 1.0, 0.0);
	else if (e.key.keysym.scancode == SDL_SCANCODE_W)
		CameraMan.moveRelative(0.0, 0.0, -speed);
	else if (e.key.keysym.scancode == SDL_SCANCODE_S)
		CameraMan.moveRelative(0.0, 0.0,  speed);
	else if (e.key.keysym.scancode == SDL_SCANCODE_D)
		CameraMan.rotate(Common::deg2rad(-5), 0.0, 1.0, 0.0);
	else if (e.key.keysym.scancode == SDL_SCANCODE_A)
		CameraMan.rotate(Common::deg2rad( 5), 0.0, 1.0, 0.0);
	else if (e.key.keysym.scancode == SDL_SCANCODE_E)
		CameraMan.moveRelative( speed, 0.0, 0.0);
	else if (e.key.keysym.scancode == SDL_SCANCODE_Q)
		CameraMan.moveRelative(-speed, 0.0, 0.0);
	else if (e.key.keysym.sym == SDLK_INSERT)
		CameraMan.moveRelative(0.0,  speed, 0.0);
	else if (e.key.keysym.sym == SDLK_DELETE)
		CameraMan.moveRelative(0.0, -speed, 0.0);
	else if (e.key.keysym.sym == SDLK_PAGEUP)
		CameraMan.pitch(Common::deg2rad(5));
		//CameraMan.rotate(Common::deg2rad( 5), 1.0, 0.0, 0.0);
	else if (e.key.keysym.sym == SDLK_PAGEDOWN)
		CameraMan.pitch(Common::deg2rad(-5));
		//CameraMan.rotate(Common::deg2rad(-5), 1.0, 0.0, 0.0);
	else if (e.key.keysym.sym == SDLK_END) {
		float x, y, z;
		CameraMan.getDirection(x,   y, z);
		CameraMan.setDirection(x, 0.0, z);
	} else if (e.key.keysym.sym == SDLK_RETURN) {
		GfxMan.toggleVSync();
	} else if (e.key.keysym.scancode == SDL_SCANCODE_1) {
		GfxMan.setFSAA(GfxMan.getCurrentFSAA() / 2);
	} else if (e.key.keysym.scancode == SDL_SCANCODE_2) {
		GfxMan.setFSAA((GfxMan.getCurrentFSAA() == 0) ? 2 : (GfxMan.getCurrentFSAA() * 2));
	} else if (e.key.keysym.scancode == SDL_SCANCODE_F) {
		double averageFrameTime, averageFPS;
		if (GfxMan.getRenderStatistics(averageFrameTime, averageFPS))
			warning("FPS: %lf, AverageFrameTime: %lf", averageFPS, averageFrameTime);
		else
			warning("NO STATS");

	} else
		return false;

	return true;
}

void Module::unload() {
	unloadAreas();
	unloadTexturePack();
	unloadHAKs();
	unloadModule();
}

void Module::unloadModule() {
	TwoDAReg.clear();

	_tag.clear();

	_ifo.unload();

	ResMan.undo(_resModule);

	_newModule.clear();
	_hasModule = false;
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

	/*
	// If we already had a texture pack loaded, reload all textures
	if (_currentTexturePack != -1)
		TextureMan.reloadAll();
	*/

	_currentTexturePack = level;
}

void Module::unloadTexturePack() {
	for (int i = 0; i < 4; i++)
		ResMan.undo(_resTP[i]);

	_currentTexturePack = -1;
}

void Module::loadAreas() {
	status("Loading areas...");

	const std::vector<Common::UString> &areas = _ifo.getAreas();
	for (uint32 i = 0; i < areas.size(); i++) {
		status("Loading area \"%s\" (%d / %d)", areas[i].c_str(), i, (int) areas.size() - 1);

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
	for (AreaMap::iterator a = _areas.begin(); a != _areas.end(); ++a)
		delete a->second;

	_areas.clear();
	_newArea.clear();

	_currentArea = 0;
}

const Common::UString &Module::getName() const {
	return _ifo.getName().getString();
}

void Module::changeModule(const Common::UString &module) {
	_newModule = module;
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
