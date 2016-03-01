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
 *  The global resource manager for Aurora resources.
 */

#ifndef AURORA_RESMAN_H
#define AURORA_RESMAN_H

#include <list>
#include <vector>
#include <map>
#include <set>

#include "src/common/types.h"
#include "src/common/ustring.h"
#include "src/common/singleton.h"
#include "src/common/filelist.h"
#include "src/common/hash.h"
#include "src/common/changeid.h"

#include "src/aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class Archive;
class KEYFile;
class BIFFile;

/** A resource manager holding information about and handling all request for all
 *  resources usable by the game.
 */
class ResourceManager : public Common::Singleton<ResourceManager> {
public:
	struct ResourceID {
		Common::UString name;
		FileType type;
		uint64 hash;
	};

	ResourceManager();
	~ResourceManager();

	/** Clear all resource information. */
	void clear();

	// .--- Global resource properties
	/** Are .rim/.rimp files actually ERF files? */
	void setRIMsAreERFs(bool rimsAreERFs);

	/** Do we have "small" files (compressed with Nintendo DS's LZSS algorithm)? */
	void setHasSmall(bool hasSmall);

	/** With which hash algorithm are/should the names be hashed? */
	void setHashAlgo(Common::HashAlgo algo);

	/** Set the array used to map cursor ID to cursor names. */
	void setCursorRemap(const std::vector<Common::UString> &remap);

	/** Add an alias for one file type to another.
	 *
	 *  @param alias The type to alias.
	 *  @param realType The actual type a resource of the alias type is.
	 */
	void addTypeAlias(FileType alias, FileType realType);
	// '---

	// .--- Data base
	/** Register a path to be the data base.
	 *
	 *  If this is a directory, it will be indexed, non-recursively.
	 *  If this is a file, it will be indexed as an archive.
	 *
	 *  The priority of files within the data base is always 1, the
	 *  lowest priority.
	 *
	 *  All further games files and archives are assumed to be inside
	 *  this directory or archive.
	 *
	 *  @param path The path to a base data directory or archive.
	 */
	void registerDataBase(const Common::UString &path);

	/** Return the path of the currently registered base data directory or archive. */
	const Common::UString &getDataBase() const;
	// '---

	// .--- Archives
	/** Does a specific archive exist?
	 *
	 *  @param  file The name of the archive file.
	 *  @return true if the archive exists, false otherwise.
	 */
	bool hasArchive(const Common::UString &file);

	/** Add all the resources of an archive to the resource manager.
	 *
	 *  @param file The name of the archive file to index.
	 *  @param priority The priority these files have over others of the same name
	 *                  and type. Higher number = higher priority. 0 means blacklisted.
	 *  @param changeID If given, record the collective changes done here.
	 */
	void indexArchive(const Common::UString &file, uint32 priority, Common::ChangeID *changeID = 0);
	// '---

	/** Add all the resources of an archive to the resource manager.
	 *
	 *  @param file The name of the archive file to index.
	 *  @param priority The priority these files have over others of the same name
	 *                  and type. Higher number = higher priority. 0 means blacklisted.
	 *  @param password Use this password to decrypt the archive file, if necessary.
	 *  @param changeID If given, record the collective changes done here.
	 */
	void indexArchive(const Common::UString &file, uint32 priority, const std::vector<byte> &password,
	                  Common::ChangeID *changeID = 0);
	// '---

	// .--- Directories and files
	/** Does a specific directory, relative to the base directory, exist?
	 *
	 *  @param  dir A subdirectory of the base directory to look for.
	 *  @return true if the directory exists, false otherwise.
	 */
	bool hasResourceDir(const Common::UString &dir);

	/** Add a single file to the resource manager.
	 *
	 *  Relative to the base directory.
	 *
	 *  @param file The file to add.
	 *  @param priority The priority these files have over others of the same name
	 *                  and type. Higher number = higher priority. 0 means blacklisted.
	 *  @param changeID If given, record the collective changes done here.
	 */
	void indexResourceFile(const Common::UString &file, uint32 priority, Common::ChangeID *changeID = 0);

	/** Add a directory's contents to the resource manager.
	 *
	 *  Relative to the base directory.
	 *
	 *  @param dir A subdirectory of the base directory to search for resource files.
	 *  @param glob A pattern of which files to look for.
	 *  @param depth The number of levels to recurse into subdirectories. 0
	 *               for ignoring subdirectories, -1 for a limitless recursion.
	 *  @param priority The priority these files have over others of the same name
	 *                  and type. Higher number = higher priority. 0 means blacklisted.
	 *  @param changeID If given, record the collective changes done here.
	 */
	void indexResourceDir(const Common::UString &dir, const char *glob, int depth,
	                      uint32 priority, Common::ChangeID *changeID = 0);
	// '---

