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
 *  Base for BioWare's Aurora engine files.
 */

#ifndef AURORA_AURORAFILE_H
#define AURORA_AURORAFILE_H

#include "src/common/types.h"

namespace Common {
	class ReadStream;
}

namespace Aurora {

/** Base class for most files found in games using BioWare's Aurora engine.
 *
 *  Aurora files generally start with a 4-byte human-readable ID, for example
 *  'ERF ', followed by a 4-byte version string, like 'V1.0'. This base class
 *  reads them as big-endian 32-bit integer values. See the MKTAG() macro, as
 *  defined in src/common/endianness.h, for generating values to compare the
 *  IDs read out of a stream against.
 *
 *  Later games encode these ID and version values as 8-byte, little-endian
 *  UTF-16 strings instead. We automatically detect this, convert them back
 *  into the old format, and set a flag that can be queried with the method
 *  isUTF16LE().
 *
 *  Alternatively, AuroraFile provides static methods for reading the base
 *  header out of a stream.
 */
class AuroraFile {
public:
	AuroraFile();

	void clear();

	/** Return the file's ID. */
	uint32 getID() const;

	/** Return the file's version. */
	uint32 getVersion() const;

	/** Were the ID and version encoded in little-endian UTF-16 in the file? */
	bool isUTF16LE() const;

	// .--- Static base header readers
	/** Read the header out of a stream. */
	static void readHeader(Common::ReadStream &stream, uint32 &id, uint32 &version, bool &utf16le);
	/** Read the ID and version out of a stream. */
	static void readHeader(Common::ReadStream &stream, uint32 &id, uint32 &version);
	/** Read the ID out of a stream. */
	static uint32 readHeaderID(Common::ReadStream &stream);
	// '---

protected:
	uint32 _id;      ///< The file's ID.
	uint32 _version; ///< The file's version.
	bool   _utf16le; ///< The file's ID and version are in little-endian UTF-16.

	void readHeader(Common::ReadStream &stream);

	static uint32 convertUTF16LE(uint32 x1, uint32 x2);
};

} // End of namespace Aurora

#endif // AURORA_AURORAFILE_H
