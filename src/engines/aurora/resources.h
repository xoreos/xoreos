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

#ifndef ENGINES_AURORA_RESOURCES_H
#define ENGINES_AURORA_RESOURCES_H

#include <list>
#include <vector>

#include "src/common/changeid.h"

#include "src/aurora/types.h"

namespace Common {
	class UString;
}

namespace Engines {

typedef std::list<Common::ChangeID> ChangeList;

/** Add an archive file to the resource manager, erroring out if it does not exist. */
void indexMandatoryArchive(const Common::UString &file, uint32_t priority, Common::ChangeID *changeID = 0);
void indexMandatoryArchive(const Common::UString &file, uint32_t priority, ChangeList &changes);
void indexMandatoryArchive(const Common::UString &file, uint32_t priority, const std::vector<byte> &password,
                           Common::ChangeID *changeID = 0);
void indexMandatoryArchive(const Common::UString &file, uint32_t priority, const std::vector<byte> &password,
                           ChangeList &changes);

/** Add an archive file to the resource manager, if it exists. */
bool indexOptionalArchive(const Common::UString &file, uint32_t priority, Common::ChangeID *changeID = 0);
bool indexOptionalArchive(const Common::UString &file, uint32_t priority, ChangeList &changes);
bool indexOptionalArchive(const Common::UString &file, uint32_t priority, const std::vector<byte> &password,
                          Common::ChangeID *changeID = 0);
bool indexOptionalArchive(const Common::UString &file, uint32_t priority, const std::vector<byte> &password,
                          ChangeList &changes);

/** Add a directory to the resource manager, erroring out if it does not exist. */
void indexMandatoryDirectory(const Common::UString &dir, const char *glob, int depth,
                             uint32_t priority, Common::ChangeID *changeID = 0);
void indexMandatoryDirectory(const Common::UString &dir, const char *glob, int depth,
                             uint32_t priority, ChangeList &changes);

/** Add a directory to the resource manager, if it exists. */
bool indexOptionalDirectory(const Common::UString &dir, const char *glob, int depth,
                            uint32_t priority, Common::ChangeID *changeID = 0);
bool indexOptionalDirectory(const Common::UString &dir, const char *glob, int depth,
                            uint32_t priority, ChangeList &changes);

/** Remove previously added resources from the ResourceManager. */
void deindexResources(Common::ChangeID &changeID);
void deindexResources(ChangeList &changes);

} // End of namespace Engines

#endif // ENGINES_AURORA_RESOURCES_H
