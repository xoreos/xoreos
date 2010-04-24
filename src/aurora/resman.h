/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/resman.h
 *  The global resource manager for Aurora resources.
 */

#ifndef AURORA_RESMAN_H
#define AURORA_RESMAN_H

#include <stack>
#include <list>
#include <vector>
#include <map>

#include "common/types.h"
#include "common/ustring.h"
#include "common/singleton.h"
#include "common/filelist.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class Archive;
class KEYFile;
class BIFFile;

/** A resource manager holding information about and handling all request for all
 *  resources useable by the game.
 */
class ResourceManager : public Common::Singleton<ResourceManager> {
// Type definitions
private:
	typedef std::list<Common::UString> DirectoryList;
	typedef std::vector<FileType> FileTypeList;

	typedef std::list<Common::UString> ResFileList;
	typedef ResFileList::const_iterator ResFileRef;

	typedef std::list<Archive *> ArchiveList;
	typedef ArchiveList::const_iterator ArchiveRef;

	/** Where a resource can be found. */
	enum Source {
		kSourceNone   , ///< Invalid source.
		kSourceBIF    , ///< Within a BIF file.
		kSourceZIP    , ///< Within a ZIP file.
		kSourceArchive, ///< Within an archive.
		kSourceFile     ///< A direct file.
	};

	/** A resource. */
	struct Resource {
		FileType type; ///< The resource's type.

		uint32 priority; ///< The resource's priority over others with the same name and type.

		Source source; ///< Where can the resource be found?

		// For kSourceBIF / kSourceZIP
		ResFileRef resFile; ///< Iterator into the BIF/ZIP list.
		uint32 offset;      ///< The offset within the BIF/ZIP file.
		uint32 size;        ///< The size of the resource data.

		// For kSourceArchive
		Archive *archive;      ///< Pointer to the archive.
		uint32   archiveIndex; ///< Index into the archive.

		// For kSourceZIP
		Common::UString path; ///< The file's path.

		Resource();

		bool operator<(const Resource &right) const;
	};

	/** List of resources, sorted by priority. */
	typedef std::list<Resource>                    ResourceList;
	/** Map over resources with the same name but different type. */
	typedef std::map<FileType,    ResourceList>    ResourceTypeMap;
	/** Map over resources, indexed by name. */
	typedef std::map<Common::UString, ResourceTypeMap> ResourceMap;

	struct ResourceChange {
		ResourceMap::iterator     nameIt;
		ResourceTypeMap::iterator typeIt;
		ResourceList::iterator    resIt;
	};

	struct ChangeSet {
		std::list<ResFileList::iterator> bifs;
		std::list<ResFileList::iterator> zips;
		std::list<ArchiveList::iterator> archives;
		std::list<ResourceChange>        resources;
	};

	typedef std::list<ChangeSet> ChangeSetList;

public:
	typedef ChangeSetList::iterator ChangeID;

	ResourceManager();
	~ResourceManager();

	/** Clear all resource information. */
	void clear();

	/** Are .rim files actually ERF files? */
	void setRIMsAreERFs(bool rimsAreERFs);

	/** Register a path to be the base data directory.
	 *
	 *  @param path The path to a base data directory.
	 */
	void registerDataBaseDir(const Common::UString &path);

	/** Add a directory to be searched for these archives files.
	 *
	 *  Relative to the base directory.
	 *
	 *  @param archive The type of archive to look for.
	 *  @param dir A subdirectory of the base directory to search for archives files.
	 */
	void addArchiveDir(ArchiveType archive, const Common::UString &dir);

	/** Add an archive file and all its resources to the resource manager.
	 *
	 *  @param  archive The type of archive to add.
	 *  @param  The name of the archive file to index.
	 *  @param  priority The priority these files have over others of the same name and type.
	 *  @return An ID for all collective changes done by adding the archive file.
	 */
	ChangeID addArchive(ArchiveType archive, const Common::UString &file, uint32 priority = 0);

