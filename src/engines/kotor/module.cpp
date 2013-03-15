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
 *  A module.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/ustring.h"

#include "graphics/camera.h"

#include "graphics/aurora/textureman.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"
#include "engines/aurora/console.h"

#include "engines/kotor/module.h"
#include "engines/kotor/area.h"

namespace Engines {

namespace KotOR {

Module::Module(Console &console) : _console(&console), _area(0),
	_currentTexturePack(-1), _exit(false) {

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

	EventMan.enableUnicode(true);
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

	EventMan.enableUnicode(false);
	EventMan.enableKeyRepeat(0);
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

	// If we already had a texture pack loaded, reload all textures
	if (_currentTexturePack != -1)
		TextureMan.reloadAll();

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
	_area = 0;
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

	_console->printf("Entering module \"%s\"", _ifo.getName().getString().c_str());

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	_exit = false;

	CameraMan.reset();

	float entryX, entryY, entryZ;
	_ifo.getEntryPosition(entryX, entryY, entryZ);

	// Roughly head position
	CameraMan.setPosition(entryX, entryZ + 1.8, entryY);

	float entryDirX, entryDirY;
	_ifo.getEntryDirection(entryDirX, entryDirY);

	CameraMan.setOrientation(entryDirX, entryDirY);

	_area->show();
}

void Module::leave() {
	if (!_area)
		return;

	_area->hide();
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
				showMenu();
				continue;
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

		_area->addEvent(event);
	}

	_area->processEventQueue();
}

bool Module::handleCamera(const Events::Event &e) {

	if (e.type == Events::kEventKeyDown)
		return handleCameraKeyboardInput(e);
	else if (e.type == Events::kEventMouseMove)
		return handleCameraMouseInput(e);

	return false;
}

bool Module::handleCameraKeyboardInput(const Events::Event &e) {
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

bool Module::handleCameraMouseInput(const Events::Event &e) {
	// Holding down the right mouse button enables free look.
	if (e.motion.state & SDL_BUTTON(3))
		CameraMan.turn(-0.5 * e.motion.yrel, 0.5 * e.motion.xrel, 0.0);
	else
		return false;

	return true;
}

Area *Module::createArea() const {
	return new Area;
}

} // End of namespace KotOR

} // End of namespace Engines
