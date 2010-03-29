/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef AURORA_RESMAN_H
#define AURORA_RESMAN_H

#include <string>
#include <list>
#include <vector>
#include <map>

#include "common/types.h"
#include "common/filelist.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** A resource manager holding information about and handling all request for all
 *  resources useable by the game.
 */
class ResourceManager {
public:
	ResourceManager();
	~ResourceManager();

	/** Clear all resource information. */
	void clear();

	/** Save the list of resources.
	 *
	 *  So that the manager can be quickly restored to an initial game state.
	 */

	void save();
	/** Restore the list of resources.
	 *
	 *  To quickly restore the manager to an initial game state.
	 */
	void restore();

	/** Register a path to be the base data directory.
	 *
	 *  @param  path The path to a base data directory.
	 *  @return true if the path is a useable base data directory, false otherwise.
	 */
	bool registerDataBaseDir(const std::string &path);

	/** Return the list of key files found in the base data directory. */
	const Common::FileList &getKeyList() const;

	/** Does a specific resource exists?
	 *
	 *  @param  name The name (resref) of the resource.
	 *  @param  type The resource's type.
	 *  @return true if the resource exists, fale otherwise.
	 */
	bool hasResource(const std::string &name, FileType type) const;

	/** Return a resource.
	 *
	 *  @param  name The name (resref) of the resource.
	 *  @param  type The resource's type.
	 *  @return The resource stream or 0 if the resource doesn't exist.
	 */
	Common::SeekableReadStream *getResource(const std::string &name, FileType type) const;

private:
	/** Where a resource can be found. */
	enum Source {
		kSourceBIF,  ///< Within a bif file. */
		kSourceFile  ///< A direct file. */
	};

	/** A resource. */
	struct Resource {
		FileType type; ///< The resource's type.

		Source source; ///< Where can the resource be found? */

		// For kSourceBIF
		uint32 bif;    ///< Index into the bif vector.
		uint32 offset; ///< The offset within the bif file.
		uint32 size;   ///< The size of the resource data.

		// For kSourceFile
		std::string path; ///< The file's path.
	};

	/** A list of bif files. */
	typedef std::vector<std::string> BifList;

	/** Map over resources with the same name but different type. */
	typedef std::map<FileType,    Resource>        ResourceTypeMap;
	/** Map over resources, indexed by name. */
	typedef std::map<std::string, ResourceTypeMap> ResourceMap;

	BifList     _bifs;      ///< Bifs used by the game resources.
	ResourceMap _resources; ///< All game-usable resources.

	ResourceMap _resourcesSaved; ///< Saved list of game-usable resources.

	std::string _baseDir; ///< The data base directory.

	Common::FileList _keyFiles; ///< List of all key files in the base directory.
	Common::FileList _bifFiles; ///< List of all bif files in the base directory.

	const Resource *getRes(const std::string &name, FileType type) const;
};

} // End of namespace Aurora

#endif // AURORA_RESMAN_H
