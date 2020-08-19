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
 *  Base class for Nitro (Nintendo DS) files.
 */

/* Most of the Nintendo DS file formats store a BOM signaling their endianness.
 * We interpret that BOM and return an appropriate SeekableSubReadStreamEndian.
 *
 * We have two ways of opening a Nitro file:
 * - The first takes a reference to a SeekableReadStream
 * - The second takes a pointer to a SeekableReadStream
 * The second one *always* takes over the original SeekableReadStream.
 */

#include <memory>

#include "src/common/types.h"
#include "src/common/readstream.h"

#include "src/aurora/nitrofile.h"

namespace Aurora {

static bool isBigEndian(Common::SeekableReadStream &stream) {
	stream.skip(4); // Tag, we don't care about that one here

	const uint16_t bom = stream.readUint16BE();
	if ((bom != 0xFFFE) && (bom != 0xFEFF))
		throw Common::Exception("Invalid BOM: 0x%04X", (uint) bom);

	return bom == 0xFEFF;
}

Common::SeekableSubReadStreamEndian *NitroFile::open(Common::SeekableReadStream &stream) {
	const size_t begin = stream.pos();
	const size_t end   = stream.size();

	const bool bigEndian = isBigEndian(stream);

	return new Common::SeekableSubReadStreamEndian(&stream, begin, end, bigEndian, false);
}

Common::SeekableSubReadStreamEndian *NitroFile::open(Common::SeekableReadStream *stream) {
	std::unique_ptr<Common::SeekableReadStream> nitroStream(stream);

	const size_t begin = nitroStream->pos();
	const size_t end   = nitroStream->size();

	const bool bigEndian = isBigEndian(*nitroStream);

	return new Common::SeekableSubReadStreamEndian(nitroStream.release(), begin, end, bigEndian, true);
}

} // End of namespace Aurora
