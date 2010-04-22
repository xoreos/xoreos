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
	class ZipFile;
}

namespace Aurora {

class KEYFile;
class BIFFile;
class NDSFile;

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
	typedef std::list<Common::ZipFile *> ResZipList;
	typedef ResZipList::const_iterator ResZipRef;
	typedef std::list<NDSFile *> ResNDSList;
	typedef ResNDSList::const_iterator ResNDSRef;

	/** Where a resource can be found. */
	enum Source {
		kSourceBIF,  ///< Within a BIF file.
		kSourceERF,  ///< Within an ERF file.
		kSourceRIM,  ///< Within a RIM file.
		kSourceZIP,  ///< Within a ZIP file.
		kSourceNDS,  ///< Within a Nintendo DS ROM file.
		kSourceFile  ///< A direct file.
	};

	/** A resource. */
	struct Resource {
		FileType type; ///< The resource's type.

		uint32 priority; ///< The resource's priority over others with the same name and type.

		Source source; ///< Where can the resource be found?

		// For kSourceBIF / kSourceERF / kSourceRIM
		ResFileRef resFile; ///< Iterator into the BIF/ERF/RIM list.
		uint32 offset;      ///< The offset within the BIF/ERF/RIM file.
		uint32 size;        ///< The size of the resource data.

		// For kSourceZIP
		ResZipRef resZip; ///< Iterator into the ZIP list.

		// For kSourceNDS
		ResNDSRef resNDS; ///< Iterator into the NDS list.

		// For kSourceFile / kSourceZIP / kSourceNDS
		Common::UString path; ///< The file's path.

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
		std::list<ResFileList::iterator> erfs;
		std::list<ResFileList::iterator> rims;
		std::list<ResZipList::iterator>  zips;
		std::list<ResNDSList::iterator>  ndss;
		std::list<ResourceChange>        resources;
	};

	typedef std::list<ChangeSet> ChangeSetList;

public:
	typedef ChangeSetList::iterator ChangeID;

	ResourceManager();
	~ResourceManager();

	/** Clear all resource information. */
	void clear();

	/** Register a path to be the base data directory.
	 *
	 *  @param path The path to a base data directory.
	 */
	void registerDataBaseDir(const Common::UString &path);

	/** Add a directory to be searched for these archives files.
	 *
	 *  Relative to the base directory. Only direct subdirectories of the base
	 *  directory are supported.
	 *
	 *  @param archive The type of archive to look for.
	 *  @param dir A direct subdirectory of the base directory to search for archives files.
	 */
	void addArchiveDir(Archive archive, const Common::UString &dir);

	/** Add an archive file and all its resources to the resource manager.
	 *
	 *  @param  archive The type of archive to add.
	 *  @param  The name of the archive file to index.
	 *  @param  priority The priority these files have over others of the same name and type.
	 *  @return An ID for all collective changes done by adding the archive file.
	 */
	ChangeID addArchive(Archive archive, const Common::UString &file, uint32 priority = 0);

	/** Load secondary resources.
	 *
	 *  Secondary resources are plain files found in the data directory structure.
	 *
	 *  @param  priority The priority these files have over others of the same name and type.
	 *  @return An ID for all collective changes done by loading the secondary resources.
	 */
	ChangeID loadSecondaryResources(uint32 priority = 400);

	/** Load override files.
	 *
	 *  Override files are commonly found in an override/ directory, overriding basic
	 *  resource files.
	 *
	 *  @param  priority The priority these files have over others of the same name and type.
	 *  @return An ID for all collective changes done by loading the override files.
	 */
	ChangeID loadOverrideFiles(uint32 priority = 500);

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
	Common::UString _baseDir;     ///< The data base directory.

	DirectoryList    _archiveDirs [kArchiveMAX]; ///< Archive directories.
	Common::FileList _archiveFiles[kArchiveMAX]; ///< Archive files.

	ResFileList _bifs; ///< List of currently used BIF files.
	ResFileList _erfs; ///< List of currently used ERF files.
	ResFileList _rims; ///< List of currently used RIM files.

	ResZipList  _zips; ///< List of currently used ZIP files.

	ResNDSList  _ndss; ///< List of currently used NDS files.

	ResourceMap _resources;

	ChangeSetList _changes;

	FileTypeList _resourceTypeTypes[kResourceMAX]; ///< All valid resource type file types.

	Common::UString findArchive(const Common::UString &file,
			const DirectoryList &dirs, const Common::FileList &files);

	ChangeID indexKEY(const Common::UString &file, uint32 priority);
	ChangeID indexERF(const Common::UString &file, uint32 priority);
	ChangeID indexRIM(const Common::UString &file, uint32 priority);
	ChangeID indexZIP(const Common::UString &file, uint32 priority);
	ChangeID indexNDS(const Common::UString &file, uint32 priority);

	// KEY/BIF loading helpers
	ResFileRef findBIFPaths(const KEYFile &keyFile, ChangeID &change);
	void mergeKEYBIFResources(const KEYFile &keyFile, const ResFileRef &bifStart,
			ChangeID &change, uint32 priority);

	void addResource(const Resource &resource, Common::UString name, ChangeID &change);
	void addResources(const Common::FileList &files, ChangeID &change, uint32 priority);

	const Resource *getRes(Common::UString name, const std::vector<FileType> &types) const;

	Common::SeekableReadStream *getOffResFile(const Resource &res) const;
	Common::SeekableReadStream *getZipResFile(const Resource &res) const;
	Common::SeekableReadStream *getNDSResFile(const Resource &res) const;
};

} // End of namespace Aurora

/** Shortcut for accessing the sound manager. */
#define ResMan Aurora::ResourceManager::instance()

#endif // AURORA_RESMAN_H
