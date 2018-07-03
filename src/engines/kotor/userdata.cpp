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
 *  Utility class for managing save data for kotor data.
 */

#include "src/common/readfile.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"
#include "src/common/strutil.h"

#include "src/aurora/talkman.h"

#include "src/engines/kotor/userdata.h"
#include "src/graphics/aurora/textureman.h"

namespace Engines {

namespace KotOR {

UserData::UserData() : Engines::UserData("kotor"), _maxSaveId(1), _maxGameId(1) {
	Common::FileList fileList;
	fileList.addSubDirectories(_rootPath);
	fileList.getSubListGlob("^[0-9]{9} - (Game[0-9]*|AUTOSAVE)$", false, fileList);

	for (Common::FileList::const_iterator iter = fileList.begin(); iter != fileList.end(); iter++) {
		Common::UString save = *iter;
		if (
			!Common::FilePath::isRegularFile(save + "/savenfo.res") ||
			!Common::FilePath::isRegularFile(save + "/SAVEGAME.sav") ||
			!Common::FilePath::isRegularFile(save + "/PARTYTABLE.res") ||
			!Common::FilePath::isRegularFile(save + "/GLOBALVARS.res") ||
			!Common::FilePath::isRegularFile(save + "/Screen.tga")
		)
			continue;

		_saves.push_back(save);

		save = Common::FilePath::getFile(save);
		Common::UString id = save.substr(save.begin(), save.getPosition(6));
		unsigned int value = 0;
		Common::parseString(id, value);
		_maxSaveId = MAX(_maxSaveId, value);

		if (!save.contains("AUTOSAVE") && !save.contains("QUICKSAVE")) {
			// The file list glob should secure that this substr will be a number.
			id = save.substr(save.getPosition(13), save.end());
			Common::parseString(id, value);
			_maxGameId = MAX(_maxGameId, value);
		}

	}
}

size_t UserData::getNumSaves() {
	return _saves.size();
}

Common::UString UserData::getSaveDir(size_t id) {
	return _saves[id];
}

Common::SeekableReadStream *UserData::createMetadataReadStream(size_t id) {
	return new Common::ReadFile(_saves[id] + "/savenfo.res");
}

Common::UString UserData::getSaveId(size_t id) {
	Common::UString result;
	Common::UString relativeDir(Common::FilePath::getFile(_saves[id]));

	if (relativeDir.contains("QUICKSAVE"))
		result = "Quick Save";
	else if (relativeDir.contains("AUTOSAVE"))
		result = "Auto Save";
	else if (relativeDir.contains("Game")) {
		/* The format of a normal save directory is something like "000043 - Game42".
		 * The original game seems to ignore the GameXX part, and instead parses
		 * the first number, and then substracts 1 from the result.
		 *
		 * I.e. "000062 - Game42" will appears as "Game 61".
		 *
		 * Directories that fail this parsing, for example "abc - Game 42", simply
		 * won't appear in the save list.
		 */

		Common::UString::iterator it = relativeDir.begin();
		while ((it != relativeDir.end()) && (*it == '0'))
			++it;

		Common::UString tmp(it, relativeDir.end());
		tmp.truncate(tmp.findFirst(' '));

		int gameIndex = -1;

		Common::parseString(tmp, gameIndex);
		gameIndex--;

		if (gameIndex < 0)
			throw Common::Exception("Game index is negative (%d)", gameIndex);

		result = TalkMan.getString(48280) + " " + Common::composeString(gameIndex);

	} else
		throw Common::Exception("Unknown save type");

	return result;
}

Graphics::Aurora::TextureHandle UserData::getScreenshotTexture(size_t id) {
	return TextureMan.getFromFile(_saves[id] + "/Screen.tga");
}

size_t UserData::createSave(bool autosave) {
	Common::UString saveId = Common::composeString(_maxSaveId + 1);
	Common::UString gameId = Common::composeString(_maxGameId + 1);

	while (saveId.size() < 6) {
		saveId = "0" + saveId;
	}

	Common::UString save = _rootPath + "/" + saveId + " - ";

	if (autosave)
		save += "AUTOSAVE";
	else
		save += "Game" + gameId;

	Common::FilePath::createDirectories(save);
	_saves.push_back(save);

	return _saves.size() - 1;
}

} // End of namespace KotOR

} // End of namespace Engines
