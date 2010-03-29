/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef COMMON_FILELIST_H
#define COMMON_FILELIST_H

#include <string>
#include <list>
#include <map>

#include "boost/filesystem.hpp"

#include "common/types.h"

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
	 *  @param  subList The FileList to where to add the matching files.
	 *  @param  caseInsensitive Should the case of the file name be ignored?
	 *  @return true if at least one matching file was found.
	 */
	bool getSubList(const std::string &glob, FileList &subList, bool caseInsensitive = false) const;

	/** Add the files matching the given regex into a list of file names.
	 *
	 *  @param  glob A perl regular expression to match the file names against.
	 *  @param  list The list to where to add the matching file names.
	 *  @param  caseInsensitive Should the case of the file name be ignored?
	 *  @return true if at least one matching file was found.
	 */
	bool getSubList(const std::string &glob, std::list<std::string> &list, bool caseInsensitive = false) const;

	/** Does the list contain the specified file?
	 *
	 *  @param  fileName The file to look for.
	 *  @return true if the file is in the list, fale if not.
	 */
	bool contains(const std::string &fileName) const;

	/** Does the list contain at least one file matching the given regex?
	 *
	 *  @param  glob A perl regular expression to match the file names against.
	 *  @param  caseInsensitive Should the case of the file name be ignored?
	 *  @return true if at least one matching file is found, false otherwise.
	 */
	bool contains(const std::string &glob, bool caseInsensitive) const;

	/** Open the specified file.
	 *
	 *  @param  fileName The file to open.
	 *  @return A SeekableReadStream of the file, or 0 if the file not
	 *          in the list.
	 */
	SeekableReadStream *openFile(const std::string &fileName) const;

	/** Open the first file matching the given regex.
	 *
	 *  @param  glob A perl regular expression to match the file names against.
	 *  @param  caseInsensitive Should the case of the file name be ignored?
	 *  @return A SeekableReadStream of the file, or 0 if such a file is not
	 *          in the list.
	 */
	SeekableReadStream *openFile(const std::string &glob, bool caseInsensitive) const;

private:
	/** A file path. */
	struct FilePath {
		std::string baseDir;              ///< The base directory from which the path was added.
		boost::filesystem::path filePath; ///< The complete real path.

		FilePath(std::string b, boost::filesystem::path p);
		FilePath(const FilePath &p);
	};

	typedef std::multimap<std::string, std::list<FilePath>::const_iterator> FileMap;

	std::list<FilePath> _files; ///< The files.

	/** The files mapped by extensionless, lowercase filename. */
	FileMap _fileMap;

	bool addDirectory(const std::string &base, const boost::filesystem::path &directory, int recurseDepth);

	void addPath(const std::string &base, const boost::filesystem::path &p);
	void addPath(const FilePath &p);

	const FilePath *getPath(const std::string &fileName) const;
	const FilePath *getPath(const std::string &glob, bool caseInsensitive) const;

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
		const std::string &operator*() const;
		const std::string *operator->() const;
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
