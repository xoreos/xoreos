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
 *  The context holding an Neverwinter Nights 2 campaign.
 */

#include <cassert>

#include <boost/scope_exit.hpp>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/readfile.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"

#include "src/aurora/resman.h"
#include "src/aurora/language.h"
#include "src/aurora/gff3file.h"

#include "src/graphics/camera.h"

#include "src/events/events.h"

#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/console.h"
#include "src/engines/aurora/camera.h"

#include "src/engines/nwn2/campaign.h"
#include "src/engines/nwn2/module.h"
#include "src/engines/nwn2/creature.h"

namespace Engines {

namespace NWN2 {

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

const Common::UString &Campaign::getName() const {
	return _name;
}

const Common::UString &Campaign::getDescription() const {
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

	clearVariables();

	if (completeUnload)
		unloadPC();

	deindexResources(_resCampaign);
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

void Campaign::usePC(const Common::UString &bic, bool local) {
	unloadPC();

	if (bic.empty())
		throw Common::Exception("Tried to load an empty PC");

	try {
		_pc.reset(new Creature(bic, local));
	} catch (Common::Exception &e) {
		e.add("Can't load PC \"%s\"", bic.c_str());
		throw e;
	}

	LangMan.setCurrentGender(_pc->isFemale() ? Aurora::kLanguageGenderFemale : Aurora::kLanguageGenderMale);
}

void Campaign::exit() {
	_exit = true;
}

void Campaign::loadCampaignResource(const Common::UString &campaign) {
	const Common::UString directory = getDirectory(campaign, true);
	if (directory.empty())
		throw Common::Exception("No such campaign \"%s\"", campaign.c_str());

	bool success = false;
	BOOST_SCOPE_EXIT( (&success) (this_) ) {
		if (!success)
			this_->clear();
	} BOOST_SCOPE_EXIT_END

	indexMandatoryDirectory(directory, 0, -1, 1000, &_resCampaign);

	Common::ScopedPtr<Aurora::GFF3File> gff;
	try {
		gff.reset(new Aurora::GFF3File("campaign", Aurora::kFileTypeCAM, MKTAG('C', 'A', 'M', ' ')));
	} catch (Common::Exception &e) {
		e.add("Failed to load campaign information file");
		throw;
	}

	if (!gff->getTopLevel().hasField("ModNames") || !gff->getTopLevel().hasField("StartModule"))
		throw Common::Exception("Campaign information file is missing modules");

	_startModule = gff->getTopLevel().getString("StartModule") + ".mod";

	const Aurora::GFF3List &modules = gff->getTopLevel().getList("ModNames");
	for (Aurora::GFF3List::const_iterator m = modules.begin(); m != modules.end(); ++m)
		_modules.push_back((*m)->getString("ModuleName") + ".mod");

	_name        = gff->getTopLevel().getString("DisplayName");
	_description = gff->getTopLevel().getString("Description");

	success = true;
}

void Campaign::setupStandaloneModule(const Common::UString &module) {
	_modules.push_back(module);

	_startModule = module;
}

void Campaign::loadCampaign(const Common::UString &campaign, bool standalone) {
	unload(false);

	if (!standalone)
		loadCampaignResource(campaign);
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

Common::UString Campaign::getDirectory(const Common::UString &campaign, bool relative) {
	const Common::UString campaignsDir = ConfigMan.getString("NWN2_campaignDir");
	const Common::UString campaignDir  = Common::FilePath::findSubDirectory(campaignsDir, campaign, true);

	if (!relative)
		return campaignDir;

	return Common::FilePath::relativize(ResMan.getDataBase(), campaignDir);
}

Common::UString Campaign::getName(const Common::UString &campaign) {
	try {
		const Common::FileList camFiles(getDirectory(campaign, false));
		const Common::UString  camFile (camFiles.findFirst("campaign.cam", true));

		Aurora::GFF3File cam(new Common::ReadFile(camFile), MKTAG('C', 'A', 'M', ' '));

		return cam.getTopLevel().getString("DisplayName");

	} catch (...) {
	}

	return "";
}

Common::UString Campaign::getDescription(const Common::UString &campaign) {
	try {
		const Common::FileList camFiles(getDirectory(campaign, false));
		const Common::UString  camFile (camFiles.findFirst("campaign.cam", true));

		Aurora::GFF3File cam(new Common::ReadFile(camFile), MKTAG('C', 'A', 'M', ' '));

		return cam.getTopLevel().getString("Description");

	} catch (...) {
	}

	return "";
}

} // End of namespace NWN2

} // End of namespace Engines
