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

/** @file engines/nwn2/module.cpp
 *  The context needed to run a NWN2 module.
 */

#include "common/util.h"
#include "common/error.h"

#include "aurora/talkman.h"
#include "aurora/erffile.h"

#include "graphics/camera.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"

#include "engines/nwn2/module.h"
#include "engines/nwn2/console.h"
#include "engines/nwn2/area.h"


namespace Engines {

namespace NWN2 {

Module::Module(Console &console) : _console(&console), _hasModule(false),
	_exit(false), _currentArea(0) {

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
		indexMandatoryArchive(Aurora::kArchiveERF, module, 1001, &_resModule);

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

	_console->hide();

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

	_console->printf("Entering module \"%s\"", _ifo.getName().getString().c_str());

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

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	_exit    = false;
	_newArea = _ifo.getEntryArea();

	CameraMan.reset();

	// Roughly head position
	CameraMan.setPosition(entryX, entryZ + 2.0, entryY);
	CameraMan.setOrientation(entryDirX, entryDirY);
	CameraMan.update();

	return true;
}

void Module::enterArea() {
	if (_currentArea && (_currentArea->getResRef() == _newArea))
		return;

	if (_currentArea) {
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

	EventMan.flushEvents();

	_console->printf("Entering area \"%s\" \(\"%s\", \"%s\")", _currentArea->getResRef().c_str(),
			_currentArea->getName().c_str(), _currentArea->getDisplayName().c_str());
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
		// Handle console
		if (_console->processEvent(event)) {
			if (!_currentArea)
				return;

			continue;
		}

		if (event.type == Events::kEventKeyDown) {
			// Console
			if ((event.key.keysym.sym == SDLK_d) && (event.key.keysym.mod & KMOD_CTRL)) {
				_console->show();
				continue;
			}
		}

		// Camera
		if (!_console->isVisible())
			if (handleCamera(event))
				continue;
	}

	CameraMan.update();
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
	else if (e.key.keysym.scancode == SDL_SCANCODE_W)
		CameraMan.move( 0.5);
	else if (e.key.keysym.scancode == SDL_SCANCODE_S)
		CameraMan.move(-0.5);
	else if (e.key.keysym.scancode == SDL_SCANCODE_D)
		CameraMan.turn( 0.0,  5.0, 0.0);
	else if (e.key.keysym.scancode == SDL_SCANCODE_A)
		CameraMan.turn( 0.0, -5.0, 0.0);
	else if (e.key.keysym.scancode == SDL_SCANCODE_E)
		CameraMan.strafe( 0.5);
	else if (e.key.keysym.scancode == SDL_SCANCODE_Q)
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
	unloadAreas();
	unloadHAKs();
	unloadModule();
}

void Module::unloadModule() {
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
		indexMandatoryArchive(Aurora::kArchiveERF, haks[i] + ".hak", 1002 + i, &_resHAKs[i]);
}

void Module::unloadHAKs() {
	std::vector<Aurora::ResourceManager::ChangeID>::iterator hak;
	for (hak = _resHAKs.begin(); hak != _resHAKs.end(); ++hak)
		ResMan.undo(*hak);

	_resHAKs.clear();
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

void Module::changeModule(const Common::UString &module) {
	_newModule = module;
}

const Common::UString &Module::getName() const {
	return _ifo.getName().getString();
}

} // End of namespace NWN2

} // End of namespace Engines
