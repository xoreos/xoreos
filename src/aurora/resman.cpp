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

#include <cassert>

#include <boost/scope_exit.hpp>

#include "src/common/util.h"
#include "src/common/scopedptr.h"
#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/filepath.h"
#include "src/common/readfile.h"
#include "src/common/writefile.h"

#include "src/aurora/resman.h"
#include "src/aurora/util.h"

#include "src/aurora/keyfile.h"
#include "src/aurora/biffile.h"
#include "src/aurora/erffile.h"
#include "src/aurora/rimfile.h"
#include "src/aurora/ndsrom.h"
#include "src/aurora/zipfile.h"
#include "src/aurora/pefile.h"
#include "src/aurora/herffile.h"
#include "src/aurora/nsbtxfile.h"
#include "src/aurora/smallfile.h"

// Check for hash collisions (if possible)
#define CHECK_HASH_COLLISION 1

DECLARE_SINGLETON(Aurora::ResourceManager)

namespace Aurora {

ResourceManager::KnownArchive::KnownArchive() :
	type(kArchiveMAX), resource(0), opened(0) {

}

ResourceManager::KnownArchive::KnownArchive(ArchiveType t, const Common::UString &n, Resource &r) :
	name(n), type(t), resource(&r), opened(0) {

}


ResourceManager::OpenedArchive::OpenedArchive() : archive(0), known(0), parent(0) {
}

void ResourceManager::OpenedArchive::set(KnownArchive &kA, Archive &a) {
	archive = &a;
	known   = &kA;

	if (known->opened)
		throw Common::Exception("Archive \"%s\" already opened", known->name.c_str());

	known->opened = this;

	/* If the resource itself is found in an archive, double-link these two
	 * archives, so that we can catch attempts of closing archives with
	 * child-archives still open.
	 */

	assert(known->resource);
	if (known->resource->source == kSourceArchive) {
		assert(known->resource->archive);

		parent = known->resource->archive;
		parent->children.push_back(this);
	}
}


ResourceManager::Resource::Resource() : type(kFileTypeNone), isSmall(false), priority(0),
		source(kSourceNone), archive(0), archiveIndex(0xFFFFFFFF) {

	selfArchive.first = 0;
}

bool ResourceManager::Resource::operator<(const Resource &right) const {
	return priority < right.priority;
}


ResourceManager::ResourceManager() : _hasSmall(false),
	_hashAlgo(Common::kHashFNV64) {

	// These file types are archives

	_archiveTypeTypes[kArchiveKEY].insert(kFileTypeKEY);

	_archiveTypeTypes[kArchiveBIF].insert(kFileTypeBIF);

	_archiveTypeTypes[kArchiveERF].insert(kFileTypeERF);
	_archiveTypeTypes[kArchiveERF].insert(kFileTypeMOD);
	_archiveTypeTypes[kArchiveERF].insert(kFileTypeHAK);
	_archiveTypeTypes[kArchiveERF].insert(kFileTypeNWM);
	_archiveTypeTypes[kArchiveERF].insert(kFileTypeCRF);

	_archiveTypeTypes[kArchiveRIM].insert(kFileTypeRIM);
	_archiveTypeTypes[kArchiveRIM].insert(kFileTypeRIMP);

	_archiveTypeTypes[kArchiveZIP].insert(kFileTypeZIP);

	_archiveTypeTypes[kArchiveEXE].insert(kFileTypeEXE);

	_archiveTypeTypes[kArchiveNDS].insert(kFileTypeNDS);

	_archiveTypeTypes[kArchiveHERF].insert(kFileTypeHERF);

	_archiveTypeTypes[kArchiveNSBTX].insert(kFileTypeNSBTX);

	// These files types are specific resource types

	_resourceTypeTypes[kResourceImage].push_back(kFileTypeDDS);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeTPC);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeTXB);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeTGA);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypePNG);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeBMP);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeJPG);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeSBM);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypePLT);
	_resourceTypeTypes[kResourceImage].push_back(kFileTypeXEOSITEX);

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
	clearResources();
}

void ResourceManager::clear() {
	_typeAliases.clear();

	_hasSmall = false;
	_hashAlgo = Common::kHashFNV64;

	setRIMsAreERFs(false);
	clearResources();
}

