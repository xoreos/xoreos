/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and Bioware corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef COMMON_FILEMAP_H
#define COMMON_FILEMAP_H

#include <string>
#include <list>

#include "boost/filesystem.hpp"

#include "types.h"

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
	uint32 getSize() const;

	/** Copy the names of the files in the FileList into a list.
	 *
	 *  @param list The list into which to copy the file names.
	 */
	void getFileNames(std::list<std::string> &list) const;

	/** Add a directory to the list
	 *
	 *  @param  directory The directory to add.
	 *  @param  recurseDepth The number of levels to recurse into subdirectories. 0
	 *          for ignoring subdirectories, -1 for a limitless recursion.
	 *  @return true if the directory was successfully added to the list,
	 *          false otherwise.
	 */
	bool addDirectory(const std::string &directory, int recurseDepth = 0);

	/** Add the files matching the given regex into another FileList.
	 *
	 *  @param  glob A perl regular expression to match the file names against.
	 *  @param  subMap The FileList to where to add the matching files.
	 *  @return true if at least one matching file was found.
	 */
	bool getSubMap(const std::string &glob, FileList &subMap) const;

	/** Add the files matching the given regex into a list of file names.
	 *
	 *  @param  glob A perl regular expression to match the file names against.
	 *  @param  list The list to where to add the matching file names.
	 *  @return true if at least one matching file was found.
	 */
	bool getSubMap(const std::string &glob, std::list<std::string> &list) const;

	/** Does the list contain this file?
	 *
	 *  @param  fileName The file too look for.
	 *  @param  caseInsensitive Should the case of the file name be ignored?
	 *  @return true if the file is in the list, false otherwise.
	 */
	bool constains(const std::string &fileName, bool caseInsensitive = false) const;

	/** Open the specified file.
	 *
	 *  @param  fileName the file to open.
	 *  @param  caseInsensitive Should the case of the file name be ignored?
	 *  @return A SeekableReadStream of the file, or 0 if the file is not
	 *          in the list.
	 */
	SeekableReadStream *openFile(const std::string &fileName, bool caseInsensitive = false) const;

private:
	/** A file path. */
	struct FilePath {
		std::string baseDir;              ///< The base directory from which the path was added.
		boost::filesystem::path filePath; ///< The complete real path.

		FilePath(std::string b, boost::filesystem::path p);
		FilePath(const FilePath &p);
	};

	std::list<FilePath> _files; ///< The files.

	bool addDirectory(const std::string &base, const boost::filesystem::path &directory, int recurseDepth);
};

} // End of namespace Common

#endif // COMMON_FILEMAP_H
