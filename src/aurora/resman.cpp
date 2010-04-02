/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/resman.cpp
 *  The global resource manager for Aurora resources.
 */

#include "boost/algorithm/string.hpp"

#include "common/stream.h"
#include "common/filepath.h"
#include "common/file.h"
#include "common/util.h"

#include "aurora/resman.h"
#include "aurora/keyfile.h"
#include "aurora/biffile.h"

// boost-string_algo
using boost::iequals;

namespace Aurora {

ResourceManager::ResourceManager() {
}

ResourceManager::~ResourceManager() {
}

void ResourceManager::clear() {
	_bifs.clear();
	_resources.clear();

	_resourcesSaved.clear();

	_baseDir.clear();
}

void ResourceManager::save() {
	_resourcesSaved = _resources;
}

void ResourceManager::restore() {
	_resources = _resourcesSaved;
}

bool ResourceManager::registerDataBaseDir(const std::string &path) {
	_baseDir = path;

	Common::FileList rootFiles;
	if (!rootFiles.addDirectory(path))
		// Can't read the path
		return false;

	if (!rootFiles.getSubList(".*\\.key", _keyFiles, true))
		// No key files in the path's root
		return false;

	Common::FileList allFiles;
	if (!allFiles.addDirectory(path, -1))
		// Failed reading the complete directory tree
		return false;

	if (!allFiles.getSubList(".*\\.bif", _bifFiles, true))
		// No bif files in the path
		return false;

	// There's key and bif files, so it's probably a useable data base directory.
	return true;
}

const Common::FileList &ResourceManager::getKEYList() const {
	return _keyFiles;
}

bool ResourceManager::findBIFPaths(const KEYFile &keyFile, uint32 &bifStart) {
	bifStart = _bifs.size();

	uint32 keyBIFCount = keyFile.getBIFs().size();

	_bifs.resize(bifStart + keyBIFCount);

	// Go through all bif names the key wants, trying to find a match in our bif list
	for (uint32 i = 0; i < keyBIFCount; i++) {
		bool found = false;

		// All our bifs are in _baseDir/, and the bif names in the key should be relative to that
		_bifs[bifStart + i] = Common::FilePath::normalize(_baseDir + "/" + keyFile.getBIFs()[i]);

		// Look through all our bifs, looking for a match
		for (Common::FileList::const_iterator it = _bifFiles.begin(); it != _bifFiles.end(); ++it) {
			if (iequals(*it, _bifs[bifStart + i])) {
				_bifs[bifStart + i] = *it;
				found = true;
				break;
			}
		}

		// Did we find it?
		if (!found) {
			warning("ResourceManager::getBIFPaths(): \"%s\" not found", _bifs[bifStart + i].c_str());
			_bifs.resize(bifStart);
			return false;
		}

	}

	return true;
}

bool ResourceManager::mergeKEYBIFResources(const KEYFile &keyFile, uint32 bifStart) {
	uint32 keyBIFCount = keyFile.getBIFs().size();

	std::vector<BIFFile> keyBIFFiles;

	keyBIFFiles.resize(keyBIFCount);

	// Try to load all needed bif files
	for (uint32 i = 0; i < keyBIFCount; i++) {
		Common::File keyBIFFile;
		if (!keyBIFFile.open(_bifs[bifStart + i]))
			return false;

		if (!keyBIFFiles[i].load(keyBIFFile))
			return false;
	}

	// Now cycle through all resource in the key, augmenting the information its bif provides
	KEYFile::ResourceList::const_iterator keyRes;
	for (keyRes = keyFile.getResources().begin(); keyRes != keyFile.getResources().end(); ++keyRes) {

		// BIF index in range?
		if (keyRes->bifIndex >= keyBIFFiles.size())
			return false;

		// Resource index within the bif in range?
		const BIFFile::ResourceList &bifRess = keyBIFFiles[keyRes->bifIndex].getResources();
		if (keyRes->resIndex >= bifRess.size())
			return false;

		// Type has to match
		const BIFFile::Resource &bifRes = bifRess[keyRes->resIndex];
		if (keyRes->type != bifRes.type) {
			warning("ResourceManager::mergeKEYBIFResources(): Type mismatch on resource \"%s\" (%d, %d)",
					keyRes->name.c_str(), keyRes->type, bifRes.type);
			return false;
		}

		// Build the complete resource record
		Resource res;
		res.source = kSourceBIF;
		res.bif    = bifStart + keyRes->bifIndex;
		res.type   = keyRes->type;
		res.offset = bifRes.offset;
		res.size   = bifRes.size;

		// And add it to our list
		addResource(res, keyRes->name);
	}

	return true;
}

bool ResourceManager::loadKEY(Common::SeekableReadStream &key) {
	if (_baseDir.empty()) {
		warning("ResourceManager::loadKEY(): No base data directory registered");
		return false;
	}

	KEYFile keyFile;

	if (!keyFile.load(key))
		return false;

	// Search for the correct bifs
	uint32 bifStart;
	if (!findBIFPaths(keyFile, bifStart))
		return false;

	// Merge the resource information of the key file and its bif files into our resource map
	if (!mergeKEYBIFResources(keyFile, bifStart))
		return false;

	return true;
}

bool ResourceManager::hasResource(const std::string &name, FileType type) const {
	if (getRes(name, type))
		return true;

	return false;
}

Common::SeekableReadStream *ResourceManager::getResource(const std::string &name, FileType type) const {
	const Resource *res = getRes(name, type);
	if (!res)
		return 0;

	if        (res->source == kSourceBIF) {
		// Read the data out of the bif and return a MemoryReadStream

		if (res->bif >= _bifs.size())
			return 0;

		Common::File file;
		if (!file.open(_bifs[res->bif]))
			return 0;

		if (!file.seek(res->offset))
			return 0;

		byte *data = new byte[res->size];

		if (file.read(data, res->size) != res->size) {
			delete[] data;
			return 0;
		}

		return new Common::MemoryReadStream(data, res->size, DisposeAfterUse::YES);

	} else if (res->source == kSourceFile) {
		// Open the file and return it

		Common::File *file = new Common::File;

		if (!file->open(res->path)) {
			delete file;
			return 0;
		}

		return file;
	}

	return 0;
}

void ResourceManager::addResource(const Resource &resource, const std::string &name) {
	ResourceMap::iterator resTypeMap = _resources.find(name);
	if (resTypeMap == _resources.end()) {
		// We don't yet have a resource with this name, create a new type map for it

		std::pair<ResourceMap::iterator, bool> result;

		result = _resources.insert(std::make_pair(name, ResourceTypeMap()));

		resTypeMap = result.first;
	}

	// Add the resource to the type map
	resTypeMap->second.insert(std::make_pair(resource.type, resource));
}

const ResourceManager::Resource *ResourceManager::getRes(const std::string &name, FileType type) const {
	// Find the resources with the same name
	ResourceMap::const_iterator resFamily = _resources.find(name);
	if (resFamily == _resources.end())
		return 0;

	// Find the specific resource of the given type
	ResourceTypeMap::const_iterator res = resFamily->second.find(type);
	if (res == resFamily->second.end())
		return 0;

	return &res->second;
}

} // End of namespace Aurora
