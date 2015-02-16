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

/** @file engines/witcher/campaign.cpp
 *  The context holding a Witcher campaign.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/file.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"
#include "src/common/configman.h"

#include "src/aurora/gfffile.h"

#include "src/engines/witcher/campaign.h"

namespace Engines {

namespace Witcher {

void CampaignDescription::clear() {
	tag.clear();

	name.clear();
	description.clear();

	file.clear();
	directory.clear();
}


Campaign::Campaign() : _running(false) {
	findCampaigns();
}

Campaign::~Campaign() {
	clear();
}

const std::list<CampaignDescription> &Campaign::getCampaigns() const {
	return _campaigns;
}

void Campaign::findCampaigns() {
	Common::UString moduleDir = ConfigMan.getString("WITCHER_moduleDir");
	if (moduleDir.empty())
		return;

	Common::FileList modules;
	if (!modules.addDirectory(moduleDir, -1))
		return;

	Common::FileList mmdFiles;
	if (!modules.getSubListGlob(".*\\.mmd", true, mmdFiles))
		return;

	for (Common::FileList::const_iterator c = mmdFiles.begin(); c != mmdFiles.end(); ++c) {
		CampaignDescription desc;

		desc.directory = Common::FilePath::relativize(moduleDir, Common::FilePath::getDirectory(*c));
		if (!readCampaign(*c, desc))
			continue;

		_campaigns.push_back(desc);
	}
}

bool Campaign::readCampaign(const Common::UString &mmdFile, CampaignDescription &desc) {
	Common::File *file = new Common::File;
	if (!file->open(mmdFile)) {
		delete file;
		return false;
	}

	Aurora::GFFFile *gff = 0;
	try {
		gff = new Aurora::GFFFile(file, MKTAG('M', 'M', 'D', ' '));
	} catch (...) {
		return false;
	}

	gff->getTopLevel().getLocString("Meta_Name", desc.name);
	gff->getTopLevel().getLocString("Meta_Desc", desc.description);

	delete gff;

	desc.file = mmdFile;
	desc.tag  = Common::FilePath::getStem(mmdFile).toLower();

	return true;
}

void Campaign::clear() {
	_currentCampaign.clear();

	_modules.clear();
	_startModule.clear();

	_newCampaign = 0;
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

void Campaign::loadCampaignFile(const CampaignDescription &desc) {
	Common::File    *file = 0;
	Aurora::GFFFile *gff  = 0;
	try {

		try {
			if (desc.file.empty())
				throw Common::Exception("Campaign file is empty");

			file = new Common::File(desc.file);
			gff  = new Aurora::GFFFile(file, MKTAG('M', 'M', 'D', ' '));
		} catch (Common::Exception &e) {
			delete file;
			throw;
		}

		_startModule = gff->getTopLevel().getString("StartingMod");

		const Aurora::GFFList &modules = gff->getTopLevel().getList("Meta_Mod_list");
		for (Aurora::GFFList::const_iterator m = modules.begin(); m != modules.end(); ++m)
			_modules.push_back((*m)->getString("Mod_Name"));

		if (_startModule.empty())
			throw Common::Exception("No starting module");

	} catch (Common::Exception &e) {
		delete gff;

		e.add("Failed to load campaign \"%s\" (\"%s\")", desc.tag.c_str(), desc.name.getString().c_str());
		throw;
	}

	delete gff;
}

void Campaign::loadCampaign(const CampaignDescription &desc) {
	clear();
	loadCampaignFile(desc);

	_currentCampaign = desc;
}

void Campaign::run() {
	_running = true;

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

	loadCampaign(*campaign);
}

const Common::UString &Campaign::getName() const {
	return _currentCampaign.name.getString();
}

const Common::UString &Campaign::getDescription() const {
	return _currentCampaign.description.getString();
}

} // End of namespace Witcher

} // End of namespace Engines
