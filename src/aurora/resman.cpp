/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/resman.cpp
 *  The global resource manager for Aurora resources.
 */

#include <boost/algorithm/string.hpp>

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
#include "aurora/pefile.h"
#include "aurora/herffile.h"

// Check for hash collisions (if possible)
#define CHECK_HASH_COLLISION 1

// boost-string_algo
using boost::iequals;

DECLARE_SINGLETON(Aurora::ResourceManager)

static const char *kArchiveGlob[Aurora::kArchiveMAX] = {
	".*\\.key", ".*\\.bif", ".*\\.(erf|mod|hak|nwm)", ".*\\.rim", ".*\\.zip", ".*\\.exe"
};

namespace Aurora {

ResourceManager::Resource::Resource() : type(kFileTypeNone), priority(0),
		source(kSourceNone), archive(0), archiveIndex(0xFFFFFFFF) {
}


bool ResourceManager::Resource::operator<(const Resource &right) const {
	return priority < right.priority;
}


ResourceManager::ChangeID::ChangeID() : _empty(true) {
}

bool ResourceManager::ChangeID::empty() const {
	return _empty;
}

void ResourceManager::ChangeID::clear() {
	_empty = true;
}

ResourceManager::ChangeID::ChangeID(ChangeSetList::iterator c) : _empty(false), _change(c) {
}


ResourceManager::ResourceManager() : _rimsAreERFs(false), _hashAlgo(Common::kHashFNV64) {
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeDDS);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeTPC);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeTXB);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeTGA);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypePNG);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeBMP);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeJPG);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeSBM);

	_resourceTypeTypes[kResourceVideo].push_back(kFileTypeBIK);
	_resourceTypeTypes[kResourceVideo].push_back(kFileTypeMPG);
	_resourceTypeTypes[kResourceVideo].push_back(kFileTypeWMV);
	_resourceTypeTypes[kResourceVideo].push_back(kFileTypeMOV);
	_resourceTypeTypes[kResourceVideo].push_back(kFileTypeXMV);
	_resourceTypeTypes[kResourceVideo].push_back(kFileTypeVX);

	_resourceTypeTypes[kResourceSound].push_back(kFileTypeWAV);
	_resourceTypeTypes[kResourceSound].push_back(kFileTypeOGG);
	_resourceTypeTypes[kResourceSound].push_back(kFileTypeWMA);

	_resourceTypeTypes[kResourceMusic].push_back(kFileTypeWAV);
	_resourceTypeTypes[kResourceMusic].push_back(kFileTypeBMU);
	_resourceTypeTypes[kResourceMusic].push_back(kFileTypeOGG);
	_resourceTypeTypes[kResourceMusic].push_back(kFileTypeWMA);

	_resourceTypeTypes[kResourceCursor].push_back(kFileTypeCUR);
	_resourceTypeTypes[kResourceCursor].push_back(kFileTypeCURS);
	_resourceTypeTypes[kResourceCursor].push_back(kFileTypeDDS);
	_resourceTypeTypes[kResourceCursor].push_back(kFileTypeTGA);
}

ResourceManager::~ResourceManager() {
	clear();

	for (int i = 0; i < kResourceMAX; i++)
		_resourceTypeTypes[i].clear();
}

void ResourceManager::clear() {
	_rimsAreERFs = false;
	_hashAlgo    = Common::kHashFNV64;

	clearResources();
}

void ResourceManager::clearResources() {
	_cursorRemap.clear();

	_baseDir.clear();

	for (int i = 0; i < kArchiveMAX; i++) {
		_archiveDirs[i].clear();
		_archiveFiles[i].clear();
	}

	for (ArchiveList::iterator archive = _archives.begin(); archive != _archives.end(); ++archive)
		delete *archive;
	_archives.clear();

	_resources.clear();

	_typeAliases.clear();

	_changes.clear();
}

void ResourceManager::setRIMsAreERFs(bool rimsAreERFs) {
	_rimsAreERFs = rimsAreERFs;
}

void ResourceManager::setHashAlgo(Common::HashAlgo algo) {
	if ((algo != _hashAlgo) && !_resources.empty())
		throw Common::Exception("ResourceManager::setHashAlgo(): We already have resources!");

	_hashAlgo = algo;
}

void ResourceManager::setCursorRemap(const std::vector<Common::UString> &remap) {
	_cursorRemap = remap;
}

void ResourceManager::registerDataBaseDir(const Common::UString &path) {
	clearResources();

	_baseDir = Common::FilePath::normalize(path);

	for (int i = 0; i < kArchiveMAX; i++)
		addArchiveDir((ArchiveType) i, "");
}

