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
 *  Creation and loading of saved games for KotOR games.
 */

#include "src/common/error.h"
#include <memory>
#include "src/common/filepath.h"
#include "src/common/readfile.h"

#include "src/engines/kotorbase/savedgame.h"
#include "src/engines/kotorbase/gui/chargeninfo.h"

namespace Engines {

namespace KotORBase {

SavedGame::SavedGame(const Common::UString &dir, bool loadSav) :
		_timePlayed(0),
		_pcGender(kGenderMale),
		_pcLoaded(false) {

	load(dir, loadSav);
}

SavedGame::~SavedGame() {
}

void SavedGame::load(const Common::UString &dir, bool loadSav) {
	Common::UString nfoPath(Common::FilePath::normalize(dir + "/savenfo.res"));
	Common::ReadFile *nfoFile = new Common::ReadFile(nfoPath);
	Aurora::GFF3File nfoGff(nfoFile);

	fillFromNFO(nfoGff);

	if (loadSav) {
		Common::UString savPath(Common::FilePath::normalize(dir + "/SAVEGAME.sav"));
		Common::ReadFile *savFile = new Common::ReadFile(savPath);
		Aurora::ERFFile savErf(savFile);
		fillFromSAV(savErf, _moduleName);
	}
}

void SavedGame::fillFromNFO(const Aurora::GFF3File &gff) {
	const Aurora::GFF3Struct &root = gff.getTopLevel();
	_name = root.getString("SAVEGAMENAME");
	_moduleName = root.getString("LASTMODULE");
	_timePlayed = root.getUint("TIMEPLAYED");
}

void SavedGame::fillFromSAV(const Aurora::ERFFile &erf, const Common::UString &moduleName) {
	int moduleSavIndex = -1;

	const Aurora::Archive::ResourceList &resources = erf.getResources();
	for (Aurora::Archive::ResourceList::const_iterator it = resources.begin();
			it != resources.end(); ++it) {
		const Aurora::Archive::Resource &res = *it;
		if (res.name.stricmp(moduleName) == 0 && res.type == Aurora::kFileTypeSAV) {
			moduleSavIndex = res.index;
			break;
		}
	}

	if (moduleSavIndex >= 0) {
		Aurora::ERFFile moduleSav(erf.getResource(moduleSavIndex));
		fillFromModuleSAV(moduleSav);
	} else {
		warning("SAV file not found: %s", moduleName.c_str());
	}
}

void SavedGame::fillFromModuleSAV(const Aurora::ERFFile &erf) {
	int ifoIndex = -1;

	const Aurora::Archive::ResourceList &resources = erf.getResources();
	for (Aurora::Archive::ResourceList::const_iterator it = resources.begin();
			it != resources.end(); ++it) {
		const Aurora::Archive::Resource &res = *it;
		if ((res.name == "Module") && (res.type == Aurora::kFileTypeIFO)) {
			ifoIndex = res.index;
			break;
		}
	}

	if (ifoIndex >= 0) {
		Aurora::GFF3File moduleIfo(erf.getResource(ifoIndex));
		fillFromModuleIFO(moduleIfo);
	} else {
		warning("Module IFO file not found");
	}
}

void SavedGame::fillFromModuleIFO(const Aurora::GFF3File &gff) {
	const Aurora::GFF3List &playerList = gff.getTopLevel().getList("Mod_PlayerList");
	if (!playerList.empty()) {
		const Aurora::GFF3Struct *playerGff = playerList[0];
		_pcGender = playerGff->getUint("Gender");
		_pcPosition[0] = playerGff->getDouble("XPosition");
		_pcPosition[1] = playerGff->getDouble("YPosition");
		_pcPosition[2] = playerGff->getDouble("ZPosition");
		_pcLoaded = true;
	} else {
		warning("Player list not found in module IFO");
	}
}

const Common::UString &SavedGame::getName() const {
	return _name;
}

const Common::UString &SavedGame::getModuleName() const {
	return _moduleName;
}

uint32_t SavedGame::getTimePlayed() const {
	return _timePlayed;
}

bool SavedGame::isPCLoaded() const {
	return _pcLoaded;
}

} // End of namespace KotORBase

} // End of namespace Engines
