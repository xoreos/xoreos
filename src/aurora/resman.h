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

#include <string>
#include <stack>
#include <list>
#include <vector>
#include <map>

#include "common/types.h"
#include "common/singleton.h"
#include "common/filelist.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class KEYFile;
class BIFFile;

/** A resource manager holding information about and handling all request for all
 *  resources useable by the game.
 */
class ResourceManager : public Common::Singleton<ResourceManager> {
// Type definitions
private:
	typedef std::list<std::string> ResFileList;
	typedef ResFileList::const_iterator ResFileRef;

	/** Where a resource can be found. */
	enum Source {
		kSourceBIF,  ///< Within a BIF file.
		kSourceERF,  ///< Within an ERF file.
		kSourceRIM,  ///< Within a RIM file.
		kSourceZIP,  ///< Within a ZIP file.
		kSourceFile  ///< A direct file.
	};

	/** A resource. */
	struct Resource {
		FileType type; ///< The resource's type.

		uint32 priority; ///< The resource's priority over others with the same name and type.

		Source source; ///< Where can the resource be found?

		// For kSourceBIF / kSourceERF / kSourceRIM / kSourceZIP
		ResFileRef resFile; ///< Iterator into the BIF/ERF/RIM/ZIP list.
		uint32 offset;      ///< The offset within the BIF/ERF/RIM file.
		uint32 size;        ///< The size of the resource data.

		// For kSourceFile / kSourceZIP
		std::string path; ///< The file's path.

		bool operator<(const Resource &right) const;
	};

	/** List of resources, sorted by priority. */
	typedef std::list<Resource>                    ResourceList;
	/** Map over resources with the same name but different type. */
	typedef std::map<FileType,    ResourceList>    ResourceTypeMap;
	/** Map over resources, indexed by name. */
	typedef std::map<std::string, ResourceTypeMap> ResourceMap;

	struct ResourceChange {
		ResourceMap::iterator     nameIt;
		ResourceTypeMap::iterator typeIt;
		ResourceList::iterator    resIt;
	};

	struct ChangeSet {
		std::list<ResFileList::iterator> bifs;
		std::list<ResFileList::iterator> erfs;
		std::list<ResFileList::iterator> rims;
		std::list<ResFileList::iterator> zips;
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
	void registerDataBaseDir(const std::string &path);

	/** Add a directory to be searched for BIF files.
	 *
	 *  Relative to the base directory. Only direct subdirectories of the base
	 *  directory are supported.
	 *
	 *  @param dir A direct subdirectory of the base directory to search for BIFs.
	 */
	void addBIFSourceDir(const std::string &dir);

	/** Find source directories for resource archives. */
	void findSourceDirs();

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

	/** Return the list of KEY files found in the base data directory. */
	const Common::FileList &getKEYList() const;

	/** Return the list of ERF files found in the base data directory. */
	const Common::FileList &getERFList() const;

	/** Return the list of RIM files found in the base data directory. */
	const Common::FileList &getRIMList() const;

	/** Load a KEY index.
	 *
	 *  Add all resources found in the KEY and its BIFs to the manager.
	 *
	 *  @param  key The KEY file to index.
	 *  @param  priority The priority the resources have over others of the same name and type.
	 *  @return An ID for all collective changes done by loading the KEY and its BIFs.
	 */
	ChangeID loadKEY(Common::SeekableReadStream &key, uint32 priority = 10);

	/** Add resources found in the ERF file to the manager.
	 *
	 *  @param  erf The name of the ERF file within a valid ERF directory in the base dir.
	 *  @param  priority The priority the resources have over others of the same name and type.
	 *  @return An ID for all collective changes done by loading the ERF.
	 */
	ChangeID addERF(const std::string &erf, uint32 priority = 100);

	/** Add resources found in the RIM file to the manager.
	 *
	 *  @param  rim The name of the RIM file within a valid RIM directory in the base dir.
	 *  @param  priority The priority the resources have over others of the same name and type.
	 *  @return An ID for all collective changes done by loading the RIM.
	 */
	ChangeID addRIM(const std::string &rim, uint32 priority = 100);

