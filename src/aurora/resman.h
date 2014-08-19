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
#include "common/hash.h"

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

	typedef std::list<Archive *> ArchiveList;
	typedef ArchiveList::const_iterator ArchiveRef;

	/** Where a resource can be found. */
	enum Source {
		kSourceNone   , ///< Invalid source.
		kSourceArchive, ///< Within an archive.
		kSourceFile     ///< A direct file.
	};

	/** A resource. */
	struct Resource {
		Common::UString name; ///< The resource's name.
		FileType        type; ///< The resource's type.

		uint32 priority; ///< The resource's priority over others with the same name and type.

		Source source; ///< Where can the resource be found?

		// For kSourceArchive
		Archive *archive;      ///< Pointer to the archive.
		uint32   archiveIndex; ///< Index into the archive.

		// For kSourceFile
		Common::UString path; ///< The file's path.

		Resource();

		bool operator<(const Resource &right) const;
	};

	/** List of resources, sorted by priority. */
	typedef std::list<Resource> ResourceList;
	/** Map over resources, indexed by their hashed name. */
	typedef std::map<uint64, ResourceList> ResourceMap;

	/** A change produced by a manager operation. */
	struct ResourceChange {
		ResourceMap::iterator  hashIt;
		ResourceList::iterator resIt;
	};

	/** A set of changes produced by a manager operation. */
	struct ChangeSet {
		std::list<ArchiveList::iterator> archives;
		std::list<ResourceChange>        resources;
	};

	typedef std::list<ChangeSet> ChangeSetList;

public:
	struct ResourceID {
		Common::UString name;
		FileType type;
	};

	/** ID of a set of changes produced by a manager operation. */
	class ChangeID {
	public:
		ChangeID();

		bool empty() const;

		void clear();

	private:
		ChangeID(ChangeSetList::iterator c);

		bool _empty;
		ChangeSetList::iterator _change;

		friend class ResourceManager;
	};

	ResourceManager();
	~ResourceManager();

	/** Clear all resource information. */
	void clear();

	/** Are .rim files actually ERF files? */
	void setRIMsAreERFs(bool rimsAreERFs);

	/** With which hash algo are/should the names be hashed? */
	void setHashAlgo(Common::HashAlgo algo);

	/** Set the array used to map cursor ID to cursor names. */
	void setCursorRemap(const std::vector<Common::UString> &remap);

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
	void addArchiveDir(ArchiveType archive, const Common::UString &dir, bool recursive =  false);

	/** Does a specific archive exist?
	 *
	 *  @param  archive The type of archive to check.
	 *  @param  file The name of the archive file.
	 *  @return true if the archive exists, false otherwise.
	 */
	bool hasArchive(ArchiveType archive, const Common::UString &file);

	/** Add an archive file and all its resources to the resource manager.
	 *
	 *  @param  archive The type of archive to add.
	 *  @param  file The name of the archive file to index.
	 *  @param  priority The priority these files have over others of the same name
	 *          and type. Higher number = higher priority. 0 means blacklisted.
	 *  @return An ID for all collective changes done by adding the archive file.
	 */
	ChangeID addArchive(ArchiveType archive, const Common::UString &file, uint32 priority = 1);

	/** Add a directory's contents to the resource manager.
	 *
	 *  Relative to the base directory.
	 *
	 *  @param  dir A subdirectory of the base directory to search for resource files.
	 *  @param  glob A pattern of which files to look for.
	 *  @param  depth The number of levels to recurse into subdirectories. 0
	 *                for ignoring subdirectories, -1 for a limitless recursion.
	 *  @param  priority The priority these files have over others of the same name
	 *          and type. Higher number = higher priority. 0 means blacklisted.
	 *  @return An ID for all collective changes done by adding the directory.
	 */
	ChangeID addResourceDir(const Common::UString &dir, const char *glob = 0,
	                        int depth = -1, uint32 priority = 100);

	/** Undo the changes done in the specified change ID. */
	void undo(ChangeID &change);

	/** Add an alias for one file type to another.
	 *
	 *  @param alias The type to alias.
	 *  @param realType The actual type a resource of the alias type is.
	 */
	void addTypeAlias(FileType alias, FileType realType);

	/** Blacklist a specific resource.
	 *
	 *  That resource will never be returned when asked for. The ResourceManager
	 *  will act like it just doesn't exist.
	 *  Useful in cases where some resources of different FileTypes in a
	 *  ResourceType are broken and should not be preferred over the working ones.
	 */
	void blacklist(const Common::UString &name, FileType type);

	/** Declare the name of a specific resource.
	 *
	 *  Useful for declaring the name and type of hashed resources.
	 *
	 *  @param name The name (ResRef) of the resource.
	 *  @param type The resource's type.
	 */
	void declareResource(const Common::UString &name, FileType type);

	/** Declare the name of a specific resource.
	 *
	 *  Useful for declaring the name and type of hashed resources.
	 *
	 *  @param name The name (with extension) of the resource.
	 */
	void declareResource(const Common::UString &name);

	/** Does a specific resource exist?
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  type The resource's type.
	 *  @return true if the resource exists, false otherwise.
	 */
	bool hasResource(const Common::UString &name, FileType type) const;

	/** Does a specific resource exist?
	 *
	 *  @param  name The name (with extension) of the resource.
	 *  @return true if the resource exists, false otherwise.
	 */
	bool hasResource(const Common::UString &name) const;

	/** Does a specific resource exist?
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
	 *  @param  name The name (with extension) of the resource.
	 *  @return The resource stream or 0 if the resource doesn't exist.
	 */
	Common::SeekableReadStream *getResource(const Common::UString &name) const;

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
	 *  @param  foundType If != 0, that's where the actually found type is stored.
	 *  @return The resource stream or 0 if the music resource doesn't exist.
	 */
	Common::SeekableReadStream *getResource(ResourceType resType,
			const Common::UString &name, FileType *foundType = 0) const;

	/** Return a list of all available resources of the specified type. */
	void getAvailableResources(FileType type, std::list<ResourceID> &list) const;
	/** Return a list of all available resources of the specified type. */
	void getAvailableResources(const std::vector<FileType> &types, std::list<ResourceID> &list) const;
	/** Return a list of all available resources of the specified type. */
	void getAvailableResources(ResourceType type, std::list<ResourceID> &list) const;

	/** Dump a list of all resources into a file. */
	void dumpResourcesList(const Common::UString &fileName) const;

