/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/aurora/resources.h
 *  Generic Aurora engines resource utility functions.
 */

#ifndef ENGINES_AURORA_RESOURCES_H
#define ENGINES_AURORA_RESOURCES_H

#include "aurora/types.h"
#include "aurora/resman.h"

namespace Common {
	class UString;
}

namespace Engines {

void indexMandatoryArchive(Aurora::ArchiveType archive, const Common::UString &file,
		uint32 priority = 10, Aurora::ResourceManager::ChangeID *change = 0);

/** Add an archive file to the resource manager, if it exists. */
bool indexOptionalArchive(Aurora::ArchiveType archive, const Common::UString &file,
		uint32 priority = 10, Aurora::ResourceManager::ChangeID *change = 0);

/** Add a directory to the resource manager, erroring out if it does not exist. */
void indexMandatoryDirectory(const Common::UString &dir,
		const char *glob = 0, int depth = -1, uint32 priority = 10,
		Aurora::ResourceManager::ChangeID *change = 0);

/** Add a directory to the resource manager, if it exists. */
bool indexOptionalDirectory(const Common::UString &dir,
		const char *glob = 0, int depth = -1, uint32 priority = 10,
		Aurora::ResourceManager::ChangeID *change = 0);

} // End of namespace Engines

#endif // ENGINES_AURORA_RESOURCES_H