void ResourceManager::clearResources() {
	_cursorRemap.clear();

	_baseDir.clear();
	_baseArchive.clear();

	for (size_t i = 0; i < kArchiveMAX; i++)
		_knownArchives[i].clear();

	for (OpenedArchives::iterator a = _openedArchives.begin(); a != _openedArchives.end(); ++a)
		delete a->archive;
	_openedArchives.clear();

	_resources.clear();

	_changes.clear();
}

void ResourceManager::setRIMsAreERFs(bool rimsAreERFs) {
	// Treat RIM and RIMP as either RIM or ERF

	_archiveTypeTypes[kArchiveRIM].erase(kFileTypeRIM);
	_archiveTypeTypes[kArchiveRIM].erase(kFileTypeRIMP);
	_archiveTypeTypes[kArchiveERF].erase(kFileTypeRIM);
	_archiveTypeTypes[kArchiveERF].erase(kFileTypeRIMP);

	if (rimsAreERFs) {
		_archiveTypeTypes[kArchiveERF].insert(kFileTypeRIM);
		_archiveTypeTypes[kArchiveERF].insert(kFileTypeRIMP);
	} else {
		_archiveTypeTypes[kArchiveRIM].insert(kFileTypeRIM);
		_archiveTypeTypes[kArchiveRIM].insert(kFileTypeRIMP);
	}
}

void ResourceManager::setHasSmall(bool hasSmall) {
	_hasSmall = hasSmall;
}

void ResourceManager::setHashAlgo(Common::HashAlgo algo) {
	if ((algo != _hashAlgo) && !_resources.empty())
		throw Common::Exception("ResourceManager::setHashAlgo(): We already have resources!");

	_hashAlgo = algo;
}

void ResourceManager::setCursorRemap(const std::vector<Common::UString> &remap) {
	_cursorRemap = remap;
}

void ResourceManager::registerDataBase(const Common::UString &path) {
	clearResources();

	Common::UString base = Common::FilePath::canonicalize(path);

	if        (Common::FilePath::isDirectory(base)) {

		_baseDir = base;

		indexResourceDir("", 0, 0, 1);

	} else if (Common::FilePath::isRegularFile(base)) {

		_baseArchive = base;

		indexResourceFile(_baseArchive, 1);
		indexArchive     (_baseArchive, 1);

	} else
		throw Common::Exception("No such file or directory \"%s\"", path.c_str());

}

const Common::UString &ResourceManager::getDataBase() const {
	if (!_baseArchive.empty())
		return _baseArchive;

	return _baseDir;
}

ResourceManager::KnownArchive *ResourceManager::findArchive(const Common::UString &file) {
	ArchiveType archiveType = getArchiveType(file);
	if (((size_t) archiveType) >= kArchiveMAX)
		return 0;

	return findArchive(file, _knownArchives[archiveType]);
}

ResourceManager::KnownArchive *ResourceManager::findArchive(Common::UString file, KnownArchives &archives) {
	file = (Common::FilePath::isAbsolute(file) ? "" : "/") + file;
	file = Common::FilePath::normalize(file, false).toLower();

	for (KnownArchives::iterator a = archives.begin(); a != archives.end(); ++a)
		if (a->name.toLower().endsWith(file))
			return &*a;

	return 0;
}

bool ResourceManager::hasArchive(const Common::UString &file) {
	return findArchive(file) != 0;
}

Common::SeekableReadStream *ResourceManager::openArchiveStream(const KnownArchive &archive) const {
	if (!archive.resource)
		throw Common::Exception("Archive without resource reference");

	return getResource(*archive.resource, true);
}

