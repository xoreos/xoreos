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

#include "src/common/readstream.h"

#include "src/aurora/aurorafile.h"

namespace Aurora {

AuroraFile::AuroraFile() {
	clear();
}

void AuroraFile::clear() {
	_id      = 0;
	_version = 0;
	_utf16le = false;
}

uint32 AuroraFile::getID() const {
	return _id;
}

uint32 AuroraFile::getVersion() const {
	return _version;
}

bool AuroraFile::isUTF16LE() const {
	return _utf16le;
}

void AuroraFile::readHeader(Common::ReadStream &stream, uint32 &id, uint32 &version, bool &utf16le) {
	id      = stream.readUint32BE();
	version = stream.readUint32BE();

	if (((id & 0x00FF00FF) == 0) && ((version & 0x00FF00FF) == 0)) {
		// There's 0-bytes in the ID and version, this looks like little-endian UTF-16

		utf16le = true;

		id = convertUTF16LE(id, version);

		uint32 version1 = stream.readUint32BE();
		uint32 version2 = stream.readUint32BE();

		version = convertUTF16LE(version1, version2);
	} else
		utf16le = false;
}

void AuroraFile::readHeader(Common::ReadStream &stream, uint32 &id, uint32 &version) {
	bool utf16le;
	readHeader(stream, id, version, utf16le);
}

uint32 AuroraFile::readHeaderID(Common::ReadStream &stream) {
	uint32 id, version;
	readHeader(stream, id, version);

	return id;
}

void AuroraFile::readHeader(Common::ReadStream &stream) {
	readHeader(stream, _id, _version, _utf16le);
}

uint32 AuroraFile::convertUTF16LE(uint32 x1, uint32 x2) {
	// Take 8 byte and remove every second byte

	return ((x1 & 0xFF000000)      ) | ((x1 & 0x0000FF00) << 8) |
	       ((x2 & 0xFF000000) >> 16) | ((x2 & 0x0000FF00) >> 8);
}

} // End of namespace Aurora
