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

/** @file engines/witcher/module.cpp
 *  The context needed to run a Witcher module.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/configman.h"

#include "src/aurora/talkman.h"
#include "src/aurora/erffile.h"

#include "src/graphics/camera.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/camera.h"

#include "src/engines/witcher/module.h"
#include "src/engines/witcher/campaign.h"


namespace Engines {

namespace Witcher {

Module::Module(Campaign &campaign) : _campaign(&campaign),
	_hasModule(false), _running(false), _exit(false) {

}

Module::~Module() {
	clear();
}

void Module::clear() {
	unload();
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

	if (module.empty())
		throw Common::Exception("Tried to load an empty module");

	try {
		indexMandatoryArchive(Aurora::kArchiveERF, module, 1001, &_resModule);

		_ifo.load();

		if (_ifo.isSave())
			throw Common::Exception("This is a save");

		_ifo.loadTLK();

		_tag = _ifo.getTag();

	} catch (Common::Exception &e) {
		e.add("Can't load module \"%s\"", module.c_str());
		throw e;
	}

	_newModule.clear();

	_hasModule = true;
}

void Module::changeModule(const Common::UString &module) {
	_newModule = module;
}

void Module::replaceModule() {
	// Look if a campaign replacement was scheduled
	_campaign->replaceCampaign();

	if (_newModule.empty())
		return;

	Common::UString newModule = _newModule;

	unload();

	_exit = true;

	loadModule(newModule);
	enter();
}

void Module::enter() {
	if (!_hasModule)
		throw Common::Exception("Module::enter(): Lacking a module?!?");

	try {

		loadAreas();

	} catch (Common::Exception &e) {
		e.add("Can't initialize module \"%s\"", _ifo.getName().getString().c_str());
		throw e;
	}

	float entryX, entryY, entryZ, entryDirX, entryDirY;
	_ifo.getEntryPosition(entryX, entryY, entryZ);
	_ifo.getEntryDirection(entryDirX, entryDirY);

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	_exit    = false;

	CameraMan.reset();

	// Roughly head position
	CameraMan.setPosition(entryX, entryZ + 2.0, entryY);
	CameraMan.setOrientation(entryDirX, entryDirY);
	CameraMan.update();
}

void Module::enterArea() {
}

void Module::run() {
	enter();
	_running = true;

	EventMan.enableKeyRepeat();

	try {

		EventMan.flushEvents();

		while (!EventMan.quitRequested() && !_exit) {
			replaceModule();
			enterArea();
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

void Module::handleEvents() {
	Events::Event event;
	while (EventMan.pollEvent(event)) {
		// Camera
		if (handleCameraInput(event))
			continue;
	}

	CameraMan.update();
}

void Module::unload() {
	unloadAreas();
	unloadModule();
}

void Module::unloadModule() {
	_tag.clear();

	_ifo.unload();

	ResMan.undo(_resModule);

	_newModule.clear();
	_hasModule = false;
}

void Module::loadAreas() {
}

void Module::unloadAreas() {
}

void Module::movePC(float x, float y, float z) {
	// Roughly head position
	CameraMan.setPosition(x, y + 2.0, z);
	CameraMan.update();
}

const Common::UString &Module::getName() const {
	return _ifo.getName().getString();
}

const Aurora::IFOFile &Module::getIFO() const {
	return _ifo;
}

} // End of namespace Witcher

} // End of namespace Engines
