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
#include "aurora/util.h"
#include "aurora/error.h"
#include "aurora/keyfile.h"
#include "aurora/biffile.h"
#include "aurora/erffile.h"
#include "aurora/rimfile.h"

// boost-string_algo
using boost::iequals;

DECLARE_SINGLETON(Aurora::ResourceManager)

namespace Aurora {

void ResourceManager::State::clear() {
	resources.clear();
	bifs.clear();
	erfs.clear();
	rims.clear();
}


ResourceManager::ResourceManager() {
	_musicTypes.push_back(kFileTypeWAV);
	_musicTypes.push_back(kFileTypeBMU);
	_musicTypes.push_back(kFileTypeOGG);

	_soundTypes.push_back(kFileTypeWAV);
	_soundTypes.push_back(kFileTypeOGG);
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

	_bifSourceDir.clear();
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

void ResourceManager::registerDataBaseDir(const std::string &path) {
	clear();

	_baseDir = Common::FilePath::normalize(path);

	Common::FileList rootFiles;
	if (!rootFiles.addDirectory(_baseDir))
		throw Common::Exception("Can't read path");

	if (!rootFiles.getSubList(".*\\.key", _keyFiles, true))
		throw Common::Exception("No KEY files found");

	Common::FileList allFiles;
	if (!allFiles.addDirectory(_baseDir, -1))
		throw Common::Exception("Failed reading the complete directory");

	if (!allFiles.getSubList(".*\\.bif", _bifFiles, true))
		throw Common::Exception("No BIF files found");

	// Found KEY and BIF files, this looks like a useable data directory
	_bifSourceDir.push_back(_baseDir);
}

void ResourceManager::addBIFSourceDir(const std::string &dir) {
	std::string bifDir = Common::FilePath::findSubDirectory(_baseDir, dir, true);
	if (bifDir.empty())
		throw Common::Exception("No such directory");

	_bifSourceDir.push_back(Common::FilePath::normalize(bifDir));
}

void ResourceManager::loadSecondaryResources() {
	// Find all .mod, .hak and .rim in the respective directories

	_modDir = Common::FilePath::findSubDirectory(_baseDir, "modules", true);
	_hakDir = Common::FilePath::findSubDirectory(_baseDir, "hak"    , true);
	_rimDir = Common::FilePath::findSubDirectory(_baseDir, "rims"   , true);

	Common::FileList modFiles, hakFiles, rimFiles;
	modFiles.addDirectory(_modDir, -1);
	hakFiles.addDirectory(_hakDir, -1);
	rimFiles.addDirectory(_rimDir, -1);

	modFiles.getSubList(".*\\.mod", _erfFiles, true);
	hakFiles.getSubList(".*\\.hak", _erfFiles, true);
	modFiles.getSubList(".*\\.rim", _rimFiles, true);
	rimFiles.getSubList(".*\\.rim", _rimFiles, true);

	// Find all music files

	Common::FileList musicFiles;
	std::string musicDir;
	if (!(musicDir = Common::FilePath::findSubDirectory(_baseDir, "music"      , true)).empty())
		musicFiles.addDirectory(musicDir, -1);
	if (!(musicDir = Common::FilePath::findSubDirectory(_baseDir, "streammusic", true)).empty())
		musicFiles.addDirectory(musicDir, -1);

	addResources(musicFiles);

	// Find all sound files

	Common::FileList soundFiles;

	std::string soundDir;
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "ambient"     , true)).empty())
		soundFiles.addDirectory(soundDir, -1);
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "sounds"      , true)).empty())
		soundFiles.addDirectory(soundDir, -1);
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "streamsounds", true)).empty())
		soundFiles.addDirectory(soundDir, -1);
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "streamwaves" , true)).empty())
		soundFiles.addDirectory(soundDir, -1);
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "streamvoice" , true)).empty())
		soundFiles.addDirectory(soundDir, -1);

	addResources(soundFiles);

	// Add the override directory, which has priority over all other base sources

	Common::FileList overrideFiles;

	std::string overrideDir;
	if (!(overrideDir = Common::FilePath::findSubDirectory(_baseDir, "override", true)).empty())
		overrideFiles.addDirectory(overrideDir, -1);

	addResources(overrideFiles);
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