void ResourceManager::indexArchive(const Common::UString &file, uint32 priority,
                                   const std::vector<byte> &password, Common::ChangeID *changeID) {

	KnownArchive *knownArchive = findArchive(file);
	if (!knownArchive)
		throw Common::Exception("No such archive file \"%s\"", file.c_str());

	if (knownArchive->type == kArchiveBIF)
		throw Common::Exception("Attempted to index a lone BIF");

	Change *change = 0;
	if (changeID)
		change = newChangeSet(*changeID);

	Common::SeekableReadStream *archiveStream = openArchiveStream(*knownArchive);

	Common::ScopedPtr<Archive> archive;
	switch (knownArchive->type) {
		case kArchiveKEY:
			indexKEY(archiveStream, priority, change);
			break;

		case kArchiveNDS:
			archive.reset(new NDSFile(archiveStream));
			break;

		case kArchiveHERF:
			archive.reset(new HERFFile(archiveStream));
			break;

		case kArchiveERF:
			archive.reset(new ERFFile(archiveStream, password));
			break;

		case kArchiveRIM:
			archive.reset(new RIMFile(archiveStream));
			break;

		case kArchiveZIP:
			archive.reset(new ZIPFile(archiveStream));
			break;

		case kArchiveEXE:
			archive.reset(new PEFile(archiveStream, _cursorRemap));
			break;

		case kArchiveNSBTX:
			archive.reset(new NSBTXFile(archiveStream));
			break;

		default:
			throw Common::Exception("Invalid archive type %d", knownArchive->type);
	}

	if (archive)
		indexArchive(*knownArchive, archive.release(), priority, change);
}

void ResourceManager::indexArchive(const Common::UString &file, uint32 priority, Common::ChangeID *changeID) {
	std::vector<byte> password;

	indexArchive(file, priority, password, changeID);
}

uint32 ResourceManager::openKEYBIFs(Common::SeekableReadStream *keyStream,
                                    std::vector<KnownArchive *> &archives,
                                    std::vector<BIFFile *> &bifs) {

	bool success = false;
	BOOST_SCOPE_EXIT( (&success) (&archives) (&bifs) ) {
		if (!success) {
			for (std::vector<BIFFile *>::iterator b = bifs.begin(); b != bifs.end(); ++b)
				delete *b;

			bifs.clear();
			archives.clear();
		}
	} BOOST_SCOPE_EXIT_END

	Common::ScopedPtr<Common::SeekableReadStream> stream(keyStream);
	KEYFile key(*keyStream);

	const KEYFile::BIFList &keyBIFs = key.getBIFs();
	archives.resize(keyBIFs.size(), 0);
	bifs.resize(keyBIFs.size(), 0);

	for (uint32 i = 0; i < keyBIFs.size(); i++) {
		archives[i] = findArchive(keyBIFs[i], _knownArchives[kArchiveBIF]);
		if (!archives[i])
			throw Common::Exception("BIF \"%s\" not found", keyBIFs[i].c_str());

		bifs[i] = new BIFFile(openArchiveStream(*archives[i]));
		bifs[i]->mergeKEY(key, i);
	}

	success = true;
	return archives.size();
}

void ResourceManager::indexKEY(Common::SeekableReadStream *stream, uint32 priority, Change *change) {
	std::vector<KnownArchive *> archives;
	std::vector<BIFFile *> bifs;

	const uint32 count = openKEYBIFs(stream, archives, bifs);

	for (uint32 i = 0; i < count; i++)
		indexArchive(*archives[i], bifs[i], priority, change);
}

void ResourceManager::indexArchive(KnownArchive &knownArchive, Archive *archive,
                                   uint32 priority, Change *change) {

	const Common::HashAlgo hashAlgo = archive->getNameHashAlgo();
	if ((hashAlgo != Common::kHashNone) && (hashAlgo != _hashAlgo))
		throw Common::Exception("ResourceManager::indexArchive(): Archive uses a different name hashing "
		                        "algorithm than we do (%d vs. %d)", (int) hashAlgo, (int) _hashAlgo);

	bool couldSet = false;
	_openedArchives.push_back(OpenedArchive());

	BOOST_SCOPE_EXIT( (&couldSet) (&_openedArchives) (&archive) ) {
		if (!couldSet) {
			_openedArchives.pop_back();
			delete archive;
		}
	} BOOST_SCOPE_EXIT_END

	_openedArchives.back().set(knownArchive, *archive);
	couldSet = true;

	// Add the information of the new archive to the change set
	if (change)
		change->_change->openedArchives.push_back(--_openedArchives.end());

	const Archive::ResourceList &resources = archive->getResources();
	for (Archive::ResourceList::const_iterator resource = resources.begin(); resource != resources.end(); ++resource) {
		// Build the resource record
		Resource res;
		res.priority     = priority;
		res.source       = kSourceArchive;
		res.archive      = &_openedArchives.back();
		res.archiveIndex = resource->index;
		res.name         = resource->name;
		res.type         = resource->type;

		// Get the hash or calculate if we have to
		uint64 hash = (hashAlgo == Common::kHashNone) ? getHash(res.name, res.type) : resource->hash;

		// Normalize the file types if we can and recalculate the hash
		if ((res.name != "") && (res.type != kFileTypeNone))
			if (normalizeType(res))
				hash = getHash(res.name, res.type);

		// Handle "small" files
		if (_hasSmall && (res.type == kFileTypeSMALL)) {
			res.isSmall = true;

			res.name = Common::FilePath::getStem(resource->name);
			res.type = TypeMan.getFileType(resource->name);
		}

		// And add it to our list
		addResource(res, hash, change);
	}
}

