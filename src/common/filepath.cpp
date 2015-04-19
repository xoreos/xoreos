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
 *  Utility class for manipulating file paths.
 */

// Necessary to query for APPDATA and USERPROFILE on Windows
#if defined(WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

// Necessary to search the passwd file
#if defined(UNIX)
	#include <pwd.h>
	#include <unistd.h>
#endif

#include <cstdlib>

#include <list>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "src/common/filepath.h"
#include "src/common/util.h"
#include "src/common/encoding.h"

// boost-filesystem stuff
using boost::filesystem::path;
using boost::filesystem::exists;
using boost::filesystem::is_regular_file;
using boost::filesystem::is_directory;
using boost::filesystem::file_size;
using boost::filesystem::directory_iterator;
using boost::filesystem::create_directories;

// boost-string_algo
using boost::equals;
using boost::iequals;

namespace Common {

bool FilePath::isRegularFile(const UString &p) {
	return (exists(p.c_str()) && is_regular_file(p.c_str()));
}

bool FilePath::isDirectory(const UString &p) {
	return (exists(p.c_str()) && is_directory(p.c_str()));
}

uint32 FilePath::getFileSize(const UString &p) {
	uintmax_t size = file_size(p.c_str());

	if (size == ((uintmax_t) -1))
		return kFileInvalid;

	return size;
}

UString FilePath::getFile(const UString &p) {
	path file(p.c_str());

	return file.filename().string();
}

UString FilePath::getStem(const UString &p) {
	path file(p.c_str());

	return file.stem().string();
}

UString FilePath::getExtension(const UString &p) {
	path file(p.c_str());

	return file.extension().string();
}

UString FilePath::changeExtension(const UString &p, const UString &ext) {
	path file(p.c_str());

	file.replace_extension(ext.c_str());

	return file.string();
}

UString FilePath::getDirectory(const UString &p) {
	path file(p.c_str());

	return file.parent_path().string();
}

bool FilePath::isAbsolute(const UString &p) {
	return boost::filesystem::path(p.c_str()).is_absolute();
}

static path convertToSlash(const path &p) {
	const boost::regex bSlash("\\\\");
	const std::string fSlash("/");
	const boost::match_flag_type flags(boost::match_default | boost::format_sed);

	return path(boost::regex_replace(p.string(), bSlash, fSlash, flags));
}

UString FilePath::absolutize(const UString &p) {
	path source = convertToSlash(p.c_str()).native();

	// Resolve ~ as the user's home directory
	path::iterator itr = source.begin();
	if ((itr != source.end()) && (itr->string() == "~")) {
		path newSource = getHomeDirectory().c_str();

		for (++itr; itr != source.end(); ++itr)
			newSource /= *itr;

		source = newSource;
	}

	return convertToSlash(boost::filesystem::absolute(source)).string().c_str();
}

UString FilePath::relativize(const UString &basePath, const UString &path) {
	UString relative = "";

	if (path.beginsWith(basePath)) {
		relative = path.substr(path.getPosition(basePath.size() + 1), path.end());
	}

	return relative;
}

UString FilePath::normalize(const UString &p, bool resolveSymLinks) {
	boost::filesystem::path source = convertToSlash(p.c_str()).native();
	boost::filesystem::path result;

	// Resolve ~ as the user's home directory
	boost::filesystem::path::iterator itr = source.begin();
	if ((itr != source.end()) && (itr->string() == "~")) {
		boost::filesystem::path newSource = getHomeDirectory().c_str();

		for (++itr; itr != source.end(); ++itr)
			newSource /= *itr;

		source = newSource;
	}

	bool scan = true;
	while (scan) {
		scan = false;
		result.clear();

		for (itr = source.begin(); itr != source.end(); ++itr) {
			// Resolve .
			if (itr->string() == ".")
				continue;

			// Resolve .., but only if symbolic links are also resolved
			if (resolveSymLinks && (itr->string() == "..")) {
				if (result != source.root_path())
					result.remove_filename();
				continue;
			}

			result /= *itr;

			/* If the resolving of symbolic links was requested, check if the current path
			 * is one. If it is, resolve it and restart normalization.
			 */

			if (!resolveSymLinks)
				continue;

			boost::system::error_code ec;
			boost::filesystem::path link = boost::filesystem::read_symlink(result, ec);

			if (!link.empty()) {
				result.remove_filename();

				if (link.is_absolute()) {
					for (++itr; itr != source.end(); ++itr)
						link /= *itr;

					source = link;

				} else {
					boost::filesystem::path newSource = result;

					newSource /= link;

					for (++itr; itr != source.end(); ++itr)
						newSource /= *itr;

					source = newSource;
				}

				scan = true;
				break;
			}
		}
	}

	if (!result.has_root_directory())
		result = boost::filesystem::absolute(result);

	return convertToSlash(result).string().c_str();
}

UString FilePath::canonicalize(const UString &p, bool resolveSymLinks) {
	return normalize(absolutize(p), resolveSymLinks);
}

bool FilePath::getSubDirectories(const UString &directory, std::list<UString> &subDirectories) {
	path dirPath(directory.c_str());

	try {
		// Iterate over the directory's contents
		directory_iterator itEnd;
		for (directory_iterator itDir(dirPath); itDir != itEnd; ++itDir) {
			if (is_directory(itDir->status())) {
				subDirectories.push_back(itDir->path().generic_string());
			}
		}
	} catch (...) {
		return false;
	}

	return true;
}

static void splitDirectories(const UString &directory, std::list<UString> &dirs) {
	UString curDir;

	for (UString::iterator it = directory.begin(); it != directory.end(); ++it) {
		uint32 c = *it;

		if (c == '/') {
			// Found a directory separator, split here

			// Got a real directory, add it to our list
			if (!curDir.empty())
				dirs.push_back(curDir);

			curDir.clear();

		} else
			// Otherwise, just append the current character
			curDir += c;

	}

	// Got trailing data, add it to our list
	if (!curDir.empty())
		dirs.push_back(curDir);
}

static UString findSubDirectory_internal(const UString &directory, const UString &subDirectory,
		bool caseInsensitive) {

	try {
		path dirPath(directory.c_str());
		path subDirPath(subDirectory.c_str());

		// Iterator over the directory's contents
		directory_iterator itEnd;
		for (directory_iterator itDir(dirPath); itDir != itEnd; ++itDir) {
			if (is_directory(itDir->status())) {
				// It's a directory. Check if it's the one we're looking for

				if (caseInsensitive) {
					if (iequals(itDir->path().filename().string(), subDirectory.c_str()))
						return itDir->path().generic_string();
				} else {
					if (equals(itDir->path().filename().string(), subDirectory.c_str()))
						return itDir->path().generic_string();
				}
			}
		}
	} catch (...) {
	}

	return "";
}

UString FilePath::findSubDirectory(const UString &directory, const UString &subDirectory,
		bool caseInsensitive) {

	if (!exists(directory.c_str()) || !is_directory(directory.c_str()))
		// Path is either no directory or doesn't exist
		return "";

	if (subDirectory.empty())
		// Subdirectory to look for is empty, return the directory instead
		return directory;

	// Split the subDirectory string into actual directories
	std::list<UString> dirs;
	splitDirectories(subDirectory, dirs);

	// Iterate over the directory list to find each successive subdirectory
	UString curDir = directory;
	for (std::list<UString>::iterator it = dirs.begin(); it != dirs.end(); ++it)
		if ((curDir = findSubDirectory_internal(curDir, *it, caseInsensitive)).empty())
			return "";

	return curDir;
}

bool FilePath::createDirectories(const UString &path) {
	return create_directories(path.c_str());
}

UString FilePath::escapeStringLiteral(const UString &str) {
	const boost::regex esc("[\\^\\.\\$\\|\\(\\)\\[\\]\\*\\+\\?\\/\\\\]");
	const std::string  rep("\\\\\\1&");

	return boost::regex_replace(std::string(str.c_str()), esc, rep, boost::match_default | boost::format_sed);
}

#if defined(WIN32)
/** Map the Windows version mess to one simple integer,
 *  which is hopefully enough for what we care about...
 */
static inline uint32 getWindowsVersion() {
	OSVERSIONINFO win32OsVersion;
	ZeroMemory(&win32OsVersion, sizeof(OSVERSIONINFO));
	win32OsVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&win32OsVersion))
		return 0;

	if (win32OsVersion.dwPlatformId == VER_PLATFORM_WIN32s) {
		// Windows 3.1
		return 1;
	} else if (win32OsVersion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		// Windows 95 and 98
		return (win32OsVersion.dwMinorVersion == 0) ? 2 : 3;
	} else if (win32OsVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		// Windows NT
		if (win32OsVersion.dwMajorVersion < 5)
			return 4;

		// Windows 2000, XP, ...
		return win32OsVersion.dwMajorVersion;
	} else
		// Unknown
		return 0;
}

