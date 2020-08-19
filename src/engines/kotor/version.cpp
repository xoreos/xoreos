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

#include <algorithm>
#include <memory>

#include "src/common/ustring.h"
#include "src/common/readfile.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"

#include "src/aurora/util.h"

#include "src/engines/kotor/version.h"

namespace Engines {

namespace KotOR {

/* Latest version numbers by platform:
 * - Windows: 1.03.514077
 * - MacOS X: 1.03.514078
 * - Xbox:    1.00.545501
 */

Version::Version(Aurora::Platform platform) : _platform(platform),
	_versionMajor(0), _versionMinor(0), _versionBuild(0) {

}

Version::~Version() {
}

bool Version::hasVersion() const {
	return _versionMajor != 0 || _versionMinor != 0 || _versionBuild != 0;
}

Aurora::Platform Version::getPlatform() const {
	return _platform;
}

Common::UString Version::getPlatformName() const {
	return Aurora::getPlatformDescription(_platform);
}

uint16_t Version::getVersionMajor() const {
	return _versionMajor;
}

uint16_t Version::getVersionMinor() const {
	return _versionMinor;
}

uint32_t Version::getVersionBuild() const {
	return _versionBuild;
}

Common::UString Version::getVersionString() const {
	return Common::UString::format("%u.%02u.%u", _versionMajor, _versionMinor, _versionBuild);
}

uint16_t Version::getOptimumVersionMajor() const {
	switch (_platform) {
		case Aurora::kPlatformWindows:
		case Aurora::kPlatformMacOSX:
		case Aurora::kPlatformXbox:
			return 1;

		default:
			break;
	}

	return 0;
}

uint16_t Version::getOptimumVersionMinor() const {
	switch (_platform) {
		case Aurora::kPlatformWindows:
		case Aurora::kPlatformMacOSX:
			return 3;

		case Aurora::kPlatformXbox:
			return 0;

		default:
			break;
	}

	return 0;
}

Common::UString Version::getOptimumVersionString() {
	return Common::UString::format("%u.%02u",
			getOptimumVersionMajor(), getOptimumVersionMinor());
}

static uint32_t makeCombinedVersion(uint32_t major, uint32_t minor) {
	return (major << 16) + minor;
}

bool Version::isTooOld() const {
	return makeCombinedVersion(_versionMajor,            _versionMinor) <
	       makeCombinedVersion(getOptimumVersionMajor(), getOptimumVersionMinor());
}

bool Version::isTooNew() const {
	return makeCombinedVersion(_versionMajor,            _versionMinor) >
	       makeCombinedVersion(getOptimumVersionMajor(), getOptimumVersionMinor());
}

// Detect the version of the KotOR installation by inspecting the binaries
bool Version::detect(const Common::UString &directory) {
	_versionMajor = _versionMinor = _versionBuild = 0;

	bool success = false;

	if      (_platform == Aurora::kPlatformWindows)
		success = detectWindows(directory);
	else if (_platform == Aurora::kPlatformMacOSX)
		success = detectMacOSX(directory);
	else if (_platform == Aurora::kPlatformXbox)
		success = detectXbox(directory);

	if (!success)
		_versionMajor = _versionMinor = _versionBuild = 0;

	return success;
}

// In the Windows binary, read the FileVersion and PrivateBuild fields
// from the VERSIONINFO resource. They're UTF-16LE strings.
bool Version::detectWindows(const Common::UString &directory) {
	static const byte kVersionWin[23] = {
		'\0','F','\0','i','\0','l','\0','e','\0','V','\0','e','\0','r','\0','s','\0','i','\0','o','\0','n','\0'
	};

	static const byte kBuildWin[25] = {
		'\0','P','\0','r','\0','i','\0','v','\0','a','\0','t','\0','e','\0','B','\0','u','\0','i','\0','l','\0','d','\0'
	};

	size_t size;
	std::unique_ptr<byte[]> binary(readFile(directory, "/swkotor.exe", size));
	if (!binary)
		return false;

	// Search for the pattern where the version information in the binary starts
	byte *version = std::search(binary.get(), binary.get() + size, kVersionWin, kVersionWin + sizeof(kVersionWin));
	if ((size - (version - binary.get())) < (sizeof(kVersionWin) + 24))
		return false;

	// Search for the pattern where the build information in the binary starts
	byte *build = std::search(binary.get(), binary.get() + size, kBuildWin, kBuildWin + sizeof(kBuildWin));
	if ((size - (build - binary.get())) < (sizeof(kBuildWin) + 14))
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
	    (build  [sizeof(kBuildWin  ) +  9] == 0) &&
	    (build  [sizeof(kBuildWin  ) + 11] == 0) &&
	    (build  [sizeof(kBuildWin  ) + 13] == 0) &&
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
		    isdigit(build  [sizeof(kBuildWin  ) +  8]) &&
		    isdigit(build  [sizeof(kBuildWin  ) + 10]) &&
		    isdigit(build  [sizeof(kBuildWin  ) + 12])) {

			// Read the version information

			_versionMajor  =  version[sizeof(kVersionWin) +  4] - '0';

			_versionMinor  = (version[sizeof(kVersionWin) + 10] - '0') * 10;
			_versionMinor +=  version[sizeof(kVersionWin) + 16] - '0';

			_versionBuild  = (build  [sizeof(kBuildWin  ) +  2] - '0') * 100000;
			_versionBuild += (build  [sizeof(kBuildWin  ) +  4] - '0') *  10000;
			_versionBuild += (build  [sizeof(kBuildWin  ) +  6] - '0') *   1000;
			_versionBuild += (build  [sizeof(kBuildWin  ) +  8] - '0') *    100;
			_versionBuild += (build  [sizeof(kBuildWin  ) + 10] - '0') *     10;
			_versionBuild +=  build  [sizeof(kBuildWin  ) + 12] - '0';

			success = true;

		}
	}