	/** Add a directory's contents to the resource manager.
	 *
	 *  Relative to the base directory.
	 *
	 *  @param  dir A subdirectory of the base directory to search for resource files.
	 *  @param  glob A pattern of which files to look for.
	 *  @param  depth The number of levels to recurse into subdirectories. 0
	 *                for ignoring subdirectories, -1 for a limitless recursion.
	 *  @param  priority The priority these files have over others of the same name and type.
	 *  @return An ID for all collective changes done by adding the directory.
	 */
	ChangeID addResourceDir(const Common::UString &dir, const char *glob = 0,
	                        int depth = -1, uint32 priority = 100);

	/** Undo the changes done in the specified change ID. */
	void undo(ChangeID &change);

	/** Does a specific resource exists?
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  type The resource's type.
	 *  @return true if the resource exists, false otherwise.
	 */
	bool hasResource(const Common::UString &name, FileType type) const;

	/** Does a specific resource exists?
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  types The resource's types.
	 *  @return true if the resource exists, false otherwise.
	 */
	bool hasResource(const Common::UString &name, const std::vector<FileType> &types) const;

	/** Return a resource.
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  type The resource's type.
	 *  @return The resource stream or 0 if the resource doesn't exist.
	 */
	Common::SeekableReadStream *getResource(const Common::UString &name, FileType type) const;

	/** Return a resource.
	 *
	 *  This only returns one stream, even if more than one of the specified file types exist
	 *  for the given name.
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  types A list of file types to look for.
	 *  @param  foundType If != 0, that's where the actually found type is stored.
	 *  @return The resource stream or 0 if the resource doesn't exist.
	 */
	Common::SeekableReadStream *getResource(const Common::UString &name,
			const std::vector<FileType> &types, FileType *foundType = 0) const;

	/** Return a resource of a specific type.
	 *
	 *  @param  resType The type of the resource.
	 *  @param  name The name (ResRef or path) of the resource.
	 *  @param  type If != 0, that's where the resource's type is stored.
	 *  @return The resource stream or 0 if the music resource doesn't exist.
	 */
	Common::SeekableReadStream *getResource(ResourceType resType,
			const Common::UString &name, FileType *foundType = 0) const;

	/** Small debug method that lists all indexed resources. */
	void listResources() const;

private:
	bool _rimsAreERFs; ///< Are .rim files actually ERF files?

	Common::UString _baseDir;     ///< The data base directory.

	DirectoryList    _archiveDirs [kArchiveMAX]; ///< Archive directories.
	Common::FileList _archiveFiles[kArchiveMAX]; ///< Archive files.

	ResFileList _bifs; ///< List of currently used BIF files.
	ResFileList _zips; ///< List of currently used ZIP files.

	ArchiveList _archives; ///< List of currently used archives.

	ResourceMap _resources;

	ChangeSetList _changes;

	FileTypeList _resourceTypeTypes[kResourceMAX]; ///< All valid resource type file types.

	Common::UString findArchive(const Common::UString &file,
			const DirectoryList &dirs, const Common::FileList &files);

	ChangeID indexArchive(Archive *archive, uint32 priority);

	ChangeID indexKEY(const Common::UString &file, uint32 priority);
	ChangeID indexZIP(const Common::UString &file, uint32 priority);

	// KEY/BIF loading helpers
	ResFileRef findBIFPaths(const KEYFile &keyFile, ChangeID &change);
	void mergeKEYBIFResources(const KEYFile &keyFile, const ResFileRef &bifStart,
			ChangeID &change, uint32 priority);

	void addResource(const Resource &resource, Common::UString name, ChangeID &change);
	void addResources(const Common::FileList &files, ChangeID &change, uint32 priority);

	const Resource *getRes(Common::UString name, const std::vector<FileType> &types) const;

	Common::SeekableReadStream *getArchiveResource(const Resource &res) const;

	Common::SeekableReadStream *getResFile(const Resource &res) const;
};

} // End of namespace Aurora

/** Shortcut for accessing the sound manager. */
#define ResMan Aurora::ResourceManager::instance()

#endif // AURORA_RESMAN_H