bool ResourceManager::hasResourceDir(const Common::UString &dir) {
	if (_baseDir.empty())
		return false;

	return !Common::FilePath::findSubDirectory(_baseDir, dir, true).empty();
}

void ResourceManager::indexResourceFile(const Common::UString &file, uint32 priority,
                                        Common::ChangeID *changeID) {

	Common::UString path;
	path = _baseDir.empty() ? file : (_baseDir + "/" + file);
	path = Common::FilePath::normalize(path, false);

	if (!Common::FilePath::isRegularFile(path))
		throw Common::Exception("No such file \"%s\"", file.c_str());

	Change *change = 0;
	if (changeID)
		change = newChangeSet(*changeID);

	addResource(path, change, priority);
}

void ResourceManager::indexResourceDir(const Common::UString &dir, const char *glob, int depth,
                                       uint32 priority, Common::ChangeID *changeID) {
	if (_baseDir.empty())
		throw Common::Exception("No base data directory set");

	// Find the directory
	Common::UString directory = Common::FilePath::findSubDirectory(_baseDir, dir, true);
	if (directory.empty())
		throw Common::Exception("No such directory \"%s\"", dir.c_str());

	// Find files
	Common::FileList files;
	files.addDirectory(directory, depth);

	Change *change = 0;
	if (changeID)
		change = newChangeSet(*changeID);

	if (!glob) {
		// Add the files
		addResources(files, change, priority);
		return;
	}

	// Find files matching the glob pattern
	Common::FileList globFiles;
	files.getSubListGlob(glob, true, globFiles);

	// Add the files
	addResources(globFiles, change, priority);
}

