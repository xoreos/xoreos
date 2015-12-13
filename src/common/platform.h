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
 *  Platform-dependant functions, mostly for internal use in the Common namespace.
 */

#ifndef COMMON_PLATFORM_H
#define COMMON_PLATFORM_H

#include <cstdio>

#include <vector>

#include "src/common/ustring.h"

namespace Common {

class Platform {
public:
	enum FileMode {
		kFileModeRead = 0,
		kFileModeWrite   ,

		kFileModeMAX
	};

	/** Initialize platform-dependant things. */
	static void init();

	/** Read the command line parameters into UTF-8 strings. */
	static void getParameters(int argc, char **argv, std::vector<UString> &args);

	/** Open a file with an UTF-8 encoded name. */
	static std::FILE *openFile(const UString &fileName, FileMode mode);

	/** Return the OS-specific path of the user's home directory. */
	static UString getHomeDirectory();
	/** Return the OS-specific path of the config directory. */
	static UString getConfigDirectory();
	/** Return the OS-specific path of the user data directory. */
	static UString getUserDataDirectory();
};

} // End of namespace Common

#endif // COMMON_PLATFORM_H
