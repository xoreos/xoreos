/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/filelist.h
 *  A list of files.
 */

#ifndef COMMON_FILELIST_H
#define COMMON_FILELIST_H

#include <string>
#include <list>
#include <map>

#include <boost/filesystem.hpp>

#include "common/types.h"
#include "common/ustring.h"

namespace Common {

class SeekableReadStream;

/** A list of files. */
class FileList {
public:
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

	/** Copy the names of the files in the FileList into a list.
	 *
	 *  @param list The list into which to copy the file names.
	 */
	uint getFileNames(std::list<UString> &list) const;

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

	/** Open the specified file.
	 *
	 *  @param  fileName The file to open.
	 *  @return A SeekableReadStream of the file, or 0 if the file not
	 *          in the list.
	 */
	SeekableReadStream *openFile(const UString &fileName) const;

	/** Open the first file matching the given regex.
	 *
	 *  @param  glob A perl regular expression to match the file names against.
	 *  @param  caseInsensitive Should the case of the file name be ignored?
	 *  @return A SeekableReadStream of the file, or 0 if such a file is not
	 *          in the list.
	 */
	SeekableReadStream *openFile(const UString &glob, bool caseInsensitive) const;

private:
	/** A file path. */
	struct FilePath {
		UString baseDir;          ///< The base directory from which the path was added.
		UString pathString;       ///< The complete path string form.
		boost::filesystem::path filePath; ///< The complete real path.

		FilePath(UString b, boost::filesystem::path p);
		FilePath(const FilePath &p);
	};

	typedef std::multimap<UString, std::list<FilePath>::const_iterator> FileMap;

	std::list<FilePath> _files; ///< The files.

	/** The files mapped by extensionless, lowercase filename. */
	FileMap _fileMap;

	bool addDirectory(const UString &base, const boost::filesystem::path &directory, int recurseDepth);

	void addPath(const UString &base, const boost::filesystem::path &p);
	void addPath(const FilePath &p);

	const FilePath *getPath(const UString &fileName) const;
	const FilePath *getPath(const UString &glob, bool caseInsensitive) const;

public:
	/** Iterator over all files in a FileList. */
	class const_iterator {
	public:
		const_iterator(const const_iterator &i);
		const_iterator(const std::list<FilePath>::const_iterator &i);

		const_iterator &operator++();
		const_iterator operator++(int);
		const_iterator &operator--();
		const_iterator operator--(int);
		const UString &operator*() const;
		const UString *operator->() const;
		bool operator==(const const_iterator &x) const;
		bool operator!=(const const_iterator &x) const;

	private:
		std::list<FilePath>::const_iterator it;
	};

	const_iterator begin() const;
	const_iterator end() const;
};

} // End of namespace Common

#endif // COMMON_FILELIST_H