void ResourceManager::undo(Common::ChangeID &changeID) {
	Change *change = dynamic_cast<Change *>(changeID.getContent());
	if (!change || (change->_change == _changes.end()))
		return;

	// Removing all changes in the opened archives list
	for (OpenedArchiveChanges::iterator oaChange = change->_change->openedArchives.begin();
	     oaChange != change->_change->openedArchives.end(); ++oaChange) {

		if (!(*oaChange)->children.empty())
			throw Common::Exception("Attempted to deindex an archive that has opened children archives");

		assert((*oaChange)->known);

		// Remove us from the KnownArchive
		(*oaChange)->known->opened = 0;

		// Remove us from a parent's children list
		if ((*oaChange)->parent) {
			std::list<OpenedArchive *> &pChildren = (*oaChange)->parent->children;

			bool found = false;
			for (std::list<OpenedArchive *>::iterator c = pChildren.begin(); c != pChildren.end(); ++c) {
				if (*c == &**oaChange) {
					found = true;
					pChildren.erase(c);
					break;
				}
			}

			if (!found)
				throw Common::Exception("Couldn't find archive in the parent's children list");
		}

		delete (*oaChange)->archive;
		_openedArchives.erase(*oaChange);
	}

	// Removing all changes in the known archives list
	for (KnownArchiveChanges::iterator kaChange = change->_change->knownArchives.begin();
	     kaChange != change->_change->knownArchives.end(); ++kaChange) {

		if (kaChange->second->opened)
			throw Common::Exception("Attempted to deindex an archive that's still opened");

		// Remove us from the resource
		assert(kaChange->second->resource);
		kaChange->second->resource->selfArchive.first = 0;

		kaChange->first->erase(kaChange->second);
	}

	// Go through all changes in the resource map
	for (ResourceChanges::iterator resChange = change->_change->resources.begin();
	     resChange != change->_change->resources.end(); ++resChange) {

		// If the resource still has an archive attached, it was added by a
		// declareResources() call and needs to be removed manually
		if (resChange->resIt->selfArchive.first) {
			if (resChange->resIt->selfArchive.second->opened)
				throw Common::Exception("Attempted to deindex an archive resource that's still opened");

			resChange->resIt->selfArchive.first->erase(resChange->resIt->selfArchive.second);
		}

		// Remove the resource, and the name list too if it's empty
		resChange->hashIt->second.erase(resChange->resIt);

		if (resChange->hashIt->second.empty())
			_resources.erase(resChange->hashIt);
	}

	// Now we can remove the change set from our list of change sets
	_changes.erase(change->_change);

	// And finally set the change ID to a defined empty state
	changeID.clear();
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
	bool isSmall = false;

	ResourceMap::iterator resList = _resources.find(getHash(name, type));
	if (resList == _resources.end()) {
		if (_hasSmall) {
			Common::UString smallName = TypeMan.addFileType(TypeMan.setFileType(name, type), kFileTypeSMALL);

			resList = _resources.find(getHash(smallName));
			isSmall = true;
		}

		if (resList == _resources.end())
			return;
	}

	for (ResourceList::iterator r = resList->second.begin(); r != resList->second.end(); ++r) {
		r->name    = name;
		r->type    = type;
		r->isSmall = isSmall;

		checkResourceIsArchive(*r, 0);
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

bool ResourceManager::hasResource(const Common::UString &name, ResourceType type) const {
	assert((type >= 0) && (type < kResourceMAX));

	return hasResource(name, _resourceTypeTypes[type]);
}

bool ResourceManager::hasResource(const Common::UString &name) const {
	return hasResource(TypeMan.setFileType(name, kFileTypeNone), TypeMan.getFileType(name));
}

bool ResourceManager::hasResource(const Common::UString &name, const std::vector<FileType> &types) const {
	return getRes(name, types) != 0;
}

bool ResourceManager::hasResource(uint64 hash) const {
	return getRes(hash) != 0;
}

Common::UString ResourceManager::findResourceFile(const Common::UString &name, FileType type) const {
	std::vector<FileType> types;

	types.push_back(type);

	return findResourceFile(name, types);
}

Common::UString ResourceManager::findResourceFile(const Common::UString &name, ResourceType type) const {
	assert((type >= 0) && (type < kResourceMAX));

	return findResourceFile(name, _resourceTypeTypes[type]);
}

Common::UString ResourceManager::findResourceFile(const Common::UString &name) const {
	return findResourceFile(TypeMan.setFileType(name, kFileTypeNone), TypeMan.getFileType(name));
}

Common::UString ResourceManager::findResourceFile(const Common::UString &name,
                                                  const std::vector<FileType> &types) const {
	const Resource *res = getRes(name, types);
	if (res && (res->source == kSourceFile))
		return res->path;

	return "";
}

uint32 ResourceManager::getResourceSize(const Resource &res) const {
	if (res.source == kSourceArchive) {
		if ((res.archive == 0) || (res.archive->archive == 0) || (res.archiveIndex == 0xFFFFFFFF))
			return 0xFFFFFFFF;

		return res.archive->archive->getResourceSize(res.archiveIndex);
	}

	if (res.source == kSourceFile)
		return Common::FilePath::getFileSize(res.path);

	return 0xFFFFFFFF;
}

Common::SeekableReadStream *ResourceManager::getArchiveResource(const Resource &res, bool tryNoCopy) const {
	if ((res.archive == 0) || (res.archive->archive == 0) || (res.archiveIndex == 0xFFFFFFFF))
		throw Common::Exception("Archive resource has no archive");

	return res.archive->archive->getResource(res.archiveIndex, tryNoCopy);
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

	return getResource(*res);
}

Common::SeekableReadStream *ResourceManager::getResource(uint64 hash, FileType *type) const {
	const Resource *res = getRes(hash);
	if (!res)
		return 0;

	// Return the actually found type
	if (type)
		*type = res->type;

	return getResource(*res);
}

Common::SeekableReadStream *ResourceManager::getResource(const Resource &res, bool tryNoCopy) const {
	Common::SeekableReadStream *stream = 0;

	switch (res.source) {
		case kSourceFile:
			stream = new Common::ReadFile(res.path);
			break;

		case kSourceArchive:
			stream = getArchiveResource(res, tryNoCopy);
			break;

		default:
			throw Common::Exception("Invalid source for resource \"%s\": (%d)",
			                        TypeMan.setFileType(res.name, res.type).c_str(), res.source);
	}

	// Transparently decompress "small" files
	if (res.isSmall)
		stream = Small::decompress(stream);

	return stream;
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
			list.back().hash = r->first;
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
				list.back().hash = r->first;
			}
		}

	}
}