	return success;
}

// In the Mac OS X binaries, the version number ("1, 0, 3, 0") and the build number ("514078")
// precede the name "Star Wars: Knights of the Old Republic", each field separated by 2 0-byte.
bool Version::detectMacOSX(const Common::UString &directory) {
	static const byte kVersionMac[42] = {
		'\0','\0','S','t','a','r',' ','W','a','r','s',':',' ','K','n','i','g','h','t','s',' ','o','f',' ',
		't','h','e',' ','O','l','d',' ','R','e','p','u','b','l','i','c','\0','\0'
	};

	Common::UString appDir = Common::FilePath::findSubDirectory(directory, "KOTOR.app/Contents/MacOS", true);
	if (appDir.empty())
		appDir = Common::FilePath::findSubDirectory(directory, "Knights of the Old Republic.app/Contents/MacOS", true);
	if (appDir.empty())
		return false;

	size_t size;
	std::unique_ptr<byte[]> binary(readFile(appDir, "XKOTOR", size));
	if (!binary)
		binary.reset(readFile(appDir, "Knights of the Old Republic", size));
	if (!binary)
		return false;

	// Search for the pattern where the version information in the binary starts
	byte *version = std::search(binary.get(), binary.get() + size, kVersionMac, kVersionMac + sizeof(kVersionMac));
	if ((version - binary.get()) < 20)
		return false;

	bool success = false;

	// Check that the field separators are there
	if ((version[-11] == 0) &&
	    (version[-12] == 0) &&
	    (version[-19] == 0) &&
	    (version[- 3] == ',') &&
	    (version[- 6] == ',') &&
	    (version[- 9] == ',') &&
	    (version[- 9] == ',') &&
	    (version[- 2] == ' ') &&
	    (version[- 5] == ' ') &&
	    (version[- 8] == ' ')) {

		// Check that the version strings are numeric
		if (isdigit(version[- 1]) &&
		    isdigit(version[- 4]) &&
		    isdigit(version[- 7]) &&
		    isdigit(version[-10]) &&
		    isdigit(version[-13]) &&
		    isdigit(version[-14]) &&
		    isdigit(version[-15]) &&
		    isdigit(version[-16]) &&
		    isdigit(version[-17]) &&
		    isdigit(version[-18])) {

			// Read the version information

			_versionMajor  =  version[-10] - '0';

			_versionMinor  = (version[- 7] - '0') * 10;
			_versionMinor +=  version[- 4] - '0';

			_versionBuild  = (version[-18] - '0') * 100000;
			_versionBuild += (version[-17] - '0') *  10000;
			_versionBuild += (version[-16] - '0') *   1000;
			_versionBuild += (version[-15] - '0') *    100;
			_versionBuild += (version[-14] - '0') *     10;
			_versionBuild +=  version[-13] - '0';

			success = true;

		}
	}

	return success;
}

// In the Xbox binaries, the version number ("1.00") and the build number ("5455.1")
// preceed the string "Release", separated by a 0-byte.
bool Version::detectXbox(const Common::UString &directory) {
	static const byte kVersionXbox[9] = {
		'\0','R','e','l','e','a','s','e','\0'
	};

	size_t size;
	std::unique_ptr<byte[]> binary(readFile(directory, "/default.xbe", size));
	if (!binary)
		return false;

	// Search for the pattern where the version information in the binary starts
	byte *version = std::search(binary.get(), binary.get() + size, kVersionXbox, kVersionXbox + sizeof(kVersionXbox));
	if ((version - binary.get()) < 13)
		return false;

	bool success = false;

	// Check that the field separators are there
	if ((version[-12] == 0) &&
	    (version[- 2] == '.') &&
	    (version[- 7] == '.') &&
	    (version[-10] == '.')) {

		// Check that the version strings are numeric
		if (isdigit(version[- 1]) &&
		    isdigit(version[- 3]) &&
		    isdigit(version[- 4]) &&
		    isdigit(version[- 5]) &&
		    isdigit(version[- 6]) &&
		    isdigit(version[- 8]) &&
		    isdigit(version[- 9]) &&
		    isdigit(version[-11])) {

			// Read the version information

			_versionMajor  =  version[-11] - '0';

			_versionMinor  = (version[- 9] - '0') * 10;
			_versionMinor +=  version[- 8] - '0';

			_versionBuild  = (version[- 6] - '0') * 100000;
			_versionBuild += (version[- 5] - '0') *  10000;
			_versionBuild += (version[- 4] - '0') *   1000;
			_versionBuild += (version[- 3] - '0') *    100;
			_versionBuild +=  version[- 1] - '0';

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

} // End of namespace KotOR

} // End of namespace Engines
