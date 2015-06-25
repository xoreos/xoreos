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
 *  The context needed to run a NWN2 module.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"
#include "src/aurora/talkman.h"

#include "src/graphics/camera.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/camera.h"
#include "src/engines/aurora/console.h"

#include "src/engines/nwn2/module.h"
#include "src/engines/nwn2/campaign.h"
#include "src/engines/nwn2/area.h"


namespace Engines {

namespace NWN2 {

Module::Module(::Engines::Console &console, Campaign *campaign) :
	_console(&console), _campaign(campaign),
	_hasModule(false), _running(false), _exit(false), _currentArea(0) {

}

Module::~Module() {
	try {
		clear();
	} catch (...) {
	}
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
		indexMandatoryArchive(module, 1001, &_resModule);

		_ifo.load();

		if (_ifo.isSave())
			throw Common::Exception("This is a save");

		checkXPs();
		checkHAKs();

		_tag = _ifo.getTag();

	} catch (Common::Exception &e) {
		e.add("Can't load module \"%s\"", module.c_str());
		throw e;
	}

	_newModule.clear();

	_hasModule = true;
}

void Module::checkXPs() {
	uint16 hasXP = 0;

	hasXP |= ConfigMan.getBool("NWN2_hasXP1") ? 1 : 0;
	hasXP |= ConfigMan.getBool("NWN2_hasXP2") ? 2 : 0;
	hasXP |= ConfigMan.getBool("NWN2_hasXP3") ? 4 : 0;

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

void Module::changeModule(const Common::UString &module) {
	_newModule = module;
}

void Module::replaceModule() {
	// Look if a campaign replacement was scheduled
	if (_campaign)
		_campaign->replaceCampaign();

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
	if (!_hasModule)
		throw Common::Exception("Module::enter(): Lacking a module?!?");

	_console->printf("Entering module \"%s\"", _ifo.getName().getString().c_str());

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

	Common::UString startMovie = _ifo.getStartMovie();
	if (!startMovie.empty())
		playVideo(startMovie);

	_exit    = false;
	_newArea = _ifo.getEntryArea();

	CameraMan.reset();

	const float entryAngle = -Common::rad2deg(atan2(entryDirX, entryDirY));

	// Roughly head position
	CameraMan.setPosition(entryX, entryY, entryZ + 1.8f);
	CameraMan.setOrientation(90.0f, 0.0f, entryAngle);
	CameraMan.update();
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

	_console->printf("Entering area \"%s\" (\"%s\", \"%s\")", _currentArea->getResRef().c_str(),
			_currentArea->getName().c_str(), _currentArea->getDisplayName().c_str());
}

void Module::run() {
	enter();
	_running = true;

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
			if (handleCameraInput(event))
				continue;

		_currentArea->addEvent(event);
	}

	CameraMan.update();
	_currentArea->processEventQueue();
}

void Module::unload() {
	unloadAreas();
	unloadHAKs();
	unloadTLK();
	unloadModule();
}

void Module::unloadModule() {
	_tag.clear();

	_ifo.unload();

	deindexResources(_resModule);

	_newModule.clear();
	_hasModule = false;
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
		indexMandatoryArchive(haks[i] + ".hak", 1002 + i, &_resHAKs[i]);
}

void Module::unloadHAKs() {
	std::vector<Common::ChangeID>::iterator hak;
	for (hak = _resHAKs.begin(); hak != _resHAKs.end(); ++hak)
		deindexResources(*hak);

	_resHAKs.clear();
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
	for (AreaMap::iterator a = _areas.begin(); a != _areas.end(); ++a)
		delete a->second;

	_areas.clear();
	_newArea.clear();

	_currentArea = 0;
}

void Module::movePC(const Common::UString &area) {
	_newArea = area;
}

void Module::movePC(float x, float y, float z) {
	// Roughly head position
	CameraMan.setPosition(x, y, z + 1.8f);
	CameraMan.update();
}

void Module::movePC(const Common::UString &area, float x, float y, float z) {
	movePC(area);
	movePC(x, y, z);
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

} // End of namespace NWN2

} // End of namespace Engines