void ResourceManager::findBIFPaths(const KEYFile &keyFile, uint32 &bifStart) {
	bifStart = _state.bifs.size();

	uint32 keyBIFCount = keyFile.getBIFs().size();

	_state.bifs.resize(bifStart + keyBIFCount);

	// Go through all BIF names the KEY wants, trying to find a match in our BIF list
	for (uint32 i = 0; i < keyBIFCount; i++) {
		bool found = false;

		// Look through all BIF base directories
		for (std::vector<std::string>::const_iterator bifBase = _bifSourceDir.begin(); bifBase != _bifSourceDir.end(); ++bifBase) {
			// The BIF names in the KEY are relative to a BIF base directory
			_state.bifs[bifStart + i] = Common::FilePath::normalize(*bifBase + "/" + keyFile.getBIFs()[i]);

			// Look through all our BIFs, looking for a match
			for (Common::FileList::const_iterator it = _bifFiles.begin(); it != _bifFiles.end(); ++it) {
				if (iequals(*it, _state.bifs[bifStart + i])) {
					_state.bifs[bifStart + i] = *it;
					found = true;
					break;
				}
			}

			if (found)
				break;
		}

		// Did we find it?
		if (!found)
			throw Common::Exception("BIF \"%s\" not found", _state.bifs[bifStart + i].c_str());

	}
}

