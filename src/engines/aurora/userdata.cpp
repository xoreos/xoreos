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
 *  Utility class for managing user data, for example save games.
 */

#include "src/common/filepath.h"
#include "src/common/configman.h"

#include "src/engines/aurora/userdata.h"

namespace Engines {

UserData::UserData(const Common::UString &id) {
	// If a directory for the user data is given over command line, we use it.
	if (ConfigMan.hasKey("userdata"))
		_rootPath = ConfigMan.getString("userdata");
	else
		_rootPath = Common::FilePath::getUserDataDirectory() + "/saves/" + id;

	// If the user data directory was not created yet, create it.
	if (!Common::FilePath::isDirectory(_rootPath))
		Common::FilePath::createDirectories(_rootPath);
}

UserData::~UserData() {
}

} // End of namespace Engines
