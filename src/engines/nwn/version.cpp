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

#include <algorithm>
#include <memory>

#include "src/common/ustring.h"
#include "src/common/readfile.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"

#include "src/aurora/util.h"

#include "src/engines/nwn/version.h"


#define MAKE_NWN_VERSION(MAJOR, MINOR, BUILD) ((((uint64_t)(MAJOR)) << 32) + \
                                               (((uint64_t)(MINOR)) << 16) + \
                                               (((uint64_t)(BUILD))))

static const byte kVersionWin[23] = {
	'\0','F','\0','i','\0','l','\0','e','\0','V','\0','e','\0','r','\0','s','\0','i','\0','o','\0','n','\0'
};

static const byte kBuildWin[25] = {
	'\0','P','\0','r','\0','i','\0','v','\0','a','\0','t','\0','e','\0','B','\0','u','\0','i','\0','l','\0','d','\0'
};

static const byte kVersionUnix[20] = {
	'\0','N','e','v','e','r','w','i','n','t','e','r',' ','N','i','g','h','t','s','\0'
};

namespace Engines {

namespace NWN {

Version::Version(Aurora::Platform platform) : _platform(platform),
	_versionMajor(0), _versionMinor(0), _versionBuild(0), _version(0) {

}

Version::~Version() {
}

bool Version::hasVersion() const {
	return _version != 0;
}

Aurora::Platform Version::getPlatform() const {
	return _platform;
}

Common::UString Version::getPlatformName() const {
	return Aurora::getPlatformDescription(_platform);
}

uint64_t Version::getVersion() const {
	return _version;
}

uint16_t Version::getVersionMajor() const {
	return _versionMajor;
}

uint16_t Version::getVersionMinor() const {
	return _versionMinor;
}

uint16_t Version::getVersionBuild() const {
	return _versionBuild;
}

Common::UString Version::getVersionString() const {
	return Common::UString::format("%d.%d.%d", _versionMajor, _versionMinor, _versionBuild);
}

uint64_t Version::getOptimumVersion() {
	return MAKE_NWN_VERSION(kOptimumVersionMajor, kOptimumVersionMinor, kOptimumVersionBuild);
}

uint16_t Version::getOptimumVersionMajor() {
	return kOptimumVersionMajor;
}

uint16_t Version::getOptimumVersionMinor() {
	return kOptimumVersionMinor;
}

uint16_t Version::getOptimumVersionBuild() {
	return kOptimumVersionBuild;
}

Common::UString Version::getOptimumVersionString() {
	return Common::UString::format("%d.%d.%d",
			kOptimumVersionMajor, kOptimumVersionMinor, kOptimumVersionBuild);
}

bool Version::isTooOld() const {
	return _version < getOptimumVersion();
}

bool Version::isTooNew() const {
	return _version > getOptimumVersion();
}

// Detect the version of the Neverwinter Nights installation by inspecting the binaries.
// Hopefully, the arrangements of the string "Neverwinter Nights", the version and the
// build number stay constant for all versions...
bool Version::detect(const Common::UString &directory) {
	_versionMajor = _versionMinor = _versionBuild = 0;

	bool success = false;

	if      (_platform == Aurora::kPlatformWindows)
		success = detectWindows(directory);
	else if (_platform == Aurora::kPlatformMacOSX)
		success = detectMacOSX(directory);
	else if (_platform == Aurora::kPlatformLinux)
		success = detectLinux(directory);

	_version = MAKE_NWN_VERSION(_versionMajor, _versionMinor, _versionBuild);

	return success;
}

// In the Windows binary, read the FileVersion and PrivateBuild fields
// from the VERSIONINFO resource. They're UTF-16LE strings.
bool Version::detectWindows(const Common::UString &directory) {
	size_t size;
	std::unique_ptr<byte[]> binary(readFile(directory, "/nwmain.exe", size));
	if (!binary)
		return false;

	// Search for the pattern where the version information in the binary starts
	byte *version = std::search(binary.get(), binary.get() + size, kVersionWin, kVersionWin + sizeof(kVersionWin));
	if ((size - (version - binary.get())) < (sizeof(kVersionWin) + 24))
		return false;

	// Search for the pattern where the build information in the binary starts
	byte *build = std::search(binary.get(), binary.get() + size, kBuildWin, kBuildWin + sizeof(kBuildWin));
	if ((size - (build - binary.get())) < (sizeof(kBuildWin) + 10))
		return false;

	bool success = false;

	// Check that the field separators are there and the upper bytes of UTF-16 character are 0
	if ((version[sizeof(kVersionWin) +  0] == 0) &&
	    (version[sizeof(kVersionWin) +  1] == 0) &&
	    (version[sizeof(kVersionWin) +  2] == 0) &&
	    (version[sizeof(kVersionWin) +  3] == 0) &&
	    (version[sizeof(kVersionWin) +  5] == 0) &&
	    (version[sizeof(kVersionWin) +  7] == 0) &&
	    (version[sizeof(kVersionWin) +  9] == 0) &&
	    (version[sizeof(kVersionWin) + 11] == 0) &&
	    (version[sizeof(kVersionWin) + 13] == 0) &&
	    (version[sizeof(kVersionWin) + 17] == 0) &&
	    (version[sizeof(kVersionWin) + 21] == 0) &&
	    (version[sizeof(kVersionWin) + 23] == 0) &&
	    (build  [sizeof(kBuildWin  ) +  0] == 0) &&
	    (build  [sizeof(kBuildWin  ) +  1] == 0) &&
	    (build  [sizeof(kBuildWin  ) +  3] == 0) &&
	    (build  [sizeof(kBuildWin  ) +  5] == 0) &&
	    (build  [sizeof(kBuildWin  ) +  7] == 0) &&
	    (build  [sizeof(kBuildWin  ) +  9] == 0)  &&
	    (version[sizeof(kVersionWin) +  6] == ',') &&
	    (version[sizeof(kVersionWin) + 12] == ',') &&
	    (version[sizeof(kVersionWin) + 18] == ',') &&
	    (version[sizeof(kVersionWin) +  8] == ' ') &&
	    (version[sizeof(kVersionWin) + 14] == ' ') &&
	    (version[sizeof(kVersionWin) + 20] == ' ')) {

		// Check that the version strings are numeric
		if (isdigit(version[sizeof(kVersionWin) +  4]) &&
		    isdigit(version[sizeof(kVersionWin) + 10]) &&
		    isdigit(version[sizeof(kVersionWin) + 16]) &&
		    isdigit(build  [sizeof(kBuildWin  ) +  2]) &&
		    isdigit(build  [sizeof(kBuildWin  ) +  4]) &&
		    isdigit(build  [sizeof(kBuildWin  ) +  6]) &&
		    isdigit(build  [sizeof(kBuildWin  ) +  8])) {

			// Read the version information

			_versionMajor  =  version[sizeof(kVersionWin) +  4] - '0';

			_versionMinor  = (version[sizeof(kVersionWin) + 10] - '0') * 10;
			_versionMinor +=  version[sizeof(kVersionWin) + 16] - '0';

			_versionBuild  = (build  [sizeof(kBuildWin  ) +  2] - '0') * 1000;
			_versionBuild += (build  [sizeof(kBuildWin  ) +  4] - '0') *  100;
			_versionBuild += (build  [sizeof(kBuildWin  ) +  6] - '0') *   10;
			_versionBuild +=  build  [sizeof(kBuildWin  ) +  8] - '0';

			success = true;

		}
	}

	return success;
}

// In the Mac OS X binaries, the version number ("1.69") and the build number ("8109")
// precede the name "Neverwinter Nights", each field separated by 4 0-byte.
bool Version::detectMacOSX(const Common::UString &directory) {
	Common::UString appDir =
		Common::FilePath::findSubDirectory(directory, "Neverwinter Nights.app/Contents/MacOS", true);
	if (appDir.empty())
		return false;

	size_t size;
	std::unique_ptr<byte[]> binary(readFile(appDir, "Neverwinter Nights", size));
	if (!binary)
		return false;

	// Search for the pattern where the version information in the binary starts
	byte *version = std::search(binary.get(), binary.get() + size, kVersionUnix, kVersionUnix + sizeof(kVersionUnix));
	if ((version - binary.get()) < 15)
		return false;

	bool success = false;

	// Check that the field separators are there
	if ((version[- 1] == 0) &&
	    (version[- 2] == 0) &&
	    (version[- 3] == 0) &&
	    (version[- 8] == 0) &&
	    (version[- 9] == 0) &&
	    (version[-10] == 0) &&
	    (version[-11] == 0) &&
	    (version[- 6] == '.')) {

		// Check that the version strings are numeric
		if (isdigit(version[- 4]) &&
		    isdigit(version[- 5]) &&
		    isdigit(version[- 7]) &&
		    isdigit(version[-12]) &&
		    isdigit(version[-13]) &&
		    isdigit(version[-14]) &&
		    isdigit(version[-15])) {

			// Read the version information

			_versionMajor  =  version[- 7] - '0';

			_versionMinor  = (version[- 5] - '0') * 10;
			_versionMinor +=  version[- 4] - '0';

			_versionBuild  = (version[-15] - '0') * 1000;
			_versionBuild += (version[-14] - '0') *  100;
			_versionBuild += (version[-13] - '0') *   10;
			_versionBuild +=  version[-12] - '0';

			success = true;

		}
	}

	return success;
}

// In the Linux binaries, the version number ("1.69") and the build number ("8109")
// follow the name "Neverwinter Nights", each field separated by a 0-byte.
bool Version::detectLinux(const Common::UString &directory) {
	size_t size;
	std::unique_ptr<byte[]> binary(readFile(directory, "/nwmain", size));
	if (!binary)
		return false;

	// Search for the pattern where the version information in the binary starts
	byte *version = std::search(binary.get(), binary.get() + size, kVersionUnix, kVersionUnix + sizeof(kVersionUnix));
	if ((size - (version - binary.get())) < (sizeof(kVersionUnix) + 21))
		return false;

	bool success = false;

	// Check that the field separators are there
	if ((version[sizeof(kVersionUnix) +  4] == 0) &&
	    (version[sizeof(kVersionUnix) + 15] == 0) &&
	    (version[sizeof(kVersionUnix) + 20] == 0) &&
	    (version[sizeof(kVersionUnix) + 17] == '.')) {

		// Check that the version strings are numeric
		if (isdigit(version[sizeof(kVersionUnix) +  0]) &&
		    isdigit(version[sizeof(kVersionUnix) +  1]) &&
		    isdigit(version[sizeof(kVersionUnix) +  2]) &&
		    isdigit(version[sizeof(kVersionUnix) +  3]) &&
		    isdigit(version[sizeof(kVersionUnix) + 16]) &&
		    isdigit(version[sizeof(kVersionUnix) + 18]) &&
		    isdigit(version[sizeof(kVersionUnix) + 19])) {

			// Read the version information

			_versionMajor  =  version[sizeof(kVersionUnix) + 16] - '0';

			_versionMinor  = (version[sizeof(kVersionUnix) + 18] - '0') * 10;
			_versionMinor +=  version[sizeof(kVersionUnix) + 19] - '0';

			_versionBuild  = (version[sizeof(kVersionUnix) +  0] - '0') * 1000;
			_versionBuild += (version[sizeof(kVersionUnix) +  1] - '0') *  100;
			_versionBuild += (version[sizeof(kVersionUnix) +  2] - '0') *   10;
			_versionBuild +=  version[sizeof(kVersionUnix) +  3] - '0';

			success = true;

		}
	}

	return success;
}

byte *Version::readFile(const Common::UString &directory, const Common::UString &file, size_t &size) {
	Common::FileList files;

	if (!files.addDirectory(directory))
		return 0;

	return readFile(files.findFirst(file, true), size);
}

byte *Version::readFile(const Common::UString &path, size_t &size) {
	if (path.empty())
		return 0;

	Common::ReadFile file;
	if (!file.open(path))
		return 0;

	size = file.size();

	std::unique_ptr<byte[]> buffer = std::make_unique<byte[]>(size);
	if (file.read(buffer.get(), size) != size)
		return 0;

	return buffer.release();
}

} // End of namespace NWN

} // End of namespace Engines