void ResourceManager::mergeKEYBIFResources(const KEYFile &keyFile, uint32 bifStart) {
	uint32 keyBIFCount = keyFile.getBIFs().size();

	std::vector<BIFFile> keyBIFFiles;

	keyBIFFiles.resize(keyBIFCount);

	// Try to load all needed BIF files
	try {

		for (uint32 i = 0; i < keyBIFCount; i++) {
			Common::File keyBIFFile;
			if (!keyBIFFile.open(_state.bifs[bifStart + i]))
				throw Common::Exception("Can't open file \"%s\"", _state.bifs[bifStart + i].c_str());

			keyBIFFiles[i].load(keyBIFFile);
		}

	} catch (Common::Exception &e) {
		e.add("Failed opening needed BIFs");
		throw e;
	}

	try {

		// Now cycle through all resource in the KEY, augmenting the information its BIF provides
		KEYFile::ResourceList::const_iterator keyRes;
		for (keyRes = keyFile.getResources().begin(); keyRes != keyFile.getResources().end(); ++keyRes) {

			// BIF index in range?
			if (keyRes->bifIndex >= keyBIFFiles.size())
				throw Common::Exception("No such BIF");

			// Resource index within the BIF in range?
			const BIFFile::ResourceList &bifRess = keyBIFFiles[keyRes->bifIndex].getResources();
			if (keyRes->resIndex >= bifRess.size())
				throw Common::Exception("Resource index out of range");

			// Type has to match
			const BIFFile::Resource &bifRes = bifRess[keyRes->resIndex];
			if (keyRes->type != bifRes.type)
				throw Common::Exception("Type mismatch on resource \"%s\" (%d, %d)",
						keyRes->name.c_str(), keyRes->type, bifRes.type);

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

	} catch (Common::Exception &e) {
		e.add("Failed indexing resource");
		throw e;
	}

}

void ResourceManager::loadKEY(Common::SeekableReadStream &key) {
	if (_baseDir.empty())
		throw Common::Exception("No base data directory registered");

	KEYFile keyFile;

	keyFile.load(key);

	// Search for the correct BIFs
	uint32 bifStart;
	findBIFPaths(keyFile, bifStart);

	// Merge the resource information of the KEY file and its BIF files into our resource map
	mergeKEYBIFResources(keyFile, bifStart);
}

void ResourceManager::addERF(const std::string &erf) {
	std::string erfFileName;

	if (Common::FilePath::isAbsolute(erf)) {
		// Absolute path to an ERF, open it from our ERF list

		erfFileName = _erfFiles.findFirst(Common::FilePath::normalize(erf), true);

		if (erfFileName.empty())
			// Does not exist
			throw Common::Exception("No such ERF");
	}

	if (erfFileName.empty())
		// Try to open from the .mod directory
		erfFileName = _erfFiles.findFirst(Common::FilePath::normalize(_modDir + "/" + erf), true);
	if (erfFileName.empty())
		// Try to open from the .hak directory
		erfFileName = _erfFiles.findFirst(Common::FilePath::normalize(_hakDir + "/" + erf), true);

	if (erfFileName.empty())
		// Does not exist
		throw Common::Exception("No such ERF");

	Common::File erfFile;
	if (!erfFile.open(erfFileName))
		throw Common::Exception(kOpenError);

	ERFFile erfIndex;

	erfIndex.load(erfFile);

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
}

void ResourceManager::addRIM(const std::string &rim) {
	std::string rimFileName;

	if (Common::FilePath::isAbsolute(rim)) {
		// Absolute path to an RIM, open it from our RIM list

		rimFileName = _rimFiles.findFirst(Common::FilePath::normalize(rim), true);

		if (rimFileName.empty())
			// Does not exist
			throw Common::Exception("No such RIM");
	}

	if (rimFileName.empty())
		// Try to open from the .rim directory
		rimFileName = _rimFiles.findFirst(Common::FilePath::normalize(_rimDir + "/" + rim), true);
	if (rimFileName.empty())
		// Try to open from the module directory
		rimFileName = _rimFiles.findFirst(Common::FilePath::normalize(_modDir + "/" + rim), true);

	if (rimFileName.empty())
		// Does not exist
		throw Common::Exception("No such RIM");

	Common::File rimFile;
	if (!rimFile.open(rimFileName))
		throw Common::Exception(kOpenError);

	RIMFile rimIndex;

	rimIndex.load(rimFile);

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
}

bool ResourceManager::hasResource(const std::string &name, FileType type) const {
	std::vector<FileType> types;

	types.push_back(type);

	return hasResource(name, types);
}

bool ResourceManager::hasResource(const std::string &name, const std::vector<FileType> &types) const {
	if (getRes(name, types))
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
	std::vector<FileType> types;

	types.push_back(type);

	return getResource(name, types);
}

Common::SeekableReadStream *ResourceManager::getResource(const std::string &name,
		const std::vector<FileType> &types) const {

	const Resource *res = getRes(name, types);
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

Common::SeekableReadStream *ResourceManager::getMusic(const std::string &name) const {
	// Try every known music file type
	Common::SeekableReadStream *res;
	if ((res = getResource(name, _musicTypes)))
		return res;

	// No such music
	return 0;
}

Common::SeekableReadStream *ResourceManager::getSound(const std::string &name) const {
	// Try every known sound file type
	Common::SeekableReadStream *res;
	if ((res = getResource(name, _soundTypes)))
		return res;

	// No such sound
	return 0;
}

void ResourceManager::addResource(const Resource &resource, std::string name) {
	boost::to_lower(name);

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

void ResourceManager::addResources(const Common::FileList &files) {
	for (Common::FileList::const_iterator file = files.begin(); file != files.end(); ++file) {
		Resource res;
		res.source = kSourceFile;
		res.path   = *file;
		res.type   = getFileType(*file);
		res.idx    = 0xFFFFFFFF;
		res.offset = 0xFFFFFFFF;
		res.size   = 0xFFFFFFFF;

		addResource(res, Common::FilePath::getStem(*file));
	}
}

const ResourceManager::Resource *ResourceManager::getRes(std::string name,
		const std::vector<FileType> &types) const {

	boost::to_lower(name);

	// Find the resources with the same name
	ResourceMap::const_iterator resFamily = _state.resources.find(name);
	if (resFamily == _state.resources.end())
		return 0;

	for (std::vector<FileType>::const_iterator type = types.begin(); type != types.end(); ++type) {
		// Find the specific resource of the given type
		ResourceTypeMap::const_iterator res = resFamily->second.find(*type);
		if (res != resFamily->second.end())
			return &res->second;
	}

	return 0;
}

} // End of namespace Aurora
