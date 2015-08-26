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

#include "src/engines/witcher/campaign.h"
#include "src/engines/witcher/console.h"

namespace Engines {

namespace Witcher {

Campaign::Campaign(::Engines::Console &console) : _running(false),
	_module(console, this) {

}

Campaign::~Campaign() {
	clear();
}

void Campaign::clear() {
	_module.clear();

	_name.clear();
	_description.clear();

	_modules.clear();
	_startModule.clear();

	_newCampaign.clear();
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

	} catch (Common::Exception &e) {
		e.add("Failed to load campaign \"%s\" (\"%s\")", campaign.c_str(), _description.getString().c_str());

		throw;
	}
}

void Campaign::loadCampaign(const Common::UString &campaign) {
	clear();
	loadCampaignFile(campaign);

	try {
		_module.load(getDirectory(campaign) + "/" + _startModule + ".mod");
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

	loadCampaign(campaign);
}

const Aurora::LocString &Campaign::getName() const {
	return _name;
}

const Aurora::LocString &Campaign::getDescription() const {
	return _description;
}

void Campaign::refreshLocalized() {
	_module.refreshLocalized();
}

Module &Campaign::getModule() {
	return _module;
}

Common::UString Campaign::getName(const Common::UString &campaign) {
	try {
		Aurora::GFF3File mmd(openMMD(campaign), MKTAG('M', 'M', 'D', ' '));

		return mmd.getTopLevel().getString("Meta_Name");

	} catch (Common::Exception &e) {
	}

	return "";
}

Common::UString Campaign::getDescription(const Common::UString &campaign) {
	try {
		Aurora::GFF3File mmd(openMMD(campaign), MKTAG('M', 'M', 'D', ' '));

		return mmd.getTopLevel().getString("Meta_Desc");

	} catch (Common::Exception &e) {
	}

	return "";
}

} // End of namespace Witcher

} // End of namespace Engines
