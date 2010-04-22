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
#include "aurora/ndsrom.h"

// boost-string_algo
using boost::iequals;

DECLARE_SINGLETON(Aurora::ResourceManager)

static const char *kArchiveGlob[Aurora::kArchiveMAX] = {
	".*\\.key", ".*\\.bif", ".*\\.(erf|mod|hak)", ".*\\.rim", ".*\\.zip"
};

namespace Aurora {

bool ResourceManager::Resource::operator<(const Resource &right) const {
	return priority < right.priority;
}


ResourceManager::ResourceManager() {
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeDDS);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeTPC);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeTGA);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypePNG);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeBMP);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeJPG);

	_resourceTypeTypes[kResourceVideo].push_back(kFileTypeBIK);
	_resourceTypeTypes[kResourceVideo].push_back(kFileTypeMPG);
	_resourceTypeTypes[kResourceVideo].push_back(kFileTypeWMV);

	_resourceTypeTypes[kResourceSound].push_back(kFileTypeWAV);
	_resourceTypeTypes[kResourceSound].push_back(kFileTypeOGG);

	_resourceTypeTypes[kResourceMusic].push_back(kFileTypeWAV);
	_resourceTypeTypes[kResourceMusic].push_back(kFileTypeBMU);
	_resourceTypeTypes[kResourceMusic].push_back(kFileTypeOGG);
}

ResourceManager::~ResourceManager() {
	clear();

	for (int i = 0; i < kResourceMAX; i++)
		_resourceTypeTypes[i].clear();
}

void ResourceManager::clear() {
	_baseDir.clear();

	for (int i = 0; i < kArchiveMAX; i++) {
		_archiveDirs[i].clear();
		_archiveFiles[i].clear();
	}

	_bifs.clear();
	_erfs.clear();
	_rims.clear();

	for (ResZipList::iterator zip = _zips.begin(); zip != _zips.end(); ++zip)
		delete *zip;
	_zips.clear();

	for (ResNDSList::iterator nds = _ndss.begin(); nds != _ndss.end(); ++nds)
		delete *nds;
	_ndss.clear();

	_resources.clear();

	_changes.clear();
}

void ResourceManager::registerDataBaseDir(const Common::UString &path) {
	clear();

	_baseDir = Common::FilePath::normalize(path);

	for (int i = 0; i < kArchiveMAX; i++)
		addArchiveDir((Archive) i, "");
}

void ResourceManager::addArchiveDir(Archive archive, const Common::UString &dir) {
	if (archive == kArchiveNDS)
		return;

	assert((archive >= 0) && (archive < kArchiveMAX));

	Common::UString directory = Common::FilePath::findSubDirectory(_baseDir, dir, true);
	if (directory.empty())
		throw Common::Exception("No such directory \"%s\"", dir.c_str());

	Common::FileList dirFiles;
	if (!dirFiles.addDirectory(directory))
		throw Common::Exception("Can't read directory \"%s\"", directory.c_str());

	dirFiles.getSubList(kArchiveGlob[archive], _archiveFiles[archive], true);
	_archiveDirs[archive].push_back(directory);
}

Common::UString ResourceManager::findArchive(const Common::UString &file,
		const DirectoryList &dirs, const Common::FileList &files) {

	Common::FileList nameMatch;
	if (!files.getSubList(".*/" + file, nameMatch, true))
		return "";

	Common::UString realName;
	for (DirectoryList::const_iterator dir = dirs.begin(); dir != dirs.end(); ++dir)
		if (!(realName = nameMatch.findFirst(Common::FilePath::normalize(*dir + "/" + file), true)).empty())
			return realName;

	return "";
}

