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
