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

#ifndef COMMON_FILEPATH_H
#define COMMON_FILEPATH_H

#include <list>

#include "src/common/types.h"
#include "src/common/ustring.h"

namespace Common {

static const size_t kFileInvalid = SIZE_MAX;

/** Utility class for manipulating file paths. */
class FilePath {
public:
	/** Does specified path exist and is it a regular file?
	 *
	 *  @param  p The path to look up.
	 *  @return true if the path exists and is a regular file, false otherwise.
	 */
	static bool isRegularFile(const UString &p);

	/** Does specified path exist and is it a directory?
	 *
	 *  @param  p The path to look up.
	 *  @return true if the path exists and is a directory, false otherwise.
	 */
	static bool isDirectory(const UString &p);

	/** Return a file's size.
	 *
	 *  @param  p The file to look up.
	 *  @return The size of the file or kFileInvalid if not a valid file.
	 */
	static size_t getFileSize(const UString &p);

	/** Return a file name without its path.
	 *
	 *  Example: "/path/to/file.ext" > "file.ext"
	 *
	 *  @param  p The path to manipulate.
	 *  @return The path's file name.
	 */
	static UString getFile(const UString &p);

	/** Return a file name's stem.
	 *
	 *  Example: "/path/to/file.ext" -> "file"
	 *
	 *  @param  p The path to manipulate.
	 *  @return The path's stem.
	 */
	static UString getStem(const UString &p);

	/** Return a file name's extension.
	 *
	 *  Example: "/path/to/file.ext" -> ".ext"
	 *
	 *  @param  p The path to manipulate.
	 *  @return The path's extension.
	 */
	static UString getExtension(const UString &p);

	/** Change a file name's extension.
	 *
	 *  Example: "/path/to/file.ext", ".bar" -> "/path/to/file.bar"
	 *
	 *  @param  p The path to manipulate.
	 *  @param  ext The path's new extension.
	 *  @return The new path.
	 */
	static UString changeExtension(const UString &p, const UString &ext = "");

	/** Return a path's directory.
	 *
	 *  Example: "/path/to/file.ext" -> "/path/to/"
	 *
	 *  @param  p The path to manipulate.
	 *  @return The path's directory.
	 */
	static UString getDirectory(const UString &p);

	/** Is the given string an absolute path?
	 *
	 *  @param  p The path to check.
	 *  @return true if the string describes an absolute path, false otherwise.
	 */
	static bool isAbsolute(const UString &p);

	/** Return the absolute path.
	 *
	 *  If the path is already absolute, just return that path. If not, interpret it
	 *  as relative to the program starting path and then return an absolute path of that.
	 *
	 *  In addition, a path starting with a ~ directory will be changed to point to
	 *  the user's home directory.
	 *
	 *  @param  p The path to absolutize.
	 *  @return The absolutized path.
	 */
	static UString absolutize(const UString &p);

	/** Return the path relative to the base path.
	 *
	 *  If the path does not start with the base path, an empty path will be returned;
	 *  @param  basePath The path to make relative to.
	 *  @param  path The path to make relative.
	 *  @return The relative path.
	 */
	static UString relativize(const UString &basePath, const UString &path);

	/** Normalize a path.
	 *
	 *  A normalized path contains no consecutive '/', uses '/' as a directory path separator,
	 *  starts with either "/", "[A-Za-z]:/" or "./" and does not end with a '/'.
	 *
	 *  In addition, "." and "..", symbolic links and ~ are resolved.
	 *
	 *  NOTE: If the resolving of symbolic links is disabled, ".." will not be resolved
	 *        either, as ".." following a symbolic links should modify the destination
	 *        of that symbolic link and not remove it.
	 *
	 *  The effect is very similar to boost::filesystem::canonical(), except:
	 *  - boost::filesystem::canonical() does not resolve ~ to the user's home directory
	 *  - boost::filesystem::canonical() absolutizes a path, normalize() does not
	 *  - boost::filesystem::canonical() fails on paths that don't exist, normalize() does not
	 *
	 *  @param  p The path to normalize.
	 *  @param  resolveSymLinks Should symbolic links be resolved?
	 *  @return The normalized path.
	 */
	static UString normalize(const UString &p, bool resolveSymLinks = true);