void ResourceManager::getAvailableResources(ResourceType type,
		std::list<ResourceID> &list) const {

	getAvailableResources(_resourceTypeTypes[type], list);
}

ArchiveType ResourceManager::getArchiveType(FileType type) const {
	for (size_t i = 0; i < kArchiveMAX; i++)
		if (_archiveTypeTypes[i].find(type) != _archiveTypeTypes[i].end())
			return (ArchiveType) i;

	return kArchiveMAX;
}

ArchiveType ResourceManager::getArchiveType(const Common::UString &name) const {
	return getArchiveType(TypeMan.getFileType(name));
}

Common::UString ResourceManager::getArchiveName(const Resource &resource) const {
	switch (resource.source) {
		case kSourceFile:
			return resource.path;

		case kSourceArchive:
			return "/" + TypeMan.addFileType(resource.name, resource.type);

		default:
			break;
	}

	throw Common::Exception("Invalid source for resource \"%s\": (%d)",
	                        TypeMan.addFileType(resource.name, resource.type).c_str(),
	                        resource.source);
}

bool ResourceManager::normalizeType(Resource &resource) {
	// Resolve the type aliases
	std::map<FileType, FileType>::const_iterator alias = _typeAliases.find(resource.type);
	if (alias != _typeAliases.end()) {
		resource.type = alias->second;
		return true;
	}

	// Normalize resource type *sigh*
	if      (resource.type == kFileTypeQST2)
		resource.type = kFileTypeQST;
	else if (resource.type == kFileTypeMDX2)
		resource.type = kFileTypeMDX;
	else if (resource.type == kFileTypeTXB2)
		resource.type = kFileTypeTXB;
	else if (resource.type == kFileTypeMDB2)
		resource.type = kFileTypeMDB;
	else if (resource.type == kFileTypeMDA2)
		resource.type = kFileTypeMDA;
	else if (resource.type == kFileTypeSPT2)
		resource.type = kFileTypeSPT;
	else if (resource.type == kFileTypeJPG2)
		resource.type = kFileTypeJPG;
	else
		return false;

	return true;
}

inline uint64 ResourceManager::getHash(const Common::UString &name, FileType type) const {
	return getHash(TypeMan.setFileType(name, type));
}

inline uint64 ResourceManager::getHash(const Common::UString &name) const {
	return Common::hashString(name.toLower(), _hashAlgo);
}

void ResourceManager::checkHashCollision(const Resource &resource, ResourceMap::const_iterator resList) {
	if (resource.name.empty() || resList->second.empty())
		return;

	Common::UString newName = TypeMan.setFileType(resource.name, resource.type).toLower();

	for (ResourceList::const_iterator r = resList->second.begin(); r != resList->second.end(); ++r) {
		if (r->name.empty())
			continue;

		Common::UString oldName = TypeMan.setFileType(r->name, r->type).toLower();
		if (oldName != newName) {
			warning("ResourceManager: Found hash collision: %s (\"%s\" and \"%s\")",
					Common::formatHash(getHash(oldName)).c_str(), oldName.c_str(), newName.c_str());
			return;
		}
	}
}

