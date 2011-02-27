/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/aurora/resources.cpp
 *  Generic Aurora engines resource utility functions.
 */

#include "common/error.h"
#include "common/ustring.h"

#include "events/events.h"

#include "engines/aurora/resources.h"

namespace Engines {

void indexMandatoryArchive(Aurora::ArchiveType archive, const Common::UString &file,
		uint32 priority, Aurora::ResourceManager::ChangeID *change) {

	if (EventMan.quitRequested())
		return;

	Aurora::ResourceManager::ChangeID c;
	c = ResMan.addArchive(archive, file, priority);

	if (change)
		*change = c;
}

bool indexOptionalArchive(Aurora::ArchiveType archive, const Common::UString &file,
		uint32 priority, Aurora::ResourceManager::ChangeID *change) {

	if (EventMan.quitRequested())
		return false;

	try {
		Aurora::ResourceManager::ChangeID c;
		c = ResMan.addArchive(archive, file, priority);

		if (change)
			*change = c;
	} catch (Common::Exception &e) {
		return false;
	}

	return true;
}

void indexMandatoryDirectory(const Common::UString &dir,
		const char *glob, int depth, uint32 priority,
		Aurora::ResourceManager::ChangeID *change) {

	if (EventMan.quitRequested())
		return;

	Aurora::ResourceManager::ChangeID c;
	c = ResMan.addResourceDir(dir, glob, depth, priority);

	if (change)
		*change = c;
}

bool indexOptionalDirectory(const Common::UString &dir,
		const char *glob, int depth, uint32 priority,
		Aurora::ResourceManager::ChangeID *change) {

	if (EventMan.quitRequested())
		return false;

	try {
		Aurora::ResourceManager::ChangeID c;
		c = ResMan.addResourceDir(dir, glob, depth, priority);

		if (change)
			*change = c;
	} catch (Common::Exception &e) {
		return false;
	}

	return true;
}


} // End of namespace Engines