static inline UString getWindowsVariable(const wchar_t *variable) {
	DWORD length = GetEnvironmentVariableW(variable, 0, 0);
	if (!length)
		return "";

	uint32 size = length * sizeof(wchar_t);
	byte  *data = new byte[size];

	DWORD newLength = GetEnvironmentVariableW(variable, (wchar_t *) data, length);
	if (!newLength || (newLength > length)) {
		delete[] data;
		return "";
	}

	UString value = readString(data, size, kEncodingUTF16LE);
	delete[] data;

	return value;
}
#endif // WIN32

UString FilePath::getHomeDirectory() {
	UString directory;

#if defined(WIN32)
	// Windows: $USERPROFILE

	directory = getWindowsVariable(L"USERPROFILE");

#elif defined(UNIX)
	// Default Unixoid: $HOME. As a fallback, search the passwd file

	const char *pathStr = getenv("HOME");
	if (!pathStr) {
		struct passwd *pwd = getpwuid(getuid());
		if (pwd)
			pathStr = pwd->pw_dir;
	}

	if (pathStr)
		directory = pathStr;

#endif

	return directory;
}

UString FilePath::getConfigDirectory() {
	UString directory;

#if defined(WIN32)
	// Windows: $APPDATA/xoreos/ or $USERPROFILE/Application Data/xoreos/ or ./

	uint32 windowsVersion = getWindowsVersion();
	if (windowsVersion == 0)
		// If we can't determine the version, just try everything...
		windowsVersion = 5;

	// Try the Application Data directory
	if (windowsVersion >= 5) {
		directory = getWindowsVariable(L"APPDATA");
		if (!directory.empty())
			directory += "\\xoreos";
	}

	// Try the User Profile directory and create our own Application Data directory
	if ((windowsVersion >= 4) && directory.empty()) {
		directory = getWindowsVariable(L"USERPROFILE");
		if (!directory.empty())
			directory += "\\Application Data\\xoreos";
	}

	// If all else fails (or the Windows version is too low), use the current directory
	if (directory.empty())
		directory = ".";

#elif defined(MACOSX)
	// Mac OS X: ~/Library/Preferences/xoreos/

	directory = getHomeDirectory();
	if (!directory.empty())
		directory += "/Library/Preferences/xoreos";

	if (directory.empty())
		directory = ".";

#elif defined(UNIX)
	// Default Unixoid: $XDG_CONFIG_HOME/xoreos/ or ~/.config/xoreos/

	const char *pathStr = getenv("XDG_CONFIG_HOME");
	if (pathStr) {
		directory = UString(pathStr) + "/xoreos";
	} else {
		directory = getHomeDirectory();
		if (!directory.empty())
			directory += "/.config/xoreos";
	}

	if (directory.empty())
		directory = ".";

#else
	// Fallback: Current directory

	directory = ".";
#endif

	return canonicalize(directory);
}

UString FilePath::getUserDataDirectory() {
	UString directory;

#if defined(WIN32)
	// Windows: Same as getConfigDirectory()
	directory = getConfigDirectory();
#elif defined(MACOSX)
	// Mac OS X: ~/Library/Application\ Support/xoreos/

	directory = getHomeDirectory();
	if (!directory.empty())
		directory += "/Library/Application Support/xoreos";

	if (directory.empty())
		directory = ".";

#elif defined(UNIX)
	// Default Unixoid: $XDG_DATA_HOME/xoreos/ or ~/.local/share/xoreos/

	const char *pathStr = getenv("XDG_DATA_HOME");
	if (pathStr) {
		directory = UString(pathStr) + "/xoreos";
	} else {
		directory = getHomeDirectory();
		if (!directory.empty())
			directory += "/.local/share/xoreos";
	}

	if (directory.empty())
		directory = ".";

#else
	// Fallback: Same as getConfigDirectory()
	directory = getConfigDirectory();
#endif

	return canonicalize(directory);
}

} // End of namespace Common
