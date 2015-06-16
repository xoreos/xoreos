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
 *  The context holding an NWN2 campaign.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/readfile.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"

#include "src/aurora/resman.h"
#include "src/aurora/gff3file.h"

#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/console.h"

#include "src/engines/nwn2/campaign.h"

namespace Engines {

namespace NWN2 {

Campaign::Campaign(::Engines::Console &console) : _running(false),
	_module(console, this), _newCampaign(0) {

	findCampaigns();
}

Campaign::~Campaign() {
	try {
		clear();
	} catch (...) {
	}
}

const std::list<CampaignDescription> &Campaign::getCampaigns() const {
	return _campaigns;
}

void Campaign::findCampaigns() {
	Common::UString baseDir = ResMan.getDataBase();

	Common::UString campaignBaseDir = Common::FilePath::findSubDirectory(baseDir, "campaigns", true);
	if (campaignBaseDir.empty())
		return;

	Common::FileList campaignFiles;
	if (!campaignFiles.addDirectory(campaignBaseDir, -1))
		return;

	Common::FileList camFiles;
	if (!campaignFiles.getSubList("campaign.cam", true, camFiles))
		return;

	for (Common::FileList::const_iterator c = camFiles.begin(); c != camFiles.end(); ++c) {
		CampaignDescription desc;

		desc.directory = Common::FilePath::relativize(baseDir, Common::FilePath::getDirectory(*c));
		if (!readCampaign(*c, desc))
			continue;

		_campaigns.push_back(desc);
	}
}

bool Campaign::readCampaign(const Common::UString &camFile, CampaignDescription &desc) {
	Common::ReadFile *file = new Common::ReadFile;
	if (!file->open(camFile)) {
		delete file;
		return false;
	}

	Aurora::GFF3File *gff = 0;
	try {
		gff = new Aurora::GFF3File(file, MKTAG('C', 'A', 'M', ' '));
	} catch (...) {
		return false;
	}

	gff->getTopLevel().getLocString("DisplayName", desc.name);
	gff->getTopLevel().getLocString("Description", desc.description);

	delete gff;

	return true;
}

void Campaign::clear() {
	_module.clear();

	_currentCampaign.directory.clear();
	_currentCampaign.name.clear();
	_currentCampaign.description.clear();

	_modules.clear();
	_startModule.clear();

	_newCampaign = 0;

	deindexResources(_resCampaign);
}

void Campaign::load(const CampaignDescription &desc) {
	if (isRunning()) {
		// We are currently running a campaign. Schedule a safe change instead

		changeCampaign(desc);
		return;
	}

	// We are not currently running a campaign. Directly load the new campaign
	loadCampaign(desc);
}

void Campaign::loadCampaignResource(const CampaignDescription &desc) {
	if (desc.directory.empty())
		throw Common::Exception("Campaign path is empty");

	indexMandatoryDirectory(desc.directory, 0, -1, 1000, &_resCampaign);

	Aurora::GFF3File *gff = 0;
	try {
		gff = new Aurora::GFF3File("campaign", Aurora::kFileTypeCAM, MKTAG('C', 'A', 'M', ' '));
	} catch (Common::Exception &e) {
		clear();

		e.add("Failed to load campaign information file");
		throw;
	}

	if (!gff->getTopLevel().hasField("ModNames") || !gff->getTopLevel().hasField("StartModule")) {
		delete gff;
		clear();

		throw Common::Exception("Campaign information file is missing modules");
	}

	_startModule = gff->getTopLevel().getString("StartModule") + ".mod";

	const Aurora::GFF3List &modules = gff->getTopLevel().getList("ModNames");
	for (Aurora::GFF3List::const_iterator m = modules.begin(); m != modules.end(); ++m)
		_modules.push_back((*m)->getString("ModuleName") + ".mod");

	delete gff;
}

void Campaign::loadCampaign(const CampaignDescription &desc) {
	clear();
	loadCampaignResource(desc);

	_currentCampaign = desc;

	try {
		_module.load(_startModule);
	} catch (Common::Exception &e) {
		clear();

		e.add("Failed to load campaign's starting module");
		throw;
	}
}

void Campaign::run() {
	_running = true;

	try {
		_module.run();
	} catch (...) {
		_running = false;
		throw;
	}

	_running = false;
}

bool Campaign::isRunning() const {
	return _running;
}

void Campaign::changeCampaign(const CampaignDescription &desc) {
	_newCampaign = &desc;
}

void Campaign::replaceCampaign() {
	if (!_newCampaign)
		return;

	const CampaignDescription *campaign = _newCampaign;

	clear();
	loadCampaignResource(*campaign);

	_module.load(_startModule);
}

const Common::UString &Campaign::getName() const {
	return _currentCampaign.name.getString();
}

const Common::UString &Campaign::getDescription() const {
	return _currentCampaign.description.getString();
}

Module *Campaign::getModule() {
	return &_module;
}

} // End of namespace NWN2

} // End of namespace Engines