bool ResourceManager::checkResourceIsArchive(Resource &resource, Change *change) {
	if ((resource.source == kSourceNone) || resource.name.empty())
		return false;

	ArchiveType type = getArchiveType(resource.type);
	if (type == kArchiveMAX)
		return false;

	Common::UString name = getArchiveName(resource);
	if (name.empty())
		return false;

	if (resource.selfArchive.first) {
		if ((resource.selfArchive.second->type != type) || (resource.selfArchive.second->name != name))
			throw Common::Exception("Tried to reclassify a resource archive (\"%s\")", name.c_str());

		return false;
	}

	KnownArchives &archives = _knownArchives[type];

	archives.push_back(KnownArchive(type, name, resource));

	resource.selfArchive = std::make_pair(&archives, --archives.end());

	if (change)
		change->_change->knownArchives.push_back(resource.selfArchive);

	return true;
}

void ResourceManager::addResource(Resource &resource, uint64 hash, Change *change) {
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

	// Add the resource to the list
	resList->second.push_back(resource);
	Resource *res = &resList->second.back();

	checkResourceIsArchive(*res, change);

	// Remember the resource in the change set
	if (change) {
		change->_change->resources.push_back(ResourceChange());
		change->_change->resources.back().hashIt = resList;
		change->_change->resources.back().resIt  = --resList->second.end();
	}

	// Resort the list by priority
	resList->second.sort();
}

void ResourceManager::addResource(const Common::UString &path, Change *change, uint32 priority) {
	Resource res;
	res.priority = priority;
	res.source   = kSourceFile;
	res.path     = path;
	res.name     = Common::FilePath::getStem(path);
	res.type     = TypeMan.getFileType(path);

	// Handle "small" files
	if (_hasSmall && (res.type == kFileTypeSMALL)) {
		const Common::UString name = res.name;

		res.isSmall = true;

		res.name = Common::FilePath::getStem(name);
		res.type = TypeMan.getFileType(name);
	}

	uint64 hash = getHash(res.name, res.type);
	if (normalizeType(res))
		hash = getHash(res.name, res.type);

	addResource(res, hash, change);
}

void ResourceManager::addResources(const Common::FileList &files, Change *change, uint32 priority) {
	for (Common::FileList::const_iterator file = files.begin(); file != files.end(); ++file)
		addResource(*file, change, priority);
}

const ResourceManager::Resource *ResourceManager::getRes(uint64 hash) const {
	ResourceMap::const_iterator r = _resources.find(hash);
	if ((r == _resources.end()) || r->second.empty() || (r->second.back().priority == 0))
		return 0;

	return &r->second.back();
}

const ResourceManager::Resource *ResourceManager::getRes(const Common::UString &name,
		const std::vector<FileType> &types) const {

	const Resource *result = 0;
	for (std::vector<FileType>::const_iterator type = types.begin(); type != types.end(); ++type) {
		const Resource *res = getRes(getHash(name, *type));
		if (res && (!result || *result < *res))
			result = res;
	}
	if (!result && _hasSmall) {
		for (std::vector<FileType>::const_iterator type = types.begin(); type != types.end(); ++type) {
			Common::UString smallName = TypeMan.addFileType(TypeMan.setFileType(name, *type), kFileTypeSMALL);

			const Resource *res = getRes(getHash(smallName));
			if (res && (!result || *result < *res))
				result = res;
		}
	}

	return result;
}

const ResourceManager::Resource *ResourceManager::getRes(const Common::UString &name, FileType type) const {
	std::vector<FileType> types(1, type);

	return getRes(name, types);
}

void ResourceManager::dumpResourcesList(const Common::UString &fileName) const {
	Common::WriteFile file;

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
			Common::UString::format("%32s%4s | %s | %12d\n", name.c_str(), ext.c_str(),
                               Common::formatHash(hash).c_str(), size);

		file.writeString(line);
	}

	file.flush();
	file.close();
}

ResourceManager::Change *ResourceManager::newChangeSet(Common::ChangeID &changeID) {
	// Does this change ID already have a change set attached? If so, use that
	Change *change = dynamic_cast<Change *>(changeID.getContent());
	if (change && (change->_change != _changes.end()))
		return change;

	// Otherwise, generate a new one

	_changes.push_back(ChangeSet());

	change = new Change(--_changes.end());
	changeID.setContent(change);

	return change;
}

} // End of namespace Aurora
