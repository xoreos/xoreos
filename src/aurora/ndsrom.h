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
 *  Nintendo DS ROM parsing.
 */

#ifndef AURORA_NDSROM_H
#define AURORA_NDSROM_H

#include <vector>
#include <memory>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/archive.h"

namespace Common {
	class UString;
	class SeekableReadStream;
}

namespace Aurora {

/** A class encapsulating Nintendo DS ROM access. */
class NDSFile : public Archive {
public:
	/** Over this file in the filesystem and read a NDS file out of it. */
	NDSFile(const Common::UString &fileName);
	/** Take over this stream and read a NDS file out of it. */
	NDSFile(Common::SeekableReadStream *nds);
	~NDSFile();

	/** Does the Nintendo DS ROM contain a certain resource? */
	bool hasResource(Common::UString name) const;

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32_t getResourceSize(uint32_t index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32_t index, bool tryNoCopy = false) const;

	/** Return the game title string stored in the NDS header. */
	const Common::UString &getTitle() const;
	/** Return the game code string stored in the NDS header. */
	const Common::UString &getCode() const;
	/** Return the maker code string stored in the NDS header. */
	const Common::UString &getMaker() const;

	/** Check if a stream is a valid Nintendo DS ROM and read its title, code and maker strings. */
	static bool isNDS(Common::SeekableReadStream &stream,
	                  Common::UString &title, Common::UString &code, Common::UString &maker);

private:
	/** Internal resource information. */
	struct IResource {
		uint32_t offset; ///< The offset of the resource within the NDS.
		uint32_t size;   ///< The resource's size.
	};

	typedef std::vector<IResource> IResourceList;

	std::unique_ptr<Common::SeekableReadStream> _nds;

	Common::UString _title;
	Common::UString _code;
	Common::UString _maker;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	void load(Common::SeekableReadStream &nds);
	void readNames(Common::SeekableReadStream &nds, uint32_t offset, uint32_t length);
	void readFAT(Common::SeekableReadStream &nds, uint32_t offset);

	const IResource &getIResource(uint32_t index) const;
};

} // End of namespace Aurora

#endif // AURORA_NDSROM_H
