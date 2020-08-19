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
 *  Neverwinter Nights installation version detection.
 */

#ifndef ENGINES_NWN_VERSION_H
#define ENGINES_NWN_VERSION_H

#include <cstddef>

#include "src/common/types.h"

#include "src/aurora/types.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

class Version {
public:
	Version(Aurora::Platform platform);
	~Version();

	/** Try to detect the version of the NWN installation in this directory. */
	bool detect(const Common::UString &directory);

	/** Did we detect a version? */
	bool hasVersion() const;

	Aurora::Platform getPlatform() const;

	Common::UString getPlatformName() const;

	uint64_t getVersion() const;      ///< Return the combined version.
	uint16_t getVersionMajor() const; ///< Return the major version.
	uint16_t getVersionMinor() const; ///< Return the minor version.
	uint16_t getVersionBuild() const; ///< Return the build number.

	Common::UString getVersionString() const;

	bool isTooOld() const; ///< Is this version older than the optimum?
	bool isTooNew() const; ///< Is this version newer than the optimum?

	static uint64_t getOptimumVersion();      ///< Return the optimum combined version.
	static uint16_t getOptimumVersionMajor(); ///< Return the optimum major version.
	static uint16_t getOptimumVersionMinor(); ///< Return the optimum minor version.
	static uint16_t getOptimumVersionBuild(); ///< Return the optimum build number.

	static Common::UString getOptimumVersionString();

private:
	static const uint16_t kOptimumVersionMajor =    1;
	static const uint16_t kOptimumVersionMinor =   69;
	static const uint16_t kOptimumVersionBuild = 8109;

	Aurora::Platform _platform;

	uint16_t _versionMajor; ///< v1.69.8109 ->    1
	uint16_t _versionMinor; ///< v1.69.8109 ->   69
	uint16_t _versionBuild; ///< v1.69.8109 -> 8109
	uint64_t _version;      ///< The combined version number.

	bool detectWindows(const Common::UString &directory);
	bool detectMacOSX (const Common::UString &directory);
	bool detectLinux  (const Common::UString &directory);

	byte *readFile(const Common::UString &directory, const Common::UString &file, size_t &size);
	byte *readFile(const Common::UString &path, size_t &size);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_VERSION_H