	// .--- Utility methods
	/** Undo the changes done in the specified change ID. */
	void undo(Common::ChangeID &changeID);

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
	// '---

	// .--- Resources
	/** Does a specific resource exist?
	 *
	 *  @param  hash The hash of the name and extension of the resource.
	 *  @return true if the resource exists, false otherwise.
	 */
	bool hasResource(uint64 hash) const;

	/** Does a specific resource exist?
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  type The resource's type.
	 *  @return true if the resource exists, false otherwise.
	 */
	bool hasResource(const Common::UString &name, FileType type) const;

	/** Does a specific resource exist?
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  type The resource's type.
	 *  @return true if the resource exists, false otherwise.
	 */
	bool hasResource(const Common::UString &name, ResourceType type) const;

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

	/** Find and return the absolute filesystem file behind a resource.
	 *
	 *  If this resources does not exist, or the resource is not a direct file
	 *  (because it's found within an archive), an empty string will be returned.
	 *
	 *  @param name The name (ResRef) of the resource.
	 *  @param type The resource's type.
	 */
	Common::UString findResourceFile(const Common::UString &name, FileType type) const;

	/** Find and return the absolute filesystem file behind a resource.
	 *
	 *  If this resources does not exist, or the resource is not a direct file
	 *  (because it's found within an archive), an empty string will be returned.
	 *
	 *  @param name The name (ResRef) of the resource.
	 *  @param type The resource's type.
	 */
	Common::UString findResourceFile(const Common::UString &name, ResourceType type) const;

	/** Find and return the absolute filesystem file behind a resource.
	 *
	 *  If this resources does not exist, or the resource is not a direct file
	 *  (because it's found within an archive), an empty string will be returned.
	 *
	 *  @param name The name (with extension) of the resource.
	 */
	Common::UString findResourceFile(const Common::UString &name) const;

	/** Find and return the absolute filesystem file behind a resource.
	 *
	 *  If this resources does not exist, or the resource is not a direct file
	 *  (because it's found within an archive), an empty string will be returned.
	 *
	 *  @param name The name (ResRef) of the resource.
	 *  @param types The resource's types.
	 */
	Common::UString findResourceFile(const Common::UString &name, const std::vector<FileType> &types) const;

	/** Return a resource.
	 *
	 *  @param  hash The hash of the name and extension of the resource.
	 *  @param  type If != 0, that's where the type of the resource is stored.
	 *  @return The resource stream or 0 if the resource doesn't exist.
	 */
	Common::SeekableReadStream *getResource(uint64 hash, FileType *type = 0) const;

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
	// '---

	/** Dump a list of all resources into a file. */
	void dumpResourcesList(const Common::UString &fileName) const;


private:
	typedef std::vector<FileType> FileTypeList;
	typedef std::set<FileType> FileTypeSet;

	struct Resource;
	struct OpenedArchive;

	// .--- Archives
	struct KnownArchive {
		Common::UString name; ///< The archive's name.
		ArchiveType     type; ///< The archive's type.

		/** The resource this archive is. */
		Resource *resource;

		/** The opened archive, if it was. */
		OpenedArchive *opened;

		KnownArchive();
		KnownArchive(ArchiveType t, const Common::UString &n, Resource &r);
	};

	struct OpenedArchive {
		/** The actual archive. */
		Archive *archive;

		/** The information we know about this archive. */
		KnownArchive *known;

		/** Is this archive is found in another archive, this is the "parent" archive. */
		OpenedArchive *parent;
		/** If this archive contains other archives, these are the opened "children" archives. */
		std::list<OpenedArchive *> children;

		OpenedArchive();

		void set(KnownArchive &kA, Archive &a);
	};

	/** List of all known archive files. */
	typedef std::list<KnownArchive> KnownArchives;
	/** List of all opened archive files. */
	typedef std::list<OpenedArchive> OpenedArchives;
	// '---

	// .--- Resources
	/** Where a resource can be found. */
	enum Source {
		kSourceNone   , ///< Invalid source.
		kSourceFile   , ///< A direct file.
		kSourceArchive  ///< Within an archive.
	};

	/** A resource. */
	struct Resource {
		Common::UString name; ///< The resource's name.
		FileType        type; ///< The resource's type.

		/** Is this a "small" (compressed Nintendo DS) file? */
		bool isSmall;

		/** The resource's priority over others with the same name and type. */
		uint32 priority;

		/** The archive this resource itself is. */
		std::pair<KnownArchives *, KnownArchives::iterator> selfArchive;

		/** Where can the resource be found? */
		Source source;

		// For kSourceFile
		Common::UString path; ///< The file's path.

		// For kSourceArchive
		OpenedArchive *archive;      ///< Pointer to the opened archive.
		uint32         archiveIndex; ///< Index into the archive.

		Resource();

		bool operator<(const Resource &right) const;
	};

