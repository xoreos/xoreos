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

/** @file engines/kotor/module.cpp
 *  The context needed to run a KotOR module.
 */

#include "common/util.h"
#include "common/maths.h"
#include "common/error.h"
#include "common/ustring.h"

#include "graphics/graphics.h"
#include "graphics/cameraman.h"
#include "graphics/textureman.h"
#include "graphics/cursorman.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"

#include "engines/kotor/module.h"
#include "engines/kotor/area.h"
#include "engines/kotor/object.h"

namespace Engines {

namespace KotOR {

Module::Module() : _area(0), _currentTexturePack(-1), _exit(false), _currentObject(0) {
}

Module::~Module() {
	clear();
}

void Module::clear() {
	unload();
	unloadTexturePack();
};

bool Module::load(const Common::UString &module) {
	unload();

	_module = module;

	try {

		load();

	} catch (Common::Exception &e) {
		e.add("Failed loading module \"%s\"", _module.c_str());
		printException(e);

		_module.clear();
		return false;
	}

	return true;
}

void Module::run() {
	if (_module.empty())
		throw Common::Exception("No module");

	enter();

	EventMan.enableKeyRepeat();

	try {

		EventMan.flushEvents();

		while (!EventMan.quitRequested() && !_exit) {
			handleEvents();

			if (!EventMan.quitRequested() && !_exit)
				EventMan.delay(10);
		}

	} catch (Common::Exception &e) {
		e.add("Failed running module \"%s\"", _ifo.getName().getString().c_str());
		printException(e, "WARNING: ");
	}

	EventMan.enableKeyRepeat(0);
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
	// exploited by the KotOR 2 TSL Restored Content Mod.

	Aurora::ResourceManager::ChangeID change;

	// General module resources

	if (!indexOptionalArchive(Aurora::kArchiveERF, _module + ".erf", 100, &change))
		indexMandatoryArchive(Aurora::kArchiveRIM, _module + ".rim", 100, &change);

	_resources.push_back(change);
	change.clear();

	// Scripts

	if (!indexOptionalArchive(Aurora::kArchiveERF, _module + "_s.erf", 101, &change))
		indexMandatoryArchive(Aurora::kArchiveRIM, _module + "_s.rim", 101, &change);

	_resources.push_back(change);
	change.clear();

	// Dialogs, KotOR 2 only

	if (!indexOptionalArchive(Aurora::kArchiveERF, _module + "_dlg.erf", 102, &change))
		indexOptionalArchive(Aurora::kArchiveRIM, _module + "_dlg.rim", 102, &change);

	_resources.push_back(change);
	change.clear();

	// Layouts, Xbox only

	indexOptionalArchive(Aurora::kArchiveRIM, _module + "_a.rim", 103, &change);

	_resources.push_back(change);
	change.clear();

	// Textures, Xbox only

	indexOptionalArchive(Aurora::kArchiveRIM, _module + "_adx.rim", 104, &change);

	_resources.push_back(change);
	change.clear();
}

void Module::loadIFO() {
	_ifo.load();
}

void Module::loadArea() {
	_area = createArea();

	_area->load(_ifo.getEntryArea());
}

static const char *texturePacks[3] = {
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
	indexOptionalArchive(Aurora::kArchiveERF, texturePacks[level], 25, &_textures);

	/*
	// If we already had a texture pack loaded, reload all textures
	if (_currentTexturePack != -1)
		TextureMan.reloadAll();
	*/

	_currentTexturePack = level;
}

void Module::unload() {
	leave();

	unloadArea();
	unloadIFO();
	unloadResources();

	_module.clear();
}

void Module::unloadResources() {
	std::list<Aurora::ResourceManager::ChangeID>::reverse_iterator r;
	for (r = _resources.rbegin(); r != _resources.rend(); ++r)
		ResMan.undo(*r);

	_resources.clear();
}

void Module::unloadIFO() {
	_ifo.unload();
}

void Module::unloadArea() {
	delete _area;

	_area          = 0;
	_currentObject = 0;
}

void Module::unloadTexturePack() {
	ResMan.undo(_textures);
	_currentTexturePack = -1;
}

bool Module::replaceModule(const Common::UString &module) {
	_exit = true;

	unload();
	if (!load(module))
		return false;

	enter();
	return true;
}

void Module::enter() {
	assert(_area);

	warning("Entering module \"%s\"", _ifo.getName().getString().c_str());

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	_exit = false;

	float entryX, entryY, entryZ, entryDirX, entryDirY;
	_ifo.getEntryPosition(entryX, entryY, entryZ);
	_ifo.getEntryDirection(entryDirX, entryDirY);

	float oRadian, oX, oY, oZ;
	convertVector2Orientation(entryDirX, entryDirY, oRadian, oX, oY, oZ);

	CameraMan.reset();

	// Roughly head position
	CameraMan.setPosition(entryX, entryZ + 2.0, -entryY);
	CameraMan.setOrientation(oRadian, oX, oY, oZ);

	_area->setVisible(true);
}

void Module::leave() {
	if (!_area)
		return;

	_area->setVisible(false);
}

void Module::handleEvents() {
	Events::Event event;
	while (EventMan.pollEvent(event)) {
		// Camera
		if (SDL_IsTextInputActive() == SDL_FALSE) {
			if (handleCamera(event))
				continue;
			if (handlePicker(event))
				continue;
			if (handleKeys(event))
				continue;
		}
	}
}

bool Module::handleCamera(const Events::Event &e) {
	if (e.type != Events::kEventKeyDown) {
		if (e.type == Events::kEventMouseMove) {
			if (e.motion.state & SDL_BUTTON(2)) {
				CameraMan.pitch(-Common::deg2rad(0.5) * e.motion.yrel);
				CameraMan.rotate(-Common::deg2rad(0.5) * e.motion.xrel, 0.0, 1.0, 0.0);
				return true;
			}
		}

		return false;
	}

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
	else if (e.key.keysym.sym == SDLK_PAGEDOWN)
		CameraMan.pitch(Common::deg2rad(-5));
	else if (e.key.keysym.sym == SDLK_END) {
		float x, y, z;
		CameraMan.getDirection(x,   y, z);
		CameraMan.setDirection(x, 0.0, z);
	} else
		return false;

	checkCurrentObject();
	return true;
}

bool Module::handlePicker(const Events::Event &e) {
	if (e.type != Events::kEventMouseMove)
		return false;

	checkCurrentObject();
	return true;
}

bool Module::handleKeys(const Events::Event &e) {
	if ((e.type != Events::kEventKeyDown) && (e.type != Events::kEventKeyUp))
		return false;

	if (e.type == Events::kEventKeyDown) {
		if        (e.key.keysym.sym == SDLK_RETURN)
			GfxMan.toggleVSync();
		else if (e.key.keysym.scancode == SDL_SCANCODE_1)
			GfxMan.setFSAA(GfxMan.getCurrentFSAA() / 2);
		else if (e.key.keysym.scancode == SDL_SCANCODE_2)
			GfxMan.setFSAA((GfxMan.getCurrentFSAA() == 0) ? 2 : (GfxMan.getCurrentFSAA() * 2));
		else if (e.key.keysym.sym == SDLK_TAB)
			_area->setHighlightAll(true);
		else if (e.key.keysym.scancode == SDL_SCANCODE_F)
			warning("FPS: %lf", GfxMan.getFPS());
		else
			return false;
	}

	if (e.type == Events::kEventKeyUp) {
		if (e.key.keysym.sym == SDLK_TAB) {
			_area->setHighlightAll(false);
			checkCurrentObject();
		} else
			return false;
	}

	return true;
}

void Module::checkCurrentObject() {
	int x, y;
	CursorMan.getPosition(x, y);

	float distance;
	Engines::KotOR::Object *object = _area->getObjectAt(x, y, distance);

	// Highlight the new object
	if (object)
		object->setHighlight(true);

	// If the highlighting changed...
	if (_currentObject != object) {
		// If we're not highlighting everything anyway, dehighlight the old object.
		if (_currentObject && !_area->getHighlightAll())
				_currentObject->setHighlight(false);

		_currentObject = object;

		// Act on a new object
		if (_currentObject)
			warning("Entered object \"%s\" (\"%s\")", _currentObject->getTag().c_str(), _currentObject->getName().c_str());
	}
}

Area *Module::createArea() const {
	return new Area;
}

} // End of namespace KotOR

} // End of namespace Engines
