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
 *  Generic Aurora engines resource utility functions.
 */

#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/events/events.h"

#include "src/engines/aurora/resources.h"

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
