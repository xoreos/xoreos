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

#ifndef XOREOS_USERDATA_H
#define XOREOS_USERDATA_H

#include "src/aurora/nfofile.h"

#include "src/graphics/aurora/texturehandle.h"

#include "src/engines/aurora/userdata.h"

namespace Engines {

namespace KotOR {

class UserData : Engines::UserData {
public:
	UserData();

	/** Get the save count for the game. */
	size_t getNumSaves();

	// .--- Save reading
	/** Get the directory of this specifc save. */
	Common::UString getSaveDir(size_t id); // TODO: This method is only for compatibility purposes and should be replaced by stream retrieving.
	/** Create a stream which reads the metadata. */
	Common::SeekableReadStream *createMetadataReadStream(size_t id);
	/** Get the Id of the save. */
	Common::UString getSaveId(size_t id);
	/** Get the texture used as screenshot. */
	Graphics::Aurora::TextureHandle getScreenshotTexture(size_t id);
	// '---

	// .--- Save writing
	/** Generate a new folder for a new save game and return the index from it. */
	size_t createSave(bool autosave = false);
	// '---

private:
	unsigned int _maxSaveId;
	unsigned int _maxGameId;

	std::vector<Common::UString> _saves;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif //XOREOS_USERDATA_H
