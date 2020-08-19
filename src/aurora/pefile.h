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
 *  A portable executable archive.
 */

#ifndef AURORA_PEFILE_H
#define AURORA_PEFILE_H

#include <vector>
#include <memory>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/archive.h"

namespace Common {
	class SeekableReadStream;
	class PEResources;
}

namespace Aurora {

/** A class encapsulating PE exe's for resource archive access. */
class PEFile : public Archive {
public:
	/** Take over this stream and read a PE file out of it. */
	PEFile(Common::SeekableReadStream *exe, const std::vector<Common::UString> &remap);
	~PEFile();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32_t index, bool tryNoCopy = false) const;

private:
	/** The actual exe. */
	std::unique_ptr<Common::PEResources> _peFile;

	/** External list of resource names and types. */
	ResourceList _resources;
	/** A map which maps a unique resource id to the corresponding pe id. */
	std::vector<uint32_t> _peIDs;

	void load(const std::vector<Common::UString> &remap);
};

} // End of namespace Aurora

#endif // AURORA_PEFILE_H