	/** List of resources, sorted by priority. */
	typedef std::list<Resource> ResourceList;
	/** Map over resources, indexed by their hashed name. */
	typedef std::map<uint64, ResourceList> ResourceMap;
	// '---

	// .--- Changes
	/** A change produced by adding an archive. */
	typedef std::pair<KnownArchives *, KnownArchives::iterator> KnownArchiveChange;
	/** A change produced by indexing/opening an archive. */
	typedef OpenedArchives::iterator OpenedArchiveChange;
	/** A change produced by indexing archive resources. */
	struct ResourceChange {
		ResourceMap::iterator  hashIt;
		ResourceList::iterator resIt;
	};

	typedef std::list<KnownArchiveChange>  KnownArchiveChanges;
	typedef std::list<OpenedArchiveChange> OpenedArchiveChanges;
	typedef std::list<ResourceChange>      ResourceChanges;

	/** A set of changes produced by a manager operation. */
	struct ChangeSet {
		KnownArchiveChanges  knownArchives;
		OpenedArchiveChanges openedArchives;
		ResourceChanges      resources;
	};

	typedef std::list<ChangeSet> ChangeSetList;

	class Change : public Common::ChangeContent {
	private:
		Change(ChangeSetList::iterator change) : _change(change) { }

		ChangeSetList::iterator _change;

		friend class ResourceManager;

	public:
		~Change() { }

		Common::ChangeContent *clone() const { return new Change(_change); }
	};
	// '---


	/** Do we have "small" files? */
	bool _hasSmall;

	/** With which hash algorithm are/should the names be hashed? */
	Common::HashAlgo _hashAlgo;

	/** Cursor ID -> cursor name. */
	std::vector<Common::UString> _cursorRemap;

	/** The data base directory (if any), the directory the current game is in. */
	Common::UString _baseDir;
	/** The data base archive (if any), the archive the current game is in. */
	Common::UString _baseArchive;

	KnownArchives  _knownArchives[kArchiveMAX]; ///< List of all known archives.
	OpenedArchives _openedArchives;             ///< List of currently used archives.

	/** The current type aliases, changing one type to another. */
	std::map<FileType, FileType> _typeAliases;

	ResourceMap   _resources; ///< All currently known resources.
	ChangeSetList _changes;   ///< Changes produced by indexing the currently known resources.

	FileTypeSet  _archiveTypeTypes [kArchiveMAX];  ///< All valid archive types file types.
	FileTypeList _resourceTypeTypes[kResourceMAX]; ///< All valid resource type file types.


	void clearResources();

	// .--- Searching for archives
	KnownArchive *findArchive(const Common::UString &file);
	KnownArchive *findArchive(Common::UString file, KnownArchives &archives);
	// '---

	// .--- Indexing archives
	void indexKEY(Common::SeekableReadStream *stream, uint32 priority, Change *change);
	uint32 openKEYBIFs(Common::SeekableReadStream *keyStream,
	                   std::vector<KnownArchive *> &archives, std::vector<BIFFile *> &bifs);

	void indexArchive(KnownArchive &knownArchive, Archive *archive,
	                  uint32 priority, Change *change);

	Common::SeekableReadStream *openArchiveStream(const KnownArchive &archive) const;
	// '---

	// .--- Adding resources

	bool checkResourceIsArchive(Resource &resource, Change *change);

	void addResource(Resource &resource, uint64 hash, Change *change);
	void addResource(const Common::UString &path, Change *change, uint32 priority);

	void addResources(const Common::FileList &files, Change *change, uint32 priority);
	// '---

	// .--- Finding and getting resources
	const Resource *getRes(uint64 hash) const;
	const Resource *getRes(const Common::UString &name, const std::vector<FileType> &types) const;
	const Resource *getRes(const Common::UString &name, FileType type) const;

	Common::SeekableReadStream *getResource(const Resource &res, bool tryNoCopy = false) const;

	Common::SeekableReadStream *getArchiveResource(const Resource &res, bool tryNoCopy = false) const;

	uint32 getResourceSize(const Resource &res) const;
	// '---

	// .--- Resource utility methods
	bool normalizeType(Resource &resource);

	ArchiveType     getArchiveType(FileType type) const;
	ArchiveType     getArchiveType(const Common::UString &name) const;
	Common::UString getArchiveName(const Resource &resource) const;

	inline uint64 getHash(const Common::UString &name, FileType type) const;
	inline uint64 getHash(const Common::UString &name) const;

	void checkHashCollision(const Resource &resource, ResourceMap::const_iterator resList);

	Change *newChangeSet(Common::ChangeID &changeID);
	// '---

};

} // End of namespace Aurora

/** Shortcut for accessing the sound manager. */
#define ResMan ::Aurora::ResourceManager::instance()

#endif // AURORA_RESMAN_H
