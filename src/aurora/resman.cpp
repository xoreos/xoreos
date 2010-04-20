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

#include "common/util.h"
#include "common/stream.h"
#include "common/filepath.h"
#include "common/file.h"
#include "common/zipfile.h"

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

bool ResourceManager::Resource::operator<(const Resource &right) const {
	return priority < right.priority;
}


ResourceManager::ResourceManager() {
	_musicTypes.push_back(kFileTypeWAV);
	_musicTypes.push_back(kFileTypeBMU);
	_musicTypes.push_back(kFileTypeOGG);

	_soundTypes.push_back(kFileTypeWAV);
	_soundTypes.push_back(kFileTypeOGG);

	_imageTypes.push_back(kFileTypeDDS);
	_imageTypes.push_back(kFileTypeTPC);
	_imageTypes.push_back(kFileTypeTGA);
	_imageTypes.push_back(kFileTypePNG);
	_imageTypes.push_back(kFileTypeBMP);
	_imageTypes.push_back(kFileTypeJPG);

	_videoTypes.push_back(kFileTypeBIK);
	_videoTypes.push_back(kFileTypeMPG);
	_videoTypes.push_back(kFileTypeWMV);
}

ResourceManager::~ResourceManager() {
	clear();

	_musicTypes.clear();
	_soundTypes.clear();
	_imageTypes.clear();
	_videoTypes.clear();
}

void ResourceManager::clear() {
	_changes.clear();

	_resources.clear();
	_bifs.clear();
	_erfs.clear();
	_rims.clear();
	_zips.clear();

	_baseDir.clear();
	_modDir.clear();
	_hakDir.clear();
	_textureDir.clear();
	_rimDir.clear();
	_zipDir.clear();

	_keyFiles.clear();
	_bifFiles.clear();
	_erfFiles.clear();
	_rimFiles.clear();
	_zipFiles.clear();

	_bifSourceDir.clear();
}

void ResourceManager::registerDataBaseDir(const std::string &path) {
	clear();

	_baseDir = Common::FilePath::normalize(path);

	Common::FileList rootFiles;
	if (!rootFiles.addDirectory(_baseDir))
		throw Common::Exception("Can't read path");

	// Find KEY files
	rootFiles.getSubList(".*\\.key", _keyFiles, true);

	Common::FileList allFiles;
	if (!allFiles.addDirectory(_baseDir, -1))
		throw Common::Exception("Failed reading the complete directory");

	// Find BIF files
	allFiles.getSubList(".*\\.bif", _bifFiles, true);

	// Find ZIP files
	_zipDir = Common::FilePath::findSubDirectory(_baseDir, "data", true);
	allFiles.getSubList(".*\\.zip", _zipFiles, true);

	if (!((!_keyFiles.isEmpty() && !_bifFiles.isEmpty()) || !_zipFiles.isEmpty()))
		throw Common::Exception("No KEY/BIF files and no ZIP files found");

	// Found KEY and BIF files, or ZIP files, this looks like a useable data directory
	_bifSourceDir.push_back(_baseDir);
}

void ResourceManager::addBIFSourceDir(const std::string &dir) {
	std::string bifDir = Common::FilePath::findSubDirectory(_baseDir, dir, true);
	if (bifDir.empty())
		throw Common::Exception("No such directory");

	_bifSourceDir.push_back(Common::FilePath::normalize(bifDir));
}

void ResourceManager::findSourceDirs() {
	// Find all .mod, .hak and .rim in the respective directories

	_modDir     = Common::FilePath::findSubDirectory(_baseDir, "modules"     , true);
	_hakDir     = Common::FilePath::findSubDirectory(_baseDir, "hak"         , true);
	_textureDir = Common::FilePath::findSubDirectory(_baseDir, "texturepacks", true);
	_rimDir     = Common::FilePath::findSubDirectory(_baseDir, "rims"        , true);

	Common::FileList modFiles, hakFiles, textureFiles, rimFiles;
	modFiles.addDirectory(_modDir, -1);
	hakFiles.addDirectory(_hakDir, -1);
	textureFiles.addDirectory(_textureDir, -1);
	rimFiles.addDirectory(_rimDir, -1);

	modFiles.getSubList(".*\\.mod", _erfFiles, true);
	hakFiles.getSubList(".*\\.hak", _erfFiles, true);
	textureFiles.getSubList(".*\\.erf", _erfFiles, true);
	modFiles.getSubList(".*\\.rim", _rimFiles, true);
	rimFiles.getSubList(".*\\.rim", _rimFiles, true);
}