ResourceManager::ChangeID ResourceManager::addArchive(Archive archive,
		const Common::UString &file, uint32 priority) {

	if (archive == kArchiveNDS)
		return indexNDS(file, priority);

	assert((archive >= 0) && (archive < kArchiveMAX));

	if (archive == kArchiveBIF)
		throw Common::Exception("Attempted to index a lone BIF");

	Common::UString realName = findArchive(file, _archiveDirs[archive], _archiveFiles[archive]);
	if (realName.empty())
		throw Common::Exception("No such archive file \"%s\"", file.c_str());

	if (archive == kArchiveKEY)
		return indexKEY(realName, priority);
	if (archive == kArchiveERF)
		return indexERF(realName, priority);
	if (archive == kArchiveRIM)
		return indexRIM(realName, priority);
	if (archive == kArchiveZIP)
		return indexZIP(realName, priority);

	return _changes.end();
}

ResourceManager::ResFileRef ResourceManager::findBIFPaths(const KEYFile &keyFile, ChangeID &change) {
	ResFileRef bifStart = _bifs.end();

	uint32 keyBIFCount = keyFile.getBIFs().size();

	// Go through all BIF names the KEY wants, trying to find a match in our BIF list
	for (uint32 i = 0; i < keyBIFCount; i++) {
		Common::UString realName = findArchive(keyFile.getBIFs()[i], _archiveDirs[kArchiveBIF], _archiveFiles[kArchiveBIF]);
		if (realName.empty())
			throw Common::Exception("BIF \"%s\" not found", keyFile.getBIFs()[i].c_str());

		_bifs.push_back(realName);

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
			addResource(res, keyRes->name.c_str(), change);
		}

	} catch (Common::Exception &e) {
		e.add("Failed indexing resource");
		throw e;
	}

}

ResourceManager::ChangeID ResourceManager::indexKEY(const Common::UString &file, uint32 priority) {
	Common::File keyFile;
	if (!keyFile.open(file))
		throw Common::Exception(Common::kOpenError);

	KEYFile keyIndex;

	keyIndex.load(keyFile);

	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	// Search for the correct BIFs
	ResFileRef bifStart = findBIFPaths(keyIndex, change);

	// Merge the resource information of the KEY file and its BIF files into our resource map
	mergeKEYBIFResources(keyIndex, bifStart, change, priority);

	return change;
}

