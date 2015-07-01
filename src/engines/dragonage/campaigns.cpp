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
 *  The context managing and running the Dragon Age: Origins campaigns.
 */

#include "src/common/error.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"

#include "src/aurora/resman.h"

#include "src/events/events.h"

#include "src/engines/dragonage/dragonage.h"
#include "src/engines/dragonage/campaigns.h"
#include "src/engines/dragonage/campaign.h"
#include "src/engines/dragonage/console.h"

namespace Engines {

namespace DragonAge {

Campaigns::Campaigns(::Engines::Console &console, DragonAgeEngine &engine) :
	_console(&console), _engine(&engine), _currentCampaign(0), _running(false) {

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

	for (PlayableCampaigns::iterator c = _campaigns.begin(); c != _campaigns.end(); ++c)
		delete *c;

	for (AddinContent::iterator a = _addins.begin(); a != _addins.end(); ++a)
		delete *a;
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

void Campaigns::findCampaigns() {
	status("Looking for campaigns...");

	const Common::UString baseDir = ResMan.getDataBase();

	// Single player

	const Common::UString  spDir = Common::FilePath::findSubDirectory(baseDir, "modules/single player", true);
	const Common::FileList spFiles(spDir, -1);
	addCampaign(readCampaign(spFiles.findFirst("singleplayer.cif", true)));

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

		campaign = new Campaign(*_engine, cifPath, manifestPath, addinBase);

	} catch (Common::Exception &e) {
		e.add("Failed reading campaign \"%s\"", Common::FilePath::getStem(cifPath).c_str());
		Common::printException(e, "WARNING: ");
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

void Campaigns::unload() {
	if (_currentCampaign)
		_currentCampaign->unload();

	_currentCampaign = 0;

	_newCampaign.clear();
}

void Campaigns::loadCampaign(const Campaign &campaign) {
	unload();

	try {
		_currentCampaign = getCampaign(campaign.getUID());
		if (!_currentCampaign)
			throw Common::Exception("Campaign does not exist in this context");

		_currentCampaign->load();

	} catch (Common::Exception &e) {
		unload();

		e.add("Failed loading campaign \"%s\" (\"%s\")",
		      campaign.getUID().c_str(), campaign.getName().getString().c_str());
		throw e;
	}
}

void Campaigns::run() {
	if (!_currentCampaign)
		return;

	_currentCampaign->enter();
	_running = true;

	EventMan.enableKeyRepeat();

	try {

		EventMan.flushEvents();

		while (!EventMan.quitRequested()) {
			replaceCampaign();
			if (!_currentCampaign)
				break;

			if (!_currentCampaign->changeArea())
				break;

			handleEvents();

			if (!EventMan.quitRequested())
				EventMan.delay(10);
		}

	} catch (Common::Exception &e) {
		_running = false;

		e.add("Failing running campaign \"%s\" (\"%s\")",
				_currentCampaign ? _currentCampaign->getUID().c_str() : "",
				_currentCampaign ? _currentCampaign->getName().getString().c_str() : "");

		throw e;
	}

	_running = false;
}

void Campaigns::handleEvents() {
	Events::Event event;
	while (EventMan.pollEvent(event)) {
		// Handle console
		if (_console->isVisible()) {
			_console->processEvent(event);
			continue;
		}

		if (event.type == Events::kEventKeyDown) {
			// Console
			if ((event.key.keysym.sym == SDLK_d) && (event.key.keysym.mod & KMOD_CTRL)) {
				_console->show();
				continue;
			}
		}

		if (_currentCampaign) {
			_currentCampaign->addEvent(event);
			continue;
		}
	}

	if (_currentCampaign)
		_currentCampaign->processEventQueue();
}

bool Campaigns::isRunning() const {
	return _running;
}

Campaign *Campaigns::getCurrentCampaign() const {
	return _currentCampaign;
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

	if (_currentCampaign)
		_currentCampaign->enter();
}

void Campaigns::refreshLocalized() {
}

} // End of namespace DragonAge

} // End of namespace Engines
