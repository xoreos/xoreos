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
 *  The context holding a The Witcher campaign.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/readfile.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"

#include "src/aurora/resman.h"
#include "src/aurora/gff3file.h"

#include "src/graphics/camera.h"

#include "src/events/events.h"

#include "src/engines/aurora/console.h"
#include "src/engines/aurora/camera.h"

#include "src/engines/witcher/campaign.h"
#include "src/engines/witcher/module.h"
#include "src/engines/witcher/creature.h"

namespace Engines {

namespace Witcher {

Campaign::Campaign(::Engines::Console &console) : _console(&console),
	_hasCampaign(false), _running(false), _exit(true), _newCampaignStandalone(false) {

	_module.reset(new Module(*_console));
}

Campaign::~Campaign() {
	try {
		clear();
	} catch (...) {
	}
}

void Campaign::clear() {
	unload(true);
}

const Aurora::LocString &Campaign::getName() const {
	return _name;
}

const Aurora::LocString &Campaign::getDescription() const {
	return _description;
}

Module &Campaign::getModule() {
	assert(_module);

	return *_module;
}

bool Campaign::isLoaded() const {
	return _hasCampaign && _module->isLoaded() && _pc;
}

bool Campaign::isRunning() const {
	return !EventMan.quitRequested() && _running && !_exit && _module->isRunning();
}

void Campaign::unload(bool completeUnload) {
	_module->clear();

	_hasCampaign = false;
	_running     = false;
	_exit        = true;

	_name.clear();
	_description.clear();

	_modules.clear();
	_startModule.clear();

	_newCampaign.clear();
	_newCampaignStandalone = false;

	_eventQueue.clear();

	if (completeUnload)
		unloadPC();
}

void Campaign::unloadPC() {
	_pc.reset();
}

void Campaign::load(const Common::UString &campaign) {
	if (isRunning()) {
		// We are currently running a campaign. Schedule a safe change instead

		changeCampaign(campaign, false);
		return;
	}

	// We are not currently running a campaign. Directly load the new campaign
	loadCampaign(campaign, false);
}

void Campaign::loadModule(const Common::UString &module) {
	if (isRunning()) {
		// We are currently running a campaign. Schedule a safe change instead

		changeCampaign(module, true);
		return;
	}

	// We are not currently running a campaign. Directly load the new campaign
	loadCampaign(module, true);
}

Common::SeekableReadStream *Campaign::openMMD(const Common::UString &campaign) {
	const Common::FileList mmdFiles(ConfigMan.getString("WITCHER_moduleDir"), -1);

	for (Common::FileList::const_iterator c = mmdFiles.begin(); c != mmdFiles.end(); ++c) {
		if (!Common::FilePath::getFile(*c).equalsIgnoreCase(campaign + ".mmd"))
			continue;

		return new Common::ReadFile(*c);
	}

	throw Common::Exception("No such campaign \"%s\"", campaign.c_str());
}

Common::UString Campaign::getDirectory(const Common::UString &campaign) {
	const Common::FileList mmdFiles(ConfigMan.getString("WITCHER_moduleDir"), -1);

	for (Common::FileList::const_iterator c = mmdFiles.begin(); c != mmdFiles.end(); ++c) {
		if (!Common::FilePath::getFile(*c).equalsIgnoreCase(campaign + ".mmd"))
			continue;

		return Common::FilePath::relativize(ResMan.getDataBase(), Common::FilePath::getDirectory(*c));
	}

	throw Common::Exception("No such campaign \"%s\"", campaign.c_str());
}

void Campaign::loadCampaignFile(const Common::UString &campaign) {
	try {
		Aurora::GFF3File mmd(openMMD(campaign), MKTAG('M', 'M', 'D', ' '));

		mmd.getTopLevel().getLocString("Meta_Name", _name);
		mmd.getTopLevel().getLocString("Meta_Desc", _description);

		_startModule = mmd.getTopLevel().getString("StartingMod");
		if (_startModule.empty())
			throw Common::Exception("No starting module");

		const Aurora::GFF3List &modules = mmd.getTopLevel().getList("Meta_Mod_list");
		for (Aurora::GFF3List::const_iterator m = modules.begin(); m != modules.end(); ++m)
			_modules.push_back((*m)->getString("Mod_Name"));

		_startModule = getDirectory(campaign) + "/" + _startModule + ".mod";

	} catch (Common::Exception &e) {
		e.add("Failed to load campaign \"%s\" (\"%s\")", campaign.c_str(), _description.getString().c_str());

		throw;
	}
}

void Campaign::setupStandaloneModule(const Common::UString &module) {
	const Common::UString mod = Module::findModule(module, true);
	if (mod.empty())
		throw Common::Exception("No such module \"%s\"", module.c_str());

	_modules.push_back(mod);
	_startModule = mod;
}

void Campaign::loadCampaign(const Common::UString &campaign, bool standalone) {
	unload(false);

	if (!standalone)
		loadCampaignFile(campaign);
	else
		setupStandaloneModule(campaign);

	try {
		_module->load(_startModule);
	} catch (Common::Exception &e) {
		clear();

		e.add("Failed to load campaign's starting module");
		throw;
	}

	_hasCampaign = true;
}

void Campaign::usePC(const Common::UString &utc) {
	unloadPC();

	if (utc.empty())
		throw Common::Exception("Tried to load an empty PC");

	try {
		_pc.reset(new Creature(utc));
	} catch (Common::Exception &e) {
		e.add("Can't load PC \"%s\"", utc.c_str());
		throw e;
	}
}

void Campaign::exit() {
	_exit = true;
}

void Campaign::enter() {
	if (!_hasCampaign)
		throw Common::Exception("Campaign::enter(): Lacking a campaign?!?");

	if (!_pc)
		throw Common::Exception("Campaign::enter(): Lacking a PC?!?");

	_pc->clearVariables();
	_module->enter(*_pc);

	_running = true;
	_exit    = false;
}

void Campaign::leave() {
	_module->leave();

	_running = false;
	_exit    = true;
}

void Campaign::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Campaign::processEventQueue() {
	if (!isRunning())
		return;

	replaceCampaign();

	if (!isRunning())
		return;

	handleEvents();
}

void Campaign::handleEvents() {
	for (EventQueue::const_iterator event = _eventQueue.begin(); event != _eventQueue.end(); ++event) {
		// Handle console
		if (_console->isVisible()) {
			_console->processEvent(*event);
			continue;
		}

		if (event->type == Events::kEventKeyDown) {
			// Console
			if ((event->key.keysym.sym == SDLK_d) && (event->key.keysym.mod & KMOD_CTRL)) {
				_console->show();
				continue;
			}
		}

		// Camera
		if (handleCameraInput(*event))
			continue;

		_module->addEvent(*event);
	}

	_eventQueue.clear();

	CameraMan.update();

	_module->processEventQueue();
}

void Campaign::changeCampaign(const Common::UString &campaign, bool standalone) {
	_newCampaign           = campaign;
	_newCampaignStandalone = standalone;
}

void Campaign::replaceCampaign() {
	if (_newCampaign.empty())
		return;

	const Common::UString campaign = _newCampaign;
	const bool standalone = _newCampaignStandalone;

	loadCampaign(campaign, standalone);
	enter();
}

void Campaign::refreshLocalized() {
	_module->refreshLocalized();
}

Common::UString Campaign::getName(const Common::UString &campaign) {
	try {
		Aurora::GFF3File mmd(openMMD(campaign), MKTAG('M', 'M', 'D', ' '));

		return mmd.getTopLevel().getString("Meta_Name");

	} catch (...) {
	}

	return "";
}

Common::UString Campaign::getDescription(const Common::UString &campaign) {
	try {
		Aurora::GFF3File mmd(openMMD(campaign), MKTAG('M', 'M', 'D', ' '));

		return mmd.getTopLevel().getString("Meta_Desc");

	} catch (...) {
	}

	return "";
}

} // End of namespace Witcher

} // End of namespace Engines
