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

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/configman.h"
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

Campaign::Campaign(::Engines::Console &console) : _running(false), _module(console, this) {
}

Campaign::~Campaign() {
	try {
		clear();
	} catch (...) {
	}
}

void Campaign::clear() {
	_module.clear();

	_name.clear();
	_description.clear();

	_modules.clear();
	_startModule.clear();

	_newCampaign.clear();

	deindexResources(_resCampaign);
}

void Campaign::load(const Common::UString &campaign) {
	if (isRunning()) {
		// We are currently running a campaign. Schedule a safe change instead

		changeCampaign(campaign);
		return;
	}

	// We are not currently running a campaign. Directly load the new campaign
	loadCampaign(campaign);
}

void Campaign::loadCampaignResource(const Common::UString &campaign) {
	const Common::UString directory = getDirectory(campaign, true);
	if (directory.empty())
		throw Common::Exception("No such campaign \"%s\"", campaign.c_str());

	indexMandatoryDirectory(directory, 0, -1, 1000, &_resCampaign);

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

	_name        = gff->getTopLevel().getString("DisplayName");
	_description = gff->getTopLevel().getString("Description");

	delete gff;
}

void Campaign::loadCampaign(const Common::UString &campaign) {
	clear();
	loadCampaignResource(campaign);

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

void Campaign::changeCampaign(const Common::UString &campaign) {
	_newCampaign = campaign;
}

void Campaign::replaceCampaign() {
	if (_newCampaign.empty())
		return;

	const Common::UString campaign = _newCampaign;

	clear();
	loadCampaignResource(campaign);

	_module.load(_startModule);
}

const Common::UString &Campaign::getName() const {
	return _name;
}

const Common::UString &Campaign::getDescription() const {
	return _description;
}

Module *Campaign::getModule() {
	return &_module;
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
