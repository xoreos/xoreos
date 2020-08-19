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
 *  Star Wars: Knights of the Old Republic installation version detection.
 */

#ifndef ENGINES_KOTOR_VERSION_H
#define ENGINES_KOTOR_VERSION_H

#include "src/common/types.h"

#include "src/aurora/types.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace KotOR {

class Version {
public:
	Version(Aurora::Platform platform);
	~Version();

	/** Try to detect the version of the KotOR installation in this directory. */
	bool detect(const Common::UString &directory);

	/** Did we detect a version? */
	bool hasVersion() const;

	Aurora::Platform getPlatform() const;

	Common::UString getPlatformName() const;

	uint16_t getVersionMajor() const; ///< Return the major version.
	uint16_t getVersionMinor() const; ///< Return the minor version.
	uint32_t getVersionBuild() const; ///< Return the build number.

	Common::UString getVersionString() const;

	bool isTooOld() const; ///< Is this version older than the optimum?
	bool isTooNew() const; ///< Is this version newer than the optimum?

	uint16_t getOptimumVersionMajor() const; ///< Return the optimum major version.
	uint16_t getOptimumVersionMinor() const; ///< Return the optimum minor version.

	Common::UString getOptimumVersionString();

private:
	Aurora::Platform _platform;

	uint16_t _versionMajor; ///< v1.03.514078 ->      1
	uint16_t _versionMinor; ///< v1.03.514078 ->      3
	uint32_t _versionBuild; ///< v1.03.514078 -> 514078

	bool detectWindows(const Common::UString &directory);
	bool detectMacOSX (const Common::UString &directory);
	bool detectXbox   (const Common::UString &directory);

	byte *readFile(const Common::UString &directory, const Common::UString &file, size_t &size);
	byte *readFile(const Common::UString &path, size_t &size);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_VERSION_H
