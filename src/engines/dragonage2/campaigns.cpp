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
 *  The context managing and running the Dragon Age II campaigns.
 */

#include "src/common/error.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"

#include "src/graphics/camera.h"

#include "src/aurora/resman.h"

#include "src/events/events.h"

#include "src/engines/aurora/console.h"
#include "src/engines/aurora/camera.h"

#include "src/engines/dragonage2/game.h"
#include "src/engines/dragonage2/campaigns.h"
#include "src/engines/dragonage2/campaign.h"
#include "src/engines/dragonage2/creature.h"

namespace Engines {

namespace DragonAge2 {

Campaigns::Campaigns(::Engines::Console &console, Game &game) : _console(&console), _game(&game),
	_hasCampaign(false), _running(false), _exit(true), _currentCampaign(0) {

	findCampaigns();
}

Campaigns::~Campaigns() {
	try {
		clean();
	} catch (...) {
	}
}

void Campaigns::clean() {
	if (_currentCampaign)
		_currentCampaign->unload();
}

const Campaigns::PlayableCampaigns &Campaigns::getCampaigns() const {
	return _campaigns;
}

const Campaigns::AddinContent &Campaigns::getAddins() const {
	return _addins;
}

const Campaign *Campaigns::findCampaign(const Common::UString &uid) const {
	for (PlayableCampaigns::const_iterator c = _campaigns.begin(); c != _campaigns.end(); ++c)
		if ((*c)->getUID().equalsIgnoreCase(uid))
			return *c;

	return 0;
}

const Campaign *Campaigns::findAddin(const Common::UString &uid) const {
	for (AddinContent::const_iterator a = _addins.begin(); a != _addins.end(); ++a)
		if ((*a)->getUID().equalsIgnoreCase(uid))
			return *a;

	return 0;
}

Campaign *Campaigns::getCampaign(const Common::UString &uid) {
	for (PlayableCampaigns::iterator c = _campaigns.begin(); c != _campaigns.end(); ++c)
		if ((*c)->getUID().equalsIgnoreCase(uid))
			return *c;

	return 0;
}

Campaign *Campaigns::getAddin(const Common::UString &uid) {
	for (AddinContent::iterator a = _addins.begin(); a != _addins.end(); ++a)
		if ((*a)->getUID().equalsIgnoreCase(uid))
			return *a;

	return 0;
}

bool Campaigns::isLoaded() const {
	return _hasCampaign && _currentCampaign && _currentCampaign->isLoaded();
}

bool Campaigns::isRunning() const {
	return !EventMan.quitRequested() && _running && !_exit && _currentCampaign && _currentCampaign->isLoaded();
}

void Campaigns::findCampaigns() {
	status("Looking for campaigns...");

	const Common::UString baseDir = ResMan.getDataBase();

	// Single player

	const Common::UString  spDir = Common::FilePath::findSubDirectory(baseDir, "modules/campaign_base", true);
	const Common::FileList spFiles(spDir, -1);
	addCampaign(readCampaign(spFiles.findFirst("campaign_base.cif", true)));

	// DLCs

	const Common::UString dlcDir = Common::FilePath::findSubDirectory(baseDir, "addins", true);

	std::list<Common::UString> dlcDirs;
	Common::FilePath::getSubDirectories(dlcDir, dlcDirs);

	for (std::list<Common::UString>::const_iterator d = dlcDirs.begin(); d != dlcDirs.end(); ++d) {
		const Common::FileList dlcFiles(*d);
		const Common::FileList moduleFiles(Common::FilePath::findSubDirectory(*d, "module", true));

		const Common::UString cifFile      = moduleFiles.findFirstGlob(".*\\.cif", true);
		const Common::UString manifestFile = dlcFiles.findFirst("/manifest.xml", true);

		const Common::UString addinBase = Common::FilePath::relativize(dlcDir, *d);

		addCampaign(readCampaign(cifFile, manifestFile, addinBase));
	}
}

void Campaigns::addCampaign(Campaign *campaign) {
	if (!campaign || !campaign->isEnabled() || campaign->getUID().empty()) {
		delete campaign;
		return;
	}

	if (campaign->getExtendsUID().empty())
		_campaigns.push_back(campaign);
	else
		_addins.push_back(campaign);
}

Campaign *Campaigns::readCampaign(const Common::UString &cifPath, const Common::UString &manifestPath,
                                  const Common::UString &addinBase) {
	if (cifPath.empty())
		return 0;

	Campaign *campaign = 0;

	try {

		campaign = new Campaign(*_game, cifPath, manifestPath, addinBase);

	} catch (...) {
		Common::exceptionDispatcherWarning("Failed reading campaign \"%s\"",
		                                   Common::FilePath::getStem(cifPath).c_str());
	}

	return campaign;
}

void Campaigns::load(const Campaign &campaign) {
	if (isRunning()) {
		// We are currently running a campaign. Schedule a safe change instead

		changeCampaign(campaign);
		return;
	}

	// We are not currently running a campaign. Directly load the new campaign
	loadCampaign(campaign);
}

void Campaigns::exit() {
	_exit = true;
}

void Campaigns::unload() {
	unload(true);
}

void Campaigns::unload(bool completeUnload) {
	if (_currentCampaign)
		_currentCampaign->unload();

	_currentCampaign = 0;

	_hasCampaign = false;
	_running     = false;
	_exit        = true;

	_newCampaign.clear();

	if (completeUnload)
		unloadPC();

	_eventQueue.clear();
}

void Campaigns::unloadPC() {
	_pc.reset();
}

void Campaigns::loadCampaign(const Campaign &campaign) {
	unload(false);

	try {
		_currentCampaign = getCampaign(campaign.getUID());
		if (!_currentCampaign)
			throw Common::Exception("Campaign does not exist in this context");

		_currentCampaign->load();

	} catch (Common::Exception &e) {
		unload(false);

		e.add("Failed loading campaign \"%s\" (\"%s\")",
		      campaign.getUID().c_str(), campaign.getName().getString().c_str());
		throw e;
	}

	_hasCampaign = true;
}

void Campaigns::usePC(Creature *pc) {
	_pc.reset(pc);
}

void Campaigns::enter() {
	if (!_hasCampaign)
		throw Common::Exception("Campaigns::enter(): Lacking a campaign?!?");

	if (!_pc)
		throw Common::Exception("Campaigns::enter(): Lacking a PC?!?");

	_currentCampaign->enter(*_pc);

	_running = true;
	_exit    = false;
}

void Campaigns::leave() {
	if (_currentCampaign)
		_currentCampaign->leave();

	_running = false;
	_exit    = true;
}

void Campaigns::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Campaigns::processEventQueue() {
	if (!isRunning())
		return;

	replaceCampaign();

	if (!isRunning())
		return;

	handleEvents();
}

void Campaigns::handleEvents() {
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

		if (_currentCampaign)
			_currentCampaign->addEvent(*event);
	}

	_eventQueue.clear();

	CameraMan.update();

		if (_currentCampaign)
			_currentCampaign->processEventQueue();
}

Campaign *Campaigns::getCurrentCampaign() const {
	return _currentCampaign;
}

Creature *Campaigns::getPC() const {
	return _pc.get();
}

void Campaigns::changeCampaign(const Campaign &campaign) {
	_newCampaign = campaign.getUID();
}

void Campaigns::replaceCampaign() {
	if (_newCampaign.empty())
		return;

	Campaign *campaign = getCampaign(_newCampaign);
	_newCampaign.clear();

	if (!campaign)
		return;

	loadCampaign(*campaign);
	enter();
}

void Campaigns::refreshLocalized() {
}

} // End of namespace DragonAge2

} // End of namespace Engines
