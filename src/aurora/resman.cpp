/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include "common/stream.h"
#include "common/file.h"

#include "aurora/resman.h"

namespace Aurora {

ResourceManager::ResourceManager() {
}

ResourceManager::~ResourceManager() {
}

void ResourceManager::clear() {
	_bifs.clear();
	_resources.clear();
}

bool ResourceManager::hasResource(const std::string &name, FileType type) const {
	if (getRes(name, type))
		return true;

	return false;
}

Common::SeekableReadStream *ResourceManager::getResource(const std::string &name, FileType type) const {
	Resource *res = getRes(name, type);
	if (!res)
		return 0;

	if        (res->source == kSourceBIF) {
		// Read the data out of the bif and return a MemoryReadStream.

		if (!res->bif)
			return 0;

		Common::File file;
		if (!file.open(*res->bif))
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
		// Open the file and return that

		Common::File *file = new Common::File;

		if (!file->open(res->path)) {
			delete file;
			return 0;
		}

		return file;
	}

	return 0;
}

ResourceManager::Resource *ResourceManager::getRes(const std::string &name, FileType type) const {
	return 0;
}

} // End of namespace Aurora