	/** Return the canonical, absolutized and normalized path.
	 *
	 *  Calls absolutize() and normalize() on the path.
	 *
	 *  The effect is very similar to boost::filesystem::canonical(), except:
	 *  - boost::filesystem::canonical() does not resolve ~ to the user's home directory
	 *  - boost::filesystem::canonical() fails on paths that don't exist, normalize() does not
	 *
	 *  @param  p The path to canonicalize.
	 *  @param  resolveSymLinks Should symbolic links be resolved?
	 *  @return The canonicalized path.
	 */
	static UString canonicalize(const UString &p, bool resolveSymLinks = true);

	/** Find a directory's subdirectory.
	 *
	 *  @param  directory The directory in which to look.
	 *  @param  subDirectory The subdirectory to find.
	 *  @param  caseInsensitive Should the case of the subdirectory be ignored?
	 *  @return The path of the subdirectory or "" if not found.
	 */
	static UString findSubDirectory(const UString &directory, const UString &subDirectory,
	                                bool caseInsensitive = false);

	/** Collect all direct subdirectories of a directory in a list.
	 *
	 *  For example, if the specified directory contains the directories "foo" and "bar", and
	 *  the file "quux", the list will contain the directories "foo" and "bar".
	 *
	 *  @param  directory The directory in which to look.
	 *  @param  subDirectories The list to add the directories to.
	 *  @return false if the specified path was not a directory or could not be searched;
	 *          true otherwise.
	 */
	static bool getSubDirectories(const UString &directory, std::list<UString> &subDirectories);

	/** Create all directories in this path.
	 *
	 *  For example, if called on the path "/foo/bar/quux/", this will create
	 *  the directories "/foo/", "/foo/bar/" and "/foo/bar/quux/" if necessary.
	 *
	 *  @param  path The path to create.
	 *  @return true if at least one directory was created.
	 */
	static bool createDirectories(const UString &path);

	/** Escape a string literal for use in a regexp. */
	static UString escapeStringLiteral(const UString &str);

	/** Format this file size into a human readable string. */
	static UString getHumanReadableSize(size_t size);

	/** Return the OS-specific path of the user's home directory.
	 *
	 *  If no home directory was accessible, an empty string is returned.
	 */
	static UString getHomeDirectory();

	/** Return the OS-specific path of the config directory.
	 *
	 *  This is where the xoreos.conf is put by default.
	 *
	 *  - On GNU/Linux, this will evaluate to $XDG_CONFIG_HOME/xoreos/
	 *  - On Mac OS X, this will evaluate to $HOME/Library/Preferences/xoreos/
	 *  - On Windows, this will evaluate to either $APPDATA/xoreos/,
	 *    $USERPROFILE/xoreos/ or the current directory,
	 *    depending on the Windows version.
	 */
	static UString getConfigDirectory();

	/** Return the OS-specific path of the user data directory.
	 *
	 *  This is where user data like the log or game saves are put by default.
	 *
	 *  - On GNU/Linux, this will evaluate to $XDG_DATA_HOME/xoreos/
	 *  - On Mac OS X, this will evaluate to $HOME/Library/Application\ Support/xoreos/
	 *  - On Windows, this will be the same place as getConfigDirectory()
	 */
	static UString getUserDataDirectory();

	/** Return a path suitable for writing into.
	 *
	 *  If the file is an absolute path, return it as is.
	 *  Otherwise, prepend the user data directory.
	 */
	static UString getUserDataFile(UString file);
};

} // End of namespace Common

#endif // COMMON_FILEPATH_H
