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
#include "aurora/erffile.h"
#include "aurora/rimfile.h"

// boost-string_algo
using boost::iequals;

namespace Aurora {

void ResourceManager::State::clear() {
	resources.clear();
	bifs.clear();
	erfs.clear();
	rims.clear();
}


ResourceManager::ResourceManager() {
}

ResourceManager::~ResourceManager() {
}

void ResourceManager::clear() {
	_state.clear();

	_baseDir.clear();
	_modDir.clear();
	_hakDir.clear();
	_rimDir.clear();

	_keyFiles.clear();
	_bifFiles.clear();
	_erfFiles.clear();
	_rimFiles.clear();
}

void ResourceManager::stackPush() {
	_stateStack.push(_state);
}

void ResourceManager::stackPop() {
	stackApply();
	stackDrop();
}

void ResourceManager::stackApply() {
	_state = _stateStack.top();
}

void ResourceManager::stackDrop() {
	_stateStack.pop();
}

bool ResourceManager::registerDataBaseDir(const std::string &path) {
	clear();

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
		// No BIF files in the path
		return false;

	// There's KEY and BIF files, so it's probably a useable data base directory.
	// Now find other resource sources.
	if (!initSecondaryResources())
		return false;

	return true;
}

bool ResourceManager::initSecondaryResources() {
	// Find all .mod, .hak and .rim in the respective directories
	_modDir = Common::FilePath::findSubDirectory(_baseDir, "modules", true);
	_hakDir = Common::FilePath::findSubDirectory(_baseDir, "hak"    , true);
	_rimDir = Common::FilePath::findSubDirectory(_baseDir, "rims"   , true);

	Common::FileList modFiles, hakFiles, rimFiles;
	modFiles.addDirectory(_modDir);
	hakFiles.addDirectory(_hakDir);
	rimFiles.addDirectory(_rimDir);

	modFiles.getSubList(".*\\.mod", _erfFiles, true);
	hakFiles.getSubList(".*\\.hak", _erfFiles, true);
	modFiles.getSubList(".*\\.rim", _rimFiles, true);
	rimFiles.getSubList(".*\\.rim", _rimFiles, true);

	return true;
}

const Common::FileList &ResourceManager::getKEYList() const {
	return _keyFiles;
}

const Common::FileList &ResourceManager::getERFList() const {
	return _erfFiles;
}

const Common::FileList &ResourceManager::getRIMList() const {
	return _rimFiles;
}

bool ResourceManager::findBIFPaths(const KEYFile &keyFile, uint32 &bifStart) {
	bifStart = _state.bifs.size();

	uint32 keyBIFCount = keyFile.getBIFs().size();

	_state.bifs.resize(bifStart + keyBIFCount);

	// Go through all BIF names the KEY wants, trying to find a match in our BIF list
	for (uint32 i = 0; i < keyBIFCount; i++) {
		bool found = false;

		// All our BIFs are in _baseDir/, and the BIF names in the KEY should be relative to that
		_state.bifs[bifStart + i] = Common::FilePath::normalize(_baseDir + "/" + keyFile.getBIFs()[i]);

		// Look through all our BIFs, looking for a match
		for (Common::FileList::const_iterator it = _bifFiles.begin(); it != _bifFiles.end(); ++it) {
			if (iequals(*it, _state.bifs[bifStart + i])) {
				_state.bifs[bifStart + i] = *it;
				found = true;
				break;
			}
		}

		// Did we find it?
		if (!found) {
			warning("ResourceManager::getBIFPaths(): \"%s\" not found", _state.bifs[bifStart + i].c_str());
			_state.bifs.resize(bifStart);
			return false;
		}

	}

	return true;
}

bool ResourceManager::mergeKEYBIFResources(const KEYFile &keyFile, uint32 bifStart) {
	uint32 keyBIFCount = keyFile.getBIFs().size();

	std::vector<BIFFile> keyBIFFiles;

	keyBIFFiles.resize(keyBIFCount);

	// Try to load all needed BIF files
	for (uint32 i = 0; i < keyBIFCount; i++) {
		Common::File keyBIFFile;
		if (!keyBIFFile.open(_state.bifs[bifStart + i]))
			return false;

		if (!keyBIFFiles[i].load(keyBIFFile))
			return false;
	}

	// Now cycle through all resource in the KEY, augmenting the information its BIF provides
	KEYFile::ResourceList::const_iterator keyRes;
	for (keyRes = keyFile.getResources().begin(); keyRes != keyFile.getResources().end(); ++keyRes) {

		// BIF index in range?
		if (keyRes->bifIndex >= keyBIFFiles.size())
			return false;

		// Resource index within the BIF in range?
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
		res.idx    = bifStart + keyRes->bifIndex;
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

	// Search for the correct BIFs
	uint32 bifStart;
	if (!findBIFPaths(keyFile, bifStart))
		return false;

	// Merge the resource information of the KEY file and its BIF files into our resource map
	if (!mergeKEYBIFResources(keyFile, bifStart))
		return false;

	return true;
}

bool ResourceManager::addERF(const std::string &erf) {
	std::string erfFileName;

	if (Common::FilePath::isAbsolute(erf)) {
		// Absolute path to an ERF, open it from our ERF list

		erfFileName = _erfFiles.findFirst(Common::FilePath::normalize(erf), true);

		if (erfFileName.empty())
			// Does not exist
			return false;
	}

	if (erfFileName.empty())
		// Try to open from the .mod directory
		erfFileName = _erfFiles.findFirst(Common::FilePath::normalize(_modDir + "/" + erf), true);
	if (erfFileName.empty())
		// Try to open from the .hak directory
		erfFileName = _erfFiles.findFirst(Common::FilePath::normalize(_hakDir + "/" + erf), true);

	if (erfFileName.empty())
		// Does not exist
		return false;

	Common::File erfFile;
	if (!erfFile.open(erfFileName))
		return false;

	ERFFile erfIndex;

	if (!erfIndex.load(erfFile))
		return false;

	int erfIdx = _state.erfs.size();

	_state.erfs.push_back(erfFileName);

	const ERFFile::ResourceList &resources = erfIndex.getResources();
	for (ERFFile::ResourceList::const_iterator resource = resources.begin(); resource != resources.end(); ++resource) {
		// Build the resource record
		Resource res;
		res.source = kSourceERF;
		res.idx    = erfIdx;
		res.type   = resource->type;
		res.offset = resource->offset;
		res.size   = resource->size;

		// And add it to our list
		addResource(res, resource->name);
	}

	return true;
}

bool ResourceManager::addRIM(const std::string &rim) {
	std::string rimFileName;

	if (Common::FilePath::isAbsolute(rim)) {
		// Absolute path to an RIM, open it from our RIM list

		rimFileName = _rimFiles.findFirst(Common::FilePath::normalize(rim), true);

		if (rimFileName.empty())
			// Does not exist
			return false;
	}

	if (rimFileName.empty())
		// Try to open from the .rim directory
		rimFileName = _rimFiles.findFirst(Common::FilePath::normalize(_rimDir + "/" + rim), true);
	if (rimFileName.empty())
		// Try to open from the module directory
		rimFileName = _rimFiles.findFirst(Common::FilePath::normalize(_modDir + "/" + rim), true);

	if (rimFileName.empty())
		// Does not exist
		return false;

	Common::File rimFile;
	if (!rimFile.open(rimFileName))
		return false;

	RIMFile rimIndex;

	if (!rimIndex.load(rimFile))
		return false;

	int rimIdx = _state.rims.size();

	_state.rims.push_back(rimFileName);

	const RIMFile::ResourceList &resources = rimIndex.getResources();
	for (RIMFile::ResourceList::const_iterator resource = resources.begin(); resource != resources.end(); ++resource) {
		// Build the resource record
		Resource res;
		res.source = kSourceRIM;
		res.idx    = rimIdx;
		res.type   = resource->type;
		res.offset = resource->offset;
		res.size   = resource->size;

		// And add it to our list
		addResource(res, resource->name);
	}

	return true;
}

bool ResourceManager::hasResource(const std::string &name, FileType type) const {
	if (getRes(name, type))
		return true;

	return false;
}

Common::SeekableReadStream *ResourceManager::getOffResFile(const ResFileList &list, const Resource &res) const {
	// Read the data out of the file and return a MemoryReadStream

	if (res.idx >= list.size())
		return 0;

	Common::File file;
	if (!file.open(list[res.idx]))
		return 0;

	if (!file.seek(res.offset))
		return 0;

	byte *data = new byte[res.size];

	if (file.read(data, res.size) != res.size) {
		delete[] data;
		return 0;
	}

	return new Common::MemoryReadStream(data, res.size, DisposeAfterUse::YES);
}

Common::SeekableReadStream *ResourceManager::getResource(const std::string &name, FileType type) const {
	const Resource *res = getRes(name, type);
	if (!res)
		return 0;

	if        (res->source == kSourceBIF) {
		// Read the data out of the BIF and return a MemoryReadStream
		return getOffResFile(_state.bifs, *res);
	} else if (res->source == kSourceERF) {
		return getOffResFile(_state.erfs, *res);
	} else if (res->source == kSourceRIM) {
		return getOffResFile(_state.rims, *res);
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
	ResourceMap::iterator resTypeMap = _state.resources.find(name);
	if (resTypeMap == _state.resources.end()) {
		// We don't yet have a resource with this name, create a new type map for it

		std::pair<ResourceMap::iterator, bool> result;

		result = _state.resources.insert(std::make_pair(name, ResourceTypeMap()));

		resTypeMap = result.first;
	}

	// Add the resource to the type map
	resTypeMap->second.insert(std::make_pair(resource.type, resource));
}

const ResourceManager::Resource *ResourceManager::getRes(const std::string &name, FileType type) const {
	// Find the resources with the same name
	ResourceMap::const_iterator resFamily = _state.resources.find(name);
	if (resFamily == _state.resources.end())
		return 0;

	// Find the specific resource of the given type
	ResourceTypeMap::const_iterator res = resFamily->second.find(type);
	if (res == resFamily->second.end())
		return 0;

	return &res->second;
}

} // End of namespace Aurora