private:
	bool _rimsAreERFs; ///< Are .rim files actually ERF files?

	Common::HashAlgo _hashAlgo; ///< With which hash algo are/should the names be hashed?

	std::vector<Common::UString> _cursorRemap; ///< Cursor ID -> cursor name

	Common::UString _baseDir;     ///< The data base directory.

	DirectoryList    _archiveDirs [kArchiveMAX]; ///< Archive directories.
	Common::FileList _archiveFiles[kArchiveMAX]; ///< Archive files.

	ArchiveList _archives; ///< List of currently used archives.

	std::map<FileType, FileType> _typeAliases;

	ResourceMap _resources;

	ChangeSetList _changes;

	FileTypeList _resourceTypeTypes[kResourceMAX]; ///< All valid resource type file types.


	void clearResources();

	Common::UString findArchive(const Common::UString &file,
			const DirectoryList &dirs, const Common::FileList &files);

	ChangeID indexKEY(const Common::UString &file, uint32 priority);
	ChangeID indexArchive(Archive *archive, uint32 priority, ChangeID &change);

	// KEY/BIF loading helpers
	void findBIFs   (const KEYFile &key, std::vector<Common::UString> &bifs);
	void mergeKEYBIF(const KEYFile &key, std::vector<Common::UString> &bifs, std::vector<BIFFile *> &bifFiles);

	void normalizeType(Resource &resource);

	inline uint64 getHash(const Common::UString &name, FileType type) const;
	inline uint64 getHash(Common::UString name) const;

	void addResource(Resource &resource, uint64 hash, ChangeID &change);
	void addResource(Resource &resource, const Common::UString &name, ChangeID &change);

	void addResources(const Common::FileList &files, ChangeID &change, uint32 priority);

	const Resource *getRes(uint64 hash) const;
	const Resource *getRes(const Common::UString &name, const std::vector<FileType> &types) const;
	const Resource *getRes(const Common::UString &name, FileType type) const;

	Common::SeekableReadStream *getArchiveResource(const Resource &res) const;

	uint32 getResourceSize(const Resource &res) const;

	ChangeID newChangeSet();

	void checkHashCollision(const Resource &resource, ResourceMap::const_iterator resList);
};

} // End of namespace Aurora

/** Shortcut for accessing the sound manager. */
#define ResMan ::Aurora::ResourceManager::instance()

#endif // AURORA_RESMAN_H