void ResourceManager::addArchiveDir(ArchiveType archive, const Common::UString &dir) {
	if (archive == kArchiveNDS || archive == kArchiveHERF)
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

	Common::UString escapedFile = Common::FilePath::escapeStringLiteral(file);
	Common::FileList nameMatch;
	if (!files.getSubList(".*/" + escapedFile, nameMatch, true))
		return "";

	Common::UString realName;
	for (DirectoryList::const_iterator dir = dirs.begin(); dir != dirs.end(); ++dir) {
		Common::UString escapedPath = Common::FilePath::escapeStringLiteral(Common::FilePath::normalize(*dir)) + "/" + escapedFile;
		if (!(realName = nameMatch.findFirst(escapedPath, true)).empty())
			return realName;
	}

	return "";
}

bool ResourceManager::hasArchive(ArchiveType archive, const Common::UString &file) {
	assert((archive >= 0) && (archive < kArchiveMAX));

	if (archive == kArchiveNDS)
		return Common::File::exists(file);

	return !findArchive(file, _archiveDirs[archive], _archiveFiles[archive]).empty();
}

ResourceManager::ChangeID ResourceManager::addArchive(ArchiveType archive,
		const Common::UString &file, uint32 priority) {

	// NDS aren't found in resource directories, they are used /instead/ of directories
	if (archive == kArchiveNDS) {
		NDSFile *nds = new NDSFile(file);

		ChangeID change = newChangeSet();

		return indexArchive(nds, priority, change);
	}

	// HERF files are only found inside NDS files
	if (archive == kArchiveHERF) {
		HERFFile *herf = new HERFFile(file);

		ChangeID change = newChangeSet();

		return indexArchive(herf, priority, change);
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

		ChangeID change = newChangeSet();

		return indexArchive(erf, priority, change);
	}

	if (archive == kArchiveRIM) {
		RIMFile *rim = new RIMFile(realName);

		ChangeID change = newChangeSet();

		return indexArchive(rim, priority, change);
	}

	if (archive == kArchiveZIP) {
		ZIPFile *zip = new ZIPFile(realName);

		ChangeID change = newChangeSet();

		return indexArchive(zip, priority, change);
	}

	if (archive == kArchiveEXE) {
		PEFile *pe = new PEFile(realName, _cursorRemap);

		ChangeID change = newChangeSet();

		return indexArchive(pe, priority, change);
	}

	return ChangeID();
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
		throw;
	}

}

ResourceManager::ChangeID ResourceManager::indexKEY(const Common::UString &file, uint32 priority) {
	KEYFile key(file);

	// Search the correct BIFs
	std::vector<Common::UString> bifs;
	findBIFs(key, bifs);

	std::vector<BIFFile *> bifFiles;
	mergeKEYBIF(key, bifs, bifFiles);

	ChangeID change = newChangeSet();

	for (std::vector<BIFFile *>::iterator bifFile = bifFiles.begin(); bifFile != bifFiles.end(); ++bifFile)
		indexArchive(*bifFile, priority, change);

	return change;
}