ResourceManager::ChangeID ResourceManager::loadSecondaryResources(uint32 priority) {
	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	// Find all music files

	Common::FileList musicFiles;
	std::string musicDir;
	if (!(musicDir = Common::FilePath::findSubDirectory(_baseDir, "music"      , true)).empty())
		musicFiles.addDirectory(musicDir, -1);
	if (!(musicDir = Common::FilePath::findSubDirectory(_baseDir, "music_x1"   , true)).empty())
		musicFiles.addDirectory(musicDir, -1);
	if (!(musicDir = Common::FilePath::findSubDirectory(_baseDir, "music_x2"   , true)).empty())
		musicFiles.addDirectory(musicDir, -1);
	if (!(musicDir = Common::FilePath::findSubDirectory(_baseDir, "streammusic", true)).empty())
		musicFiles.addDirectory(musicDir, -1);

	addResources(musicFiles, change, priority);

	// Find all sound files

	Common::FileList soundFiles;

	std::string soundDir;
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "ambient"     , true)).empty())
		soundFiles.addDirectory(soundDir, -1);
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "ambient_x1"  , true)).empty())
		soundFiles.addDirectory(soundDir, -1);
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "ambient_x2"  , true)).empty())
		soundFiles.addDirectory(soundDir, -1);
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "sounds"      , true)).empty())
		soundFiles.addDirectory(soundDir, -1);
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "streamsounds", true)).empty())
		soundFiles.addDirectory(soundDir, -1);
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "streamwaves" , true)).empty())
		soundFiles.addDirectory(soundDir, -1);
	if (!(soundDir = Common::FilePath::findSubDirectory(_baseDir, "streamvoice" , true)).empty())
		soundFiles.addDirectory(soundDir, -1);

	addResources(soundFiles, change, priority);

	// Find all video files

	Common::FileList movieFiles, rootFiles;

	std::string videoDir;
	if (!(videoDir = Common::FilePath::findSubDirectory(_baseDir, "movies"   , true)).empty())
		movieFiles.addDirectory(videoDir, -1);
	if (!(videoDir = Common::FilePath::findSubDirectory(_baseDir, "cutscenes", true)).empty())
		movieFiles.addDirectory(videoDir, -1);
	rootFiles.addDirectory(_baseDir);

	Common::FileList videoFiles;

	movieFiles.getSubList(".*\\.(bik|mpg|wmv)", videoFiles, true);
	rootFiles.getSubList (".*\\.(bik|mpg|wmv)", videoFiles, true);

	addResources(videoFiles, change, priority);

	return change;
}

