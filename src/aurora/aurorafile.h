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
 *  Utility class for handling special data structures found in BioWare's Aurora files.
 */

#ifndef AURORA_AURORAFILE_H
#define AURORA_AURORAFILE_H

#include "src/common/types.h"

namespace Common {
	class SeekableReadStream;
	class UString;
}

namespace Aurora {

/** Base class for most files found in games using BioWare's Aurora engine. */
class AuroraBase {
public:
	AuroraBase();

	void clear();

	/** Return the file's ID. */
	uint32 getID() const;

	/** Return the file's version. */
	uint32 getVersion() const;

	/** Were the ID and version encoded in little-endian UTF-16 in the file? */
	bool isUTF16LE() const;


	/** Read the header out of a stream. */
	static void readHeader(Common::SeekableReadStream &stream,
	                       uint32 &id, uint32 &version, bool &utf16le);
	/** Read the ID and version out of a stream. */
	static void readHeader(Common::SeekableReadStream &stream, uint32 &id, uint32 &version);
	/** Read the ID out of a stream. */
	static uint32 readHeaderID(Common::SeekableReadStream &stream);


protected:
	uint32 _id;      ///< The file's ID.
	uint32 _version; ///< The file's version.
	bool   _utf16le; ///< The file's ID and version are in little-endian UTF-16.

	void readHeader(Common::SeekableReadStream &stream);

	static uint32 convertUTF16LE(uint32 x1, uint32 x2);
};

} // End of namespace Aurora

#endif // AURORA_AURORAFILE_H