ResourceManager::ChangeID ResourceManager::indexArchive(Archive *archive, uint32 priority, ChangeID &change) {
	const Common::HashAlgo hashAlgo = archive->getNameHashAlgo();
	if ((hashAlgo != Common::kHashNone) && (hashAlgo != _hashAlgo))
		throw Common::Exception("ResourceManager::indexArchive(): Archive uses a different name hashing "
		                        "algorithm than we do (%d vs. %d)", (int) hashAlgo, (int) _hashAlgo);

	_archives.push_back(archive);

	// Add the information of the new archive to the change set
	change._change->archives.push_back(--_archives.end());

	const Archive::ResourceList &resources = archive->getResources();
	for (Archive::ResourceList::const_iterator resource = resources.begin(); resource != resources.end(); ++resource) {
		// Build the resource record
		Resource res;
		res.priority     = priority;
		res.source       = kSourceArchive;
		res.archive      = archive;
		res.archiveIndex = resource->index;
		res.name         = resource->name;
		res.type         = resource->type;

		// And add it to our list
		if (hashAlgo == Common::kHashNone)
			addResource(res, TypeMan.setFileType(resource->name, resource->type), change);
		else
			addResource(res, resource->hash, change);
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

	ChangeID change = newChangeSet();

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
	if (change.empty() || (change._change == _changes.end()))
		// Nothing to do
		return;

	// Go through all changes in the resource map
	for (std::list<ResourceChange>::iterator resChange = change._change->resources.begin();
	     resChange != change._change->resources.end(); ++resChange) {

		// Remove the resource, and the name list too if it's empty
		resChange->hashIt->second.erase(resChange->resIt);
		if (resChange->hashIt->second.empty())
			_resources.erase(resChange->hashIt);
	}

	// Removing all changes in the archive list
	for (std::list<ArchiveList::iterator>::iterator archiveChange = change._change->archives.begin();
	     archiveChange != change._change->archives.end(); ++archiveChange) {

		delete **archiveChange;
		_archives.erase(*archiveChange);
	}

	// Now we can remove the change set from our list of change sets
	_changes.erase(change._change);

	// And finally set the change ID to a defined empty state
	change._empty  = true;
	change._change = _changes.end();
}

void ResourceManager::addTypeAlias(FileType alias, FileType realType) {
	_typeAliases[alias] = realType;
}

void ResourceManager::blacklist(const Common::UString &name, FileType type) {
	ResourceMap::iterator resList = _resources.find(getHash(name, type));
	if (resList == _resources.end())
		return;

	for (ResourceList::iterator res = resList->second.begin(); res != resList->second.end(); ++res)
		res->priority = 0;
}

void ResourceManager::declareResource(const Common::UString &name, FileType type) {
	ResourceMap::iterator resList = _resources.find(getHash(name, type));
	if (resList == _resources.end())
		return;

	for (ResourceList::iterator r = resList->second.begin(); r != resList->second.end(); ++r) {
		r->name = name;
		r->type = type;
	}
}

void ResourceManager::declareResource(const Common::UString &name) {
	declareResource(TypeMan.setFileType(name, kFileTypeNone), TypeMan.getFileType(name));
}

bool ResourceManager::hasResource(const Common::UString &name, FileType type) const {
	std::vector<FileType> types;

	types.push_back(type);

	return hasResource(name, types);
}

bool ResourceManager::hasResource(const Common::UString &name) const {
	return hasResource(TypeMan.setFileType(name, kFileTypeNone), TypeMan.getFileType(name));
}

bool ResourceManager::hasResource(const Common::UString &name, const std::vector<FileType> &types) const {
	if (getRes(name, types))
		return true;

	return false;
}

uint32 ResourceManager::getResourceSize(const Resource &res) const {
	if (res.source == kSourceArchive) {
		if ((res.archive == 0) || (res.archiveIndex == 0xFFFFFFFF))
			return 0xFFFFFFFF;

		return res.archive->getResourceSize(res.archiveIndex);
	}

	if (res.source == kSourceFile)
		return Common::FilePath::getFileSize(res.path);

	return 0xFFFFFFFF;
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

Common::SeekableReadStream *ResourceManager::getResource(const Common::UString &name) const {
	return getResource(TypeMan.setFileType(name, kFileTypeNone), TypeMan.getFileType(name));
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

void ResourceManager::getAvailableResources(FileType type,
		std::list<ResourceID> &list) const {

	for (ResourceMap::const_iterator r = _resources.begin(); r != _resources.end(); ++r) {
		if (!r->second.empty() && (r->second.front().type == type)) {
			list.push_back(ResourceID());

			list.back().name = r->second.front().name;
			list.back().type = r->second.front().type;
		}
	}
}

void ResourceManager::getAvailableResources(const std::vector<FileType> &types,
		std::list<ResourceID> &list) const {

	for (ResourceMap::const_iterator r = _resources.begin(); r != _resources.end(); ++r) {
		for (std::vector<FileType>::const_iterator t = types.begin(); t != types.end(); ++t) {
			if (!r->second.empty() && (r->second.front().type == *t)) {
				list.push_back(ResourceID());

				list.back().name = r->second.front().name;
				list.back().type = r->second.front().type;
			}
		}

	}
}

void ResourceManager::getAvailableResources(ResourceType type,
		std::list<ResourceID> &list) const {

	getAvailableResources(_resourceTypeTypes[type], list);
}

void ResourceManager::normalizeType(Resource &resource) {
	// Normalize resource type *sigh*
	if      (resource.type == kFileTypeQST2)
		resource.type = kFileTypeQST;
	else if (resource.type == kFileTypeMDX2)
		resource.type = kFileTypeMDX;
	else if (resource.type == kFileTypeTXB2)
		resource.type = kFileTypeTXB;
	else if (resource.type == kFileTypeCRE)
		resource.type = kFileTypeBTC;

	// Resolve the type aliases
	std::map<FileType, FileType>::const_iterator alias = _typeAliases.find(resource.type);
	if (alias != _typeAliases.end())
		resource.type = alias->second;
}

inline uint64 ResourceManager::getHash(const Common::UString &name, FileType type) const {
	return getHash(TypeMan.setFileType(name, type));
}

inline uint64 ResourceManager::getHash(Common::UString name) const {
	name.tolower();

	return Common::hashString(name, _hashAlgo);
}

void ResourceManager::checkHashCollision(const Resource &resource, ResourceMap::const_iterator resList) {
	if (resource.name.empty() || resList->second.empty())
		return;

	Common::UString newName = TypeMan.setFileType(resource.name, resource.type);
	newName.tolower();

	for (ResourceList::const_iterator r = resList->second.begin(); r != resList->second.end(); ++r) {
		if (r->name.empty())
			continue;

		Common::UString oldName = TypeMan.setFileType(r->name, r->type);
		oldName.tolower();

		if (oldName != newName) {
			warning("ResourceManager: Found hash collision: 0x%016llX (\"%s\" and \"%s\")",
					(unsigned long long) getHash(oldName), oldName.c_str(), newName.c_str());
			return;
		}
	}
}

void ResourceManager::addResource(Resource &resource, uint64 hash, ChangeID &change) {
	normalizeType(resource);

	ResourceMap::iterator resList = _resources.find(hash);
	if (resList == _resources.end()) {
		// We don't have a resource with this name yet, create a new resource list for it

		std::pair<ResourceMap::iterator, bool> result;

		result = _resources.insert(std::make_pair(hash, ResourceList()));

		resList = result.first;
	}

#ifdef CHECK_HASH_COLLISION
	checkHashCollision(resource, resList);
#endif

	// Add the resource to the list and sort by priority
	resList->second.push_back(resource);
	resList->second.sort();

	// Remember the resource in the change set
	change._change->resources.push_back(ResourceChange());
	change._change->resources.back().hashIt = resList;
	change._change->resources.back().resIt  = --resList->second.end();
}

void ResourceManager::addResource(Resource &resource, const Common::UString &name, ChangeID &change) {
	if (name.empty())
		return;

	addResource(resource, getHash(name), change);
}

void ResourceManager::addResources(const Common::FileList &files, ChangeID &change, uint32 priority) {
	for (Common::FileList::const_iterator file = files.begin(); file != files.end(); ++file) {
		Resource res;
		res.priority = priority;
		res.source   = kSourceFile;
		res.path     = *file;
		res.name     = Common::FilePath::getStem(*file);
		res.type     = TypeMan.getFileType(*file);

		addResource(res, Common::FilePath::getFile(*file), change);
	}
}

const ResourceManager::Resource *ResourceManager::getRes(uint64 hash) const {
	ResourceMap::const_iterator r = _resources.find(hash);
	if ((r == _resources.end()) || r->second.empty() || (r->second.back().priority == 0))
		return 0;

	return &r->second.back();
}

const ResourceManager::Resource *ResourceManager::getRes(const Common::UString &name,
		const std::vector<FileType> &types) const {

	for (std::vector<FileType>::const_iterator type = types.begin(); type != types.end(); ++type) {
		const Resource *res = getRes(getHash(name, *type));
		if (res)
			return res;
	}

	return 0;
}

const ResourceManager::Resource *ResourceManager::getRes(const Common::UString &name, FileType type) const {
	std::vector<FileType> types(1, type);

	return getRes(name, types);
}

void ResourceManager::dumpResourcesList(const Common::UString &fileName) const {
	Common::DumpFile file;

	if (!file.open(fileName))
		throw Common::Exception(Common::kOpenError);

	file.writeString("                Name                 |        Hash        |     Size    \n");
	file.writeString("-------------------------------------|--------------------|-------------\n");

	for (ResourceMap::const_iterator r = _resources.begin(); r != _resources.end(); ++r) {
		if (r->second.empty())
			continue;

		const Resource &res = r->second.back();

		const Common::UString &name = res.name;
		const Common::UString   ext = TypeMan.setFileType("", res.type);
		const uint64           hash = r->first;
		const uint32           size = getResourceSize(res);

		const Common::UString line =
			Common::UString::sprintf("%32s%4s | 0x%016llX | %12d\n", name.c_str(), ext.c_str(),
                               (unsigned long long) hash, size);

		file.writeString(line);
	}

	file.flush();

	if (file.err())
		throw Common::Exception("Write error");

	file.close();
}

ResourceManager::ChangeID ResourceManager::newChangeSet() {
	// Generate a new change set

	_changes.push_back(ChangeSet());

	return ChangeID(--_changes.end());
}

} // End of namespace Aurora
