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
 *  A module.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/graphics/camera.h"

#include "src/graphics/aurora/textureman.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/camera.h"
#include "src/engines/aurora/console.h"

#include "src/engines/kotor/module.h"
#include "src/engines/kotor/area.h"

namespace Engines {

namespace KotOR {

Module::Module(::Engines::Console &console) : _console(&console),
	_hasModule(false), _running(false),
	_currentTexturePack(-1), _exit(false), _area(0) {

}

Module::~Module() {
	clear();
}

void Module::clear() {
	unload();
	unloadTexturePack();
}

void Module::load(const Common::UString &module) {
	if (isRunning()) {
		// We are currently running a module. Schedule a safe change instead

		changeModule(module);
		return;
	}

	// We are not currently running a module. Directly load the new module
	loadModule(module);
}

void Module::loadModule(const Common::UString &module) {
	unload();

	_module = module;

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

void Module::run() {
	enter();
	_running = true;

	EventMan.enableKeyRepeat();

	try {

		EventMan.flushEvents();

		while (!EventMan.quitRequested() && !_exit) {
			replaceModule();
			if (_exit)
				break;

			handleEvents();

			if (!EventMan.quitRequested() && !_exit)
				EventMan.delay(10);
		}

	} catch (Common::Exception &e) {
		_running = false;

		e.add("Failed running module \"%s\"", _ifo.getName().getString().c_str());
		throw e;
	}

	EventMan.enableKeyRepeat(0);

	_running = false;
}

bool Module::isRunning() const {
	return _running;
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
	_area = new Area;

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

	_newModule.clear();
	_hasModule = false;

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

void Module::changeModule(const Common::UString &module) {
	_newModule = module;
}

void Module::replaceModule() {
	if (_newModule.empty())
		return;

	_console->hide();

	Common::UString newModule = _newModule;

	unload();

	_exit = true;

	loadModule(newModule);
	enter();
}

void Module::enter() {
	if (!_hasModule || !_area)
		throw Common::Exception("Module::enter(): Lacking a module?!?");

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
	CameraMan.update();

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
		if (!_console->isVisible())
			if (handleCameraInput(event))
				continue;

		_area->addEvent(event);
	}

	CameraMan.update();
	_area->processEventQueue();
}

const Common::UString &Module::getName() const {
	return _module;
}

const Aurora::IFOFile &Module::getIFO() const {
	return _ifo;
}

Area *Module::getCurrentArea() {
	return _area;
}

} // End of namespace KotOR

} // End of namespace Engines
