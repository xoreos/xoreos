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
 *  Handling GFF3 files with SAC header.
 */

#include "src/common/encoding.h"
#include "src/common/memreadstream.h"

#include "sacfile.h"

namespace Aurora {

SACFile::SACFile(Common::SeekableReadStream *stream) : GFF3File(load(stream)), _stream(stream) {
	if (getType() != MKTAG('S', 'A', 'V', ' '))
		throw Common::Exception("Invalid GFF ID");
}

Common::UString SACFile::getLevelFile() const {
	return _levelFile;
}

Common::SeekableReadStream *SACFile::load(Common::SeekableReadStream *stream) {
	stream->skip(4); // Unknown value, probably a version header?
	const uint32_t nameLength = stream->readUint32LE(); // Length of the level identifier.

	_levelFile = Common::readStringFixed(*stream, Common::kEncodingASCII, nameLength); // The Level identifier string.

	stream->skip(4); // Unknown value, probably a checksum?

	return new Common::SeekableSubReadStream(stream, stream->pos(), stream->size());
}

} // End of namespace Aurora