	/** Undo the changes done in the specified change ID. */
	void undo(ChangeID &change);

	/** Does a specific resource exists?
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  type The resource's type.
	 *  @return true if the resource exists, false otherwise.
	 */
	bool hasResource(const std::string &name, FileType type) const;

	/** Does a specific resource exists?
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  types The resource's types.
	 *  @return true if the resource exists, false otherwise.
	 */
	bool hasResource(const std::string &name, const std::vector<FileType> &types) const;

	/** Return a resource.
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  type The resource's type.
	 *  @return The resource stream or 0 if the resource doesn't exist.
	 */
	Common::SeekableReadStream *getResource(const std::string &name, FileType type) const;

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
	Common::SeekableReadStream *getResource(const std::string &name,
			const std::vector<FileType> &types, FileType *foundType = 0) const;

	/** Return a music resource.
	 *
	 *  @param  name The name (ResRef or path) of the resource.
	 *  @param  type If != 0, that's where the resource's type is stored.
	 *  @return The music resource stream or 0 if the music resource doesn't exist.
	 */
	Common::SeekableReadStream *getMusic(const std::string &name, FileType *type = 0) const;

	/** Return a sound resource.
	 *
	 *  @param  name The name (ResRef or path) of the resource.
	 *  @param  type If != 0, that's where the resource's type is stored.
	 *  @return The sound resource stream or 0 if the sound resource doesn't exist.
	 */
	Common::SeekableReadStream *getSound(const std::string &name, FileType *type = 0) const;

	/** Return an image resource.
	 *
	 *  @param  name The name (ResRef or path) of the resource.
	 *  @param  type If != 0, that's where the resource's type is stored.
	 *  @return The image resource stream or 0 if the image resource doesn't exist.
	 */
	Common::SeekableReadStream *getImage(const std::string &name, FileType *type = 0) const;

	/** Return an video resource.
	 *
	 *  @param  name The name (ResRef or path) of the resource.
	 *  @param  type If != 0, that's where the resource's type is stored.
	 *  @return The video resource stream or 0 if the video resource doesn't exist.
	 */
	Common::SeekableReadStream *getVideo(const std::string &name, FileType *type = 0) const;

	/** Small debug method that lists all indexed resources. */
	void listResources() const;

private:
	ResFileList _bifs;
	ResFileList _erfs;
	ResFileList _rims;
	ResFileList _zips;
	ResourceMap _resources;

	ChangeSetList _changes;

	std::string _baseDir;     ///< The data base directory.
	std::string _modDir;      ///< The data directory for .mod files.
	std::string _hakDir;      ///< The data directory for .hak files.
	std::string _textureDir;  ///< The data directory for textures-related files.
	std::string _rimDir;      ///< The data directory for .rim files.

	std::vector<std::string> _bifSourceDir; ///< All directories containing BIFs.

	Common::FileList _keyFiles; ///< List of all KEY files in the base directory.
	Common::FileList _bifFiles; ///< List of all BIF files in the base directory.
	Common::FileList _erfFiles; ///< List of all ERF files in the base directory.
	Common::FileList _rimFiles; ///< List of all RIM files in the base directory.

	std::vector<FileType> _musicTypes; ///< All valid music file types.
	std::vector<FileType> _soundTypes; ///< All valid sound file types.
	std::vector<FileType> _imageTypes; ///< All valid image file types.
	std::vector<FileType> _videoTypes; ///< All valid video file types.

	// KEY/BIF loading helpers
	ResFileRef findBIFPaths(const KEYFile &keyFile, ChangeID &change);
	void mergeKEYBIFResources(const KEYFile &keyFile, const ResFileRef &bifStart,
			ChangeID &change, uint32 priority);

	void addResource(const Resource &resource, std::string name, ChangeID &change);
	void addResources(const Common::FileList &files, ChangeID &change, uint32 priority);

	const Resource *getRes(std::string name, const std::vector<FileType> &types) const;

	Common::SeekableReadStream *getOffResFile(const Resource &res) const;
};

} // End of namespace Aurora

/** Shortcut for accessing the sound manager. */
#define ResMan Aurora::ResourceManager::instance()

#endif // AURORA_RESMAN_H
