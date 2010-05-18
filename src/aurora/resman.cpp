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

#include "aurora/resman.h"
#include "aurora/util.h"
#include "aurora/error.h"

#include "aurora/keyfile.h"
#include "aurora/biffile.h"
#include "aurora/erffile.h"
#include "aurora/rimfile.h"
#include "aurora/ndsrom.h"
#include "aurora/zipfile.h"

// boost-string_algo
using boost::iequals;

DECLARE_SINGLETON(Aurora::ResourceManager)

static const char *kArchiveGlob[Aurora::kArchiveMAX] = {
	".*\\.key", ".*\\.bif", ".*\\.(erf|mod|hak)", ".*\\.rim", ".*\\.zip"
};

namespace Aurora {

ResourceManager::Resource::Resource() : type(kFileTypeNone), priority(0),
		source(kSourceNone), archive(0), archiveIndex(0xFFFFFFFF) {
}

bool ResourceManager::Resource::operator<(const Resource &right) const {
	return priority < right.priority;
}


ResourceManager::ResourceManager() : _rimsAreERFs(false) {
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

	for (ArchiveList::iterator archive = _archives.begin(); archive != _archives.end(); ++archive)
		delete *archive;
	_archives.end();

	_resources.clear();

	_changes.clear();
}

void ResourceManager::setRIMsAreERFs(bool rimsAreERFs) {
	_rimsAreERFs = rimsAreERFs;
}

void ResourceManager::registerDataBaseDir(const Common::UString &path) {
	clear();

	_baseDir = Common::FilePath::normalize(path);

	for (int i = 0; i < kArchiveMAX; i++)
		addArchiveDir((ArchiveType) i, "");
}

void ResourceManager::addArchiveDir(ArchiveType archive, const Common::UString &dir) {
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

	// If we're adding an ERF directory and .rim files are actually ERFs, add those too
	if ((archive == kArchiveERF) && _rimsAreERFs)
		dirFiles.getSubList(kArchiveGlob[kArchiveRIM], _archiveFiles[archive], true);

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

ResourceManager::ChangeID ResourceManager::addArchive(ArchiveType archive,
		const Common::UString &file, uint32 priority) {

	// NDS aren't found in resource directories, they are used /instead/ of directories
	if (archive == kArchiveNDS) {
		// Generate a new change set
		_changes.push_back(ChangeSet());
		ChangeID change = --_changes.end();

		NDSFile *nds = new NDSFile(file);

		return indexArchive(nds, priority, change);
	}

	assert((archive >= 0) && (archive < kArchiveMAX));

	if (archive == kArchiveBIF)
		throw Common::Exception("Attempted to index a lone BIF");

	Common::UString realName = findArchive(file, _archiveDirs[archive], _archiveFiles[archive]);
	if (realName.empty())
		throw Common::Exception("No such archive file \"%s\"", file.c_str());

	if (archive == kArchiveKEY)
		return indexKEY(realName, priority);

	if (archive == kArchiveERF) {
		ERFFile *erf = new ERFFile(realName);

		// Generate a new change set
		_changes.push_back(ChangeSet());
		ChangeID change = --_changes.end();

		return indexArchive(erf, priority, change);
	}

	if (archive == kArchiveRIM) {
		RIMFile *rim = new RIMFile(realName);

		// Generate a new change set
		_changes.push_back(ChangeSet());
		ChangeID change = --_changes.end();

		return indexArchive(rim, priority, change);
	}

	if (archive == kArchiveZIP) {
		ZIPFile *zip = new ZIPFile(realName);

		// Generate a new change set
		_changes.push_back(ChangeSet());
		ChangeID change = --_changes.end();

		return indexArchive(zip, priority, change);
	}

	return _changes.end();
}

void ResourceManager::findBIFs(const KEYFile &key, std::vector<Common::UString> &bifs) {
	const KEYFile::BIFList &keyBIFs = key.getBIFs();

	bifs.resize(keyBIFs.size());

	KEYFile::BIFList::const_iterator       keyBIF = keyBIFs.begin();
	std::vector<Common::UString>::iterator bif    = bifs.begin();
	for (; (keyBIF != keyBIFs.end()) && (bif != bifs.end()); ++keyBIF, ++bif) {

		*bif = findArchive(*keyBIF, _archiveDirs[kArchiveBIF], _archiveFiles[kArchiveBIF]);
		if (bif->empty())
			throw Common::Exception("BIF \"%s\" not found", keyBIF->c_str());

	}
}

void ResourceManager::mergeKEYBIF(const KEYFile &key, std::vector<Common::UString> &bifs,
		std::vector<BIFFile *> &bifFiles) {

	bifFiles.reserve(bifs.size());

	BIFFile *curBIF = 0;

	// Try to load all needed BIF files
	try {

		uint32 index = 0;
		for (std::vector<Common::UString>::const_iterator bif = bifs.begin(); bif != bifs.end(); ++index, ++bif) {
			curBIF = new BIFFile(*bif);

			curBIF->mergeKEY(key, index);

			bifFiles.push_back(curBIF);
		}

	} catch (Common::Exception &e) {
		delete curBIF;

		e.add("Failed opening needed BIFs");
		throw e;
	}

}

ResourceManager::ChangeID ResourceManager::indexKEY(const Common::UString &file, uint32 priority) {
	KEYFile key(file);

	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	// Search the correct BIFs
	std::vector<Common::UString> bifs;
	findBIFs(key, bifs);

	std::vector<BIFFile *> bifFiles;
	mergeKEYBIF(key, bifs, bifFiles);

	for (std::vector<BIFFile *>::iterator bifFile = bifFiles.begin(); bifFile != bifFiles.end(); ++bifFile)
		indexArchive(*bifFile, priority, change);

	return change;
}

ResourceManager::ChangeID ResourceManager::indexArchive(Archive *archive, uint32 priority, ChangeID &change) {
	_archives.push_back(archive);

	// Add the information of the new archive to the change set
	change->archives.push_back(--_archives.end());

	const Archive::ResourceList &resources = archive->getResources();
	for (Archive::ResourceList::const_iterator resource = resources.begin(); resource != resources.end(); ++resource) {
		// Build the resource record
		Resource res;
		res.priority     = priority;
		res.source       = kSourceArchive;
		res.archive      = archive;
		res.archiveIndex = resource->index;
		res.type         = resource->type;

		// And add it to our list
		addResource(res, resource->name, change);
	}

	archive->clear();

	return change;
}

ResourceManager::ChangeID ResourceManager::addResourceDir(const Common::UString &dir,
		const char *glob, int depth, uint32 priority) {

	// Find the directory
	Common::UString directory = Common::FilePath::findSubDirectory(_baseDir, dir, true);
	if (directory.empty())
		throw Common::Exception("No such directory \"%s\"", dir.c_str());

	// Find files
	Common::FileList files;
	files.addDirectory(directory, depth);

	// Generate a new change set
	_changes.push_back(ChangeSet());
	ChangeID change = --_changes.end();

	if (!glob) {
		// Add the files
		addResources(files, change, priority);
		return change;
	}

	// Find files matching the glob pattern
	Common::FileList globFiles;
	files.getSubList(glob, globFiles, true);

	// Add the files
	addResources(globFiles, change, priority);
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

	// Removing all changes in the archive list
	for (std::list<ArchiveList::iterator>::iterator archiveChange = change->archives.begin(); archiveChange != change->archives.end(); ++archiveChange) {
		delete **archiveChange;
		_archives.erase(*archiveChange);
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

Common::SeekableReadStream *ResourceManager::getArchiveResource(const Resource &res) const {
	if ((res.archive == 0) || (res.archiveIndex == 0xFFFFFFFF))
		throw Common::Exception("Archive resource has no archive");

	return res.archive->getResource(res.archiveIndex);
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

	if        (res->source == kSourceNone) {
		throw Common::Exception("Invalid resource source");
	} else if (res->source == kSourceArchive) {
		return getArchiveResource(*res);
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

			status("%32s%4s", itName->first.c_str(), setFileType("", resource.type).c_str());
		}
	}
}

} // End of namespace Aurora
