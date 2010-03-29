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

	return 0;
}

ResourceManager::Resource *ResourceManager::getRes(const std::string &name, FileType type) const {
	return 0;
}

} // End of namespace Aurora
