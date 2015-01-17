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

/** @file engines/nwn2/campaign.cpp
 *  The context holding an NWN2 campaign.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/file.h"
#include "common/filepath.h"
#include "common/filelist.h"

#include "aurora/gfffile.h"

#include "engines/aurora/resources.h"

#include "engines/nwn2/campaign.h"

namespace Engines {

namespace NWN2 {

Campaign::Campaign() {
	findCampaigns();
}

Campaign::~Campaign() {
	clear();
}

const std::list<CampaignDescription> &Campaign::getCampaigns() const {
	return _campaigns;
}

void Campaign::findCampaigns() {
	Common::UString baseDir = ResMan.getDataBaseDir();

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
	Common::File *file = new Common::File;
	if (!file->open(camFile)) {
		delete file;
		return false;
	}

	Aurora::GFFFile *gff = 0;
	try {
		gff = new Aurora::GFFFile(file, MKTAG('C', 'A', 'M', ' '));
	} catch (...) {
		delete gff;
		return false;
	}

	gff->getTopLevel().getLocString("DisplayName", desc.name);
	gff->getTopLevel().getLocString("Description", desc.description);

	delete gff;

	return true;
}

void Campaign::clear() {
	_currentCampaign.directory.clear();
	_currentCampaign.name.clear();
	_currentCampaign.description.clear();

	_modules.clear();
	_startModule.clear();

	ResMan.undo(_resCampaign);
}

void Campaign::loadCampaign(const CampaignDescription &desc) {
	clear();

	if (desc.directory.empty())
		throw Common::Exception("Campaign path is empty");

	indexMandatoryDirectory(desc.directory, 0, -1, 1000, &_resCampaign);

	Aurora::GFFFile *gff = 0;
	try {
		gff = new Aurora::GFFFile("campaign", Aurora::kFileTypeCAM, MKTAG('C', 'A', 'M', ' '));
	} catch (Common::Exception &e) {
		delete gff;
		clear();

		e.add("Failed to load campaign information file");
		throw;
	}

	if (!gff->getTopLevel().hasField("ModNames") || !gff->getTopLevel().hasField("StartModule")) {
		delete gff;
		clear();

		throw Common::Exception("Campaign information file is missing modules");
	}

	_startModule = gff->getTopLevel().getString("StartModule");

	const Aurora::GFFList &modules = gff->getTopLevel().getList("ModNames");
	for (Aurora::GFFList::const_iterator m = modules.begin(); m != modules.end(); ++m)
		_modules.push_back((*m)->getString("ModuleName"));

	delete gff;

	_currentCampaign = desc;
}

const Common::UString &Campaign::getName() const {
	return _currentCampaign.name.getString();
}

const Common::UString &Campaign::getDescription() const {
	return _currentCampaign.description.getString();
}

} // End of namespace NWN2

} // End of namespace Engines