ResourceManager::ChangeID ResourceManager::loadOverrideFiles(uint32 priority) {
	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	// Add the override directory, which has priority over all other base sources

	Common::FileList overrideFiles;

	std::string overrideDir;
	if (!(overrideDir = Common::FilePath::findSubDirectory(_baseDir, "override", true)).empty())
		overrideFiles.addDirectory(overrideDir, -1);

	addResources(overrideFiles, change, priority);

	return change;
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

const Common::FileList &ResourceManager::getZIPList() const {
	return _zipFiles;
}

ResourceManager::ResFileRef ResourceManager::findBIFPaths(const KEYFile &keyFile, ChangeID &change) {
	ResFileRef bifStart = _bifs.end();

	uint32 keyBIFCount = keyFile.getBIFs().size();

	// Go through all BIF names the KEY wants, trying to find a match in our BIF list
	for (uint32 i = 0; i < keyBIFCount; i++) {
		bool found = false;

		_bifs.push_back("");

		// Look through all BIF base directories
		for (std::vector<std::string>::const_iterator bifBase = _bifSourceDir.begin(); bifBase != _bifSourceDir.end(); ++bifBase) {
			// The BIF names in the KEY are relative to a BIF base directory
			_bifs.back() = Common::FilePath::normalize(*bifBase + "/" + keyFile.getBIFs()[i]);

			// Look through all our BIFs, looking for a match
			for (Common::FileList::const_iterator it = _bifFiles.begin(); it != _bifFiles.end(); ++it) {
				if (iequals(*it, _bifs.back())) {
					_bifs.back() = *it;
					found = true;
					break;
				}
			}

			if (found)
				break;
		}

		// Did we find it?
		if (!found)
			throw Common::Exception("BIF \"%s\" not found", _bifs.back().c_str());

		// Add the information of the new BIF to the change set
		change->bifs.push_back(--_bifs.end());

		// If we didn't yet remember the start of the BIF sequence, do it now
		if (bifStart == _bifs.end())
			--bifStart;
	}

	return bifStart;
}

void ResourceManager::mergeKEYBIFResources(const KEYFile &keyFile, const ResFileRef &bifStart,
		ChangeID &change, uint32 priority) {

	uint32 keyBIFCount = keyFile.getBIFs().size();

	std::vector<BIFFile> keyBIFFiles;

	keyBIFFiles.resize(keyBIFCount);

	// Try to load all needed BIF files
	try {

		ResFileList::const_iterator curBIF = bifStart;
		for (uint32 i = 0; i < keyBIFCount; i++, ++curBIF) {
			Common::File keyBIFFile;
			if (!keyBIFFile.open(*curBIF))
				throw Common::Exception("Can't open file \"%s\"", curBIF->c_str());

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
			res.priority = priority;
			res.source   = kSourceBIF;
			res.resFile  = bifStart;
			res.type     = keyRes->type;
			res.offset   = bifRes.offset;
			res.size     = bifRes.size;

			// Advance the resFile iterator
			for (uint32 i = 0; i < keyRes->bifIndex; i++, ++res.resFile);

			// And add it to our list
			addResource(res, keyRes->name, change);
		}

	} catch (Common::Exception &e) {
		e.add("Failed indexing resource");
		throw e;
	}

}

ResourceManager::ChangeID ResourceManager::loadKEY(Common::SeekableReadStream &key, uint32 priority) {
	if (_baseDir.empty())
		throw Common::Exception("No base data directory registered");

	KEYFile keyFile;

	keyFile.load(key);

	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	// Search for the correct BIFs
	ResFileRef bifStart = findBIFPaths(keyFile, change);

	// Merge the resource information of the KEY file and its BIF files into our resource map
	mergeKEYBIFResources(keyFile, bifStart, change, priority);

	return change;
}

ResourceManager::ChangeID ResourceManager::addERF(const std::string &erf, uint32 priority) {
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
		// Try to open from the textures directory
		erfFileName = _erfFiles.findFirst(Common::FilePath::normalize(_textureDir + "/" + erf), true);

	if (erfFileName.empty())
		// Does not exist
		throw Common::Exception("No such ERF");

	Common::File erfFile;
	if (!erfFile.open(erfFileName))
		throw Common::Exception(Common::kOpenError);

	ERFFile erfIndex;

	erfIndex.load(erfFile);

	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	_erfs.push_back(erfFileName);

	// Add the information of the new ERF to the change set
	change->erfs.push_back(--_erfs.end());

	const ERFFile::ResourceList &resources = erfIndex.getResources();
	for (ERFFile::ResourceList::const_iterator resource = resources.begin(); resource != resources.end(); ++resource) {
		// Build the resource record
		Resource res;
		res.priority = priority;
		res.source   = kSourceERF;
		res.resFile  = --_erfs.end();
		res.type     = resource->type;
		res.offset   = resource->offset;
		res.size     = resource->size;

		// And add it to our list
		addResource(res, resource->name, change);
	}

	return change;
}

ResourceManager::ChangeID ResourceManager::addRIM(const std::string &rim, uint32 priority) {
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
		throw Common::Exception(Common::kOpenError);

	RIMFile rimIndex;

	rimIndex.load(rimFile);

	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	_rims.push_back(rimFileName);

	// Add the information of the new RIM to the change set
	change->rims.push_back(--_rims.end());

	const RIMFile::ResourceList &resources = rimIndex.getResources();
	for (RIMFile::ResourceList::const_iterator resource = resources.begin(); resource != resources.end(); ++resource) {
		// Build the resource record
		Resource res;
		res.priority = priority;
		res.source   = kSourceRIM;
		res.resFile  = --_rims.end();
		res.type     = resource->type;
		res.offset   = resource->offset;
		res.size     = resource->size;

		// And add it to our list
		addResource(res, resource->name, change);
	}

	return change;
}

ResourceManager::ChangeID ResourceManager::addZIP(const std::string &zip, uint32 priority) {
	std::string zipFileName;

	if (Common::FilePath::isAbsolute(zip)) {
		// Absolute path to an ZIP, open it from our ZIP list

		zipFileName = _zipFiles.findFirst(Common::FilePath::normalize(zip), true);

		if (zipFileName.empty())
			// Does not exist
			throw Common::Exception("No such ZIP");
	}

	if (zipFileName.empty())
		// Try to open from the .zip directory
		zipFileName = _zipFiles.findFirst(Common::FilePath::normalize(_zipDir + "/" + zip), true);

	if (zipFileName.empty())
		// Does not exist
		throw Common::Exception("No such ZIP");

	Common::File *zipFile = new Common::File;
	if (!zipFile->open(zipFileName))
		throw Common::Exception(Common::kOpenError);

	Common::ZipFile zipIndex(zipFile);

	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	_zips.push_back(zipFileName);

	// Add the information of the new ZIP to the change set
	change->zips.push_back(--_zips.end());

	const Common::ZipFile::FileList &files = zipIndex.getFileList();
	for (Common::ZipFile::FileList::const_iterator file = files.begin(); file != files.end(); ++file) {
		// Build the resource record
		Resource res;
		res.priority = priority;
		res.source   = kSourceZIP;
		res.resFile  = --_zips.end();
		res.type     = getFileType(*file);
		res.path     = *file;

		// And add it to our list
		addResource(res, Common::FilePath::getStem(*file), change);
	}

	return change;
}

void ResourceManager::undo(ChangeID &change) {
	if (change == _changes.end())
		// Nothing to do
		return;

	// Go through all changes in the resource map
	for (std::list<ResourceChange>::iterator resChange = change->resources.begin(); resChange != change->resources.end(); ++resChange) {

		// Remove the resource
		resChange->typeIt->second.erase(resChange->resIt);

		// If the resource list is empty, remove that one too
		if (resChange->typeIt->second.empty())
			resChange->nameIt->second.erase(resChange->typeIt);

		// And if the type map is empty too, remove that one as well
		if (resChange->nameIt->second.empty())
			_resources.erase(resChange->nameIt);
	}

	// Removing all changes in the BIF list
	for (std::list<ResFileList::iterator>::iterator bifChange = change->bifs.begin(); bifChange != change->bifs.end(); ++bifChange)
		_bifs.erase(*bifChange);

	// Removing all changes in the ERF list
	for (std::list<ResFileList::iterator>::iterator erfChange = change->erfs.begin(); erfChange != change->erfs.end(); ++erfChange)
		_erfs.erase(*erfChange);

	// Removing all changes in the RIM list
	for (std::list<ResFileList::iterator>::iterator rimChange = change->rims.begin(); rimChange != change->rims.end(); ++rimChange)
		_rims.erase(*rimChange);

	// Removing all changes in the ZIP list
	for (std::list<ResFileList::iterator>::iterator zipChange = change->zips.begin(); zipChange != change->zips.end(); ++zipChange)
		_zips.erase(*zipChange);

	// Now we can remove the change set from our list of change sets
	_changes.erase(change);

	// And finally set the change ID to a defined empty state
	change = _changes.end();
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

Common::SeekableReadStream *ResourceManager::getOffResFile(const Resource &res) const {
	// Read the data out of the file and return a MemoryReadStream

	if (res.resFile == _bifs.end())
		return 0;

	Common::File file;
	if (!file.open(*res.resFile))
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

Common::SeekableReadStream *ResourceManager::getZipResFile(const Resource &res) const {
	if (res.resFile == _bifs.end())
		return 0;

	Common::File *file = new Common::File;
	if (!file->open(*res.resFile))
		return 0;

	Common::ZipFile zip(file);

	return zip.open(res.path);
}

Common::SeekableReadStream *ResourceManager::getResource(const std::string &name, FileType type) const {
	std::vector<FileType> types;

	types.push_back(type);

	return getResource(name, types);
}

Common::SeekableReadStream *ResourceManager::getResource(const std::string &name,
		const std::vector<FileType> &types, FileType *foundType) const {

	const Resource *res = getRes(name, types);
	if (!res)
		return 0;

	// Return the actually found type
	if (foundType)
		*foundType = res->type;

	if        (res->source == kSourceBIF) {
		// Read the data out of the BIF and return a MemoryReadStream
		return getOffResFile(*res);
	} else if (res->source == kSourceERF) {
		return getOffResFile(*res);
	} else if (res->source == kSourceRIM) {
		return getOffResFile(*res);
	} else if (res->source == kSourceZIP) {
		return getZipResFile(*res);
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

Common::SeekableReadStream *ResourceManager::getMusic(const std::string &name, FileType *type) const {
	// Try every known music file type
	Common::SeekableReadStream *res;
	if ((res = getResource(name, _musicTypes, type)))
		return res;

	// No such music
	return 0;
}

Common::SeekableReadStream *ResourceManager::getSound(const std::string &name, FileType *type) const {
	// Try every known sound file type
	Common::SeekableReadStream *res;
	if ((res = getResource(name, _soundTypes, type)))
		return res;

	// No such sound
	return 0;
}

Common::SeekableReadStream *ResourceManager::getImage(const std::string &name, FileType *type) const {
	// Try every known image file type
	Common::SeekableReadStream *res;
	if ((res = getResource(name, _imageTypes, type)))
		return res;

	// No such image
	return 0;
}

Common::SeekableReadStream *ResourceManager::getVideo(const std::string &name, FileType *type) const {
	// Try every known video file type
	Common::SeekableReadStream *res;
	if ((res = getResource(name, _videoTypes, type)))
		return res;

	// No such video
	return 0;
}

void ResourceManager::addResource(const Resource &resource, std::string name, ChangeID &change) {
	boost::to_lower(name);
	if (name.empty())
		return;

	ResourceMap::iterator resTypeMap = _resources.find(name);
	if (resTypeMap == _resources.end()) {
		// We don't yet have a resource with this name, create a new type map for it

		std::pair<ResourceMap::iterator, bool> result;

		result = _resources.insert(std::make_pair(name, ResourceTypeMap()));

		resTypeMap = result.first;
	}

	ResourceTypeMap::iterator resList = resTypeMap->second.find(resource.type);
	if (resList == resTypeMap->second.end()) {
		// We don't yet have a resource with that name and type, create a new resource list for it

		std::pair<ResourceTypeMap::iterator, bool> result;

		result = resTypeMap->second.insert(std::make_pair(resource.type, ResourceList()));

		resList = result.first;
	}

	// Add the resource to the list
	resList->second.push_back(resource);

	// And sort the list by priority
	resList->second.sort();

	// Remember the resource in the change set
	change->resources.push_back(ResourceChange());
	change->resources.back().nameIt = resTypeMap;
	change->resources.back().typeIt = resList;
	change->resources.back().resIt  = --resList->second.end();
}

void ResourceManager::addResources(const Common::FileList &files, ChangeID &change, uint32 priority) {
	for (Common::FileList::const_iterator file = files.begin(); file != files.end(); ++file) {
		Resource res;
		res.priority = priority;
		res.source   = kSourceFile;
		res.path     = *file;
		res.type     = getFileType(*file);
		res.resFile  = _bifs.end();
		res.offset   = 0xFFFFFFFF;
		res.size     = 0xFFFFFFFF;

		addResource(res, Common::FilePath::getStem(*file), change);
	}
}

const ResourceManager::Resource *ResourceManager::getRes(std::string name,
		const std::vector<FileType> &types) const {

	boost::to_lower(name);

	// Find the resources with the same name
	ResourceMap::const_iterator resFamily = _resources.find(name);
	if (resFamily == _resources.end())
		return 0;

	for (std::vector<FileType>::const_iterator type = types.begin(); type != types.end(); ++type) {
		// Find the specific resource of the given type
		ResourceTypeMap::const_iterator res = resFamily->second.find(*type);
		if ((res != resFamily->second.end()) && !res->second.empty())
			return &res->second.back();
	}

	return 0;
}

void ResourceManager::listResources() const {
	for (ResourceMap::const_iterator itName = _resources.begin(); itName != _resources.end(); ++itName) {
		for (ResourceTypeMap::const_iterator itType = itName->second.begin(); itType != itName->second.end(); ++itType) {
			if (itType->second.empty())
				continue;

			const Resource &resource = itType->second.back();

			status("%32s%4s %10d", itName->first.c_str(), setFileType("", resource.type).c_str(), resource.size);
		}
	}
}

} // End of namespace Aurora