ResourceManager::ChangeID ResourceManager::indexERF(const Common::UString &file, uint32 priority) {
	Common::File erfFile;
	if (!erfFile.open(file))
		throw Common::Exception(Common::kOpenError);

	ERFFile erfIndex;

	erfIndex.load(erfFile);

	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	_erfs.push_back(file);

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

ResourceManager::ChangeID ResourceManager::indexRIM(const Common::UString &file, uint32 priority) {
	Common::File rimFile;
	if (!rimFile.open(file))
		throw Common::Exception(Common::kOpenError);

	RIMFile rimIndex;

	rimIndex.load(rimFile);

	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	_rims.push_back(file);

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

ResourceManager::ChangeID ResourceManager::indexZIP(const Common::UString &file, uint32 priority) {
	Common::File *zipFile = new Common::File;
	if (!zipFile->open(file))
		throw Common::Exception(Common::kOpenError);

	Common::ZipFile *zipIndex = new Common::ZipFile(zipFile);

	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	_zips.push_back(zipIndex);

	// Add the information of the new ZIP to the change set
	change->zips.push_back(--_zips.end());

	const Common::ZipFile::FileList &files = zipIndex->getFileList();
	for (Common::ZipFile::FileList::const_iterator file = files.begin(); file != files.end(); ++file) {
		// Build the resource record
		Resource res;
		res.priority = priority;
		res.source   = kSourceZIP;
		res.resZip   = --_zips.end();
		res.type     = getFileType(*file);
		res.path     = *file;

		// And add it to our list
		addResource(res, Common::FilePath::getStem(*file), change);
	}

	return change;
}

ResourceManager::ChangeID ResourceManager::indexNDS(const Common::UString &file, uint32 priority) {
	Common::File *ndsFile = new Common::File;
	if (!ndsFile->open(file))
		throw Common::Exception(Common::kOpenError);

	NDSFile *ndsIndex = new NDSFile(ndsFile);

	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	_ndss.push_back(ndsIndex);

	// Add the information of the new NDS to the change set
	change->ndss.push_back(--_ndss.end());

	const NDSFile::FileList &files = ndsIndex->getFileList();
	for (NDSFile::FileList::const_iterator file = files.begin(); file != files.end(); ++file) {
		// Build the resource record
		Resource res;
		res.priority = priority;
		res.source   = kSourceNDS;
		res.resNDS   = --_ndss.end();
		res.type     = getFileType(*file);
		res.path     = *file;

		// And add it to our list
		addResource(res, Common::FilePath::getStem(*file), change);
	}

	return change;
}

ResourceManager::ChangeID ResourceManager::loadSecondaryResources(uint32 priority) {
	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	// Find all music files

	Common::FileList musicFiles;
	Common::UString musicDir;
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

	Common::UString soundDir;
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

	Common::UString videoDir;
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

	Common::UString overrideDir;
	if (!(overrideDir = Common::FilePath::findSubDirectory(_baseDir, "override", true)).empty())
		overrideFiles.addDirectory(overrideDir, -1);

	addResources(overrideFiles, change, priority);

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
	for (std::list<ResZipList::iterator>::iterator zipChange = change->zips.begin(); zipChange != change->zips.end(); ++zipChange) {
		delete **zipChange;
		_zips.erase(*zipChange);
	}

	// Removing all changes in the NDS list
	for (std::list<ResNDSList::iterator>::iterator ndsChange = change->ndss.begin(); ndsChange != change->ndss.end(); ++ndsChange) {
		delete **ndsChange;
		_ndss.erase(*ndsChange);
	}

	// Now we can remove the change set from our list of change sets
	_changes.erase(change);

	// And finally set the change ID to a defined empty state
	change = _changes.end();
}

bool ResourceManager::hasResource(const Common::UString &name, FileType type) const {
	std::vector<FileType> types;

	types.push_back(type);

	return hasResource(name, types);
}

bool ResourceManager::hasResource(const Common::UString &name, const std::vector<FileType> &types) const {
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
	if ((res.resZip == _zips.end() || !*res.resZip))
		return 0;

	return (*res.resZip)->open(res.path);
}

Common::SeekableReadStream *ResourceManager::getNDSResFile(const Resource &res) const {
	if ((res.resNDS == _ndss.end() || !*res.resNDS))
		return 0;

	return (*res.resNDS)->open(res.path);
}

Common::SeekableReadStream *ResourceManager::getResource(const Common::UString &name, FileType type) const {
	std::vector<FileType> types;

	types.push_back(type);

	return getResource(name, types);
}

Common::SeekableReadStream *ResourceManager::getResource(const Common::UString &name,
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
	} else if (res->source == kSourceNDS) {
		return getNDSResFile(*res);
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

Common::SeekableReadStream *ResourceManager::getResource(ResourceType resType,
		const Common::UString &name, FileType *foundType) const {

	assert((resType >= 0) && (resType < kResourceMAX));

	// Try every known file type for that resource type
	Common::SeekableReadStream *res;
	if ((res = getResource(name, _resourceTypeTypes[resType], foundType)))
		return res;

	// No such resource
	return 0;
}

void ResourceManager::addResource(const Resource &resource, Common::UString name, ChangeID &change) {
	name.tolower();
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
		res.resZip   = _zips.end();
		res.resNDS   = _ndss.end();
		res.offset   = 0xFFFFFFFF;
		res.size     = 0xFFFFFFFF;

		addResource(res, Common::FilePath::getStem(*file), change);
	}
}

const ResourceManager::Resource *ResourceManager::getRes(Common::UString name,
		const std::vector<FileType> &types) const {

	name.tolower();

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
