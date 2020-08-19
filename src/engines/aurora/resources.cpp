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

#include "src/aurora/resman.h"

#include "src/events/events.h"

#include "src/engines/aurora/resources.h"

namespace Engines {

void indexMandatoryArchive(const Common::UString &file, uint32_t priority, const std::vector<byte> &password,
                           Common::ChangeID *changeID) {

	if (EventMan.quitRequested())
		return;

	try {
		ResMan.indexArchive(file, priority, password, changeID);
	} catch (Common::Exception &e) {
		e.add("Failed to index mandatory archive \"%s\"", file.c_str());
		throw;
	}
}

void indexMandatoryArchive(const Common::UString &file, uint32_t priority, const std::vector<byte> &password,
                           ChangeList &changes) {

	changes.push_back(Common::ChangeID());
	indexMandatoryArchive(file, priority, password, &changes.back());
}

void indexMandatoryArchive(const Common::UString &file, uint32_t priority, Common::ChangeID *changeID) {
	std::vector<byte> password;

	indexMandatoryArchive(file, priority, password, changeID);
}

void indexMandatoryArchive(const Common::UString &file, uint32_t priority, ChangeList &changes) {
	std::vector<byte> password;

	indexMandatoryArchive(file, priority, password, changes);
}

bool indexOptionalArchive(const Common::UString &file, uint32_t priority, const std::vector<byte> &password,
                          Common::ChangeID *changeID) {

	if (EventMan.quitRequested())
		return false;

	if (!ResMan.hasArchive(file))
		return false;

	try {
		ResMan.indexArchive(file, priority, password, changeID);
	} catch (Common::Exception &e) {
		e.add("Found optional archive \"%s\", but failed to index it", file.c_str());
		throw;
	}

	return true;
}

bool indexOptionalArchive(const Common::UString &file, uint32_t priority, const std::vector<byte> &password,
                          ChangeList &changes) {

	changes.push_back(Common::ChangeID());
	if (!indexOptionalArchive(file, priority, password, &changes.back())) {
		changes.pop_back();
		return false;
	}

	return true;
}

bool indexOptionalArchive(const Common::UString &file, uint32_t priority, Common::ChangeID *changeID) {
	std::vector<byte> password;

	return indexOptionalArchive(file, priority, password, changeID);
}

bool indexOptionalArchive(const Common::UString &file, uint32_t priority, ChangeList &changes) {
	std::vector<byte> password;

	return indexOptionalArchive(file, priority, password, changes);
}

void indexMandatoryDirectory(const Common::UString &dir, const char *glob, int depth,
                             uint32_t priority, Common::ChangeID *changeID) {

	if (EventMan.quitRequested())
		return;

	try {
		ResMan.indexResourceDir(dir, glob, depth, priority, changeID);
	} catch (Common::Exception &e) {
		e.add("Failed to index mandatory directory \"%s\"", dir.c_str());
		throw;
	}
}

void indexMandatoryDirectory(const Common::UString &dir, const char *glob, int depth,
                             uint32_t priority, ChangeList &changes) {

	changes.push_back(Common::ChangeID());
	indexMandatoryDirectory(dir, glob, depth, priority, &changes.back());
}

bool indexOptionalDirectory(const Common::UString &dir, const char *glob, int depth,
                            uint32_t priority, Common::ChangeID *changeID) {

	if (EventMan.quitRequested())
		return false;

	if (!ResMan.hasResourceDir(dir))
		return false;

	try {
		ResMan.indexResourceDir(dir, glob, depth, priority, changeID);
	} catch (Common::Exception &e) {
		e.add("Found optional directory \"%s\", but failed to index it", dir.c_str());
		throw;
	}

	return true;
}

bool indexOptionalDirectory(const Common::UString &dir, const char *glob, int depth,
                            uint32_t priority, ChangeList &changes) {

	changes.push_back(Common::ChangeID());
	if (!indexOptionalDirectory(dir, glob, depth, priority, &changes.back())) {
		changes.pop_back();
		return false;
	}

	return true;
}

void deindexResources(Common::ChangeID &changeID) {
	ResMan.undo(changeID);
}

void deindexResources(ChangeList &changes) {
	for (ChangeList::reverse_iterator c = changes.rbegin(); c != changes.rend(); ++c)
		deindexResources(*c);

	changes.clear();
}

} // End of namespace Engines
