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

/** @file common/filelist.h
 *  A list of files.
 */

#ifndef COMMON_FILELIST_H
#define COMMON_FILELIST_H

#include <list>

#include "common/ustring.h"

namespace Common {

/** A list of files. */
class FileList {
public:
	typedef std::list<UString>::const_iterator const_iterator;

	FileList();
	FileList(const FileList &list);
	~FileList();

	FileList &operator=(const FileList &list);
	FileList &operator+=(const FileList &list);

	/** Clear the list. */
	void clear();

	/** Is the list empty? */
	bool isEmpty() const;
	/** Return the number of files in the list. */
	uint32 size() const;

	/** Return a const_iterator pointing to the beginning of the list. */
	const_iterator begin() const;
	/** Return a const_iterator pointing past the end of the list. */
	const_iterator end() const;

	/** Add a directory to the list
	 *
	 *  @param  directory The directory to add.
	 *  @param  recurseDepth The number of levels to recurse into subdirectories. 0
	 *          for ignoring subdirectories, -1 for a limitless recursion.
	 *  @return true if the directory was successfully added to the list,
	 *          false otherwise.
	 */
	bool addDirectory(const UString &directory, int recurseDepth = 0);

	/** Add the files matching the given regex into another FileList.
	 *
	 *  @param  glob A perl regular expression to match the file names against.
	 *  @param  subList The FileList to where to add the matching files.
	 *  @param  caseInsensitive Should the case of the file name be ignored?
	 *  @return true if at least one matching file was found.
	 */
	bool getSubList(const UString &glob, FileList &subList, bool caseInsensitive = false) const;

	/** Add the files matching the given regex into a list of file names.
	 *
	 *  @param  glob A perl regular expression to match the file names against.
	 *  @param  list The list to where to add the matching file names.
	 *  @param  caseInsensitive Should the case of the file name be ignored?
	 *  @return true if at least one matching file was found.
	 */
	bool getSubList(const UString &glob, std::list<UString> &list, bool caseInsensitive = false) const;

	/** Does the list contain the specified file?
	 *
	 *  @param  fileName The file to look for.
	 *  @return true if the file is in the list, false if not.
	 */
	bool contains(const UString &fileName) const;

	/** Does the list contain at least one file matching the given regex?
	 *
	 *  @param  glob A perl regular expression to match the file names against.
	 *  @param  caseInsensitive Should the case of the file name be ignored?
	 *  @return true if at least one matching file is found, false otherwise.
	 */
	bool contains(const UString &glob, bool caseInsensitive) const;

	/** Find the first file matching the given regex.
	 *
	 *  @param  glob A perl regular expression to match the file names against.
	 *  @param  caseInsensitive Should the case of the file name be ignored?
	 *  @return The path of the first matching file, or "" if such a file is not
	 *          in the list.
	 */
	UString findFirst(const UString &glob, bool caseInsensitive) const;

private:
	typedef std::list<UString> Files;

	Files _files;

	UString getPath(const UString &fileName) const;
	UString getPath(const UString &glob, bool caseInsensitive) const;
};

} // End of namespace Common

#endif // COMMON_FILELIST_H
