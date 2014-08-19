/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/** @file aurora/aurorafile.cpp
 *  Utility class for handling special data structures found in BioWare's Aurora files.
 */

#include "common/stream.h"
#include "common/strutil.h"
#include "common/ustring.h"

#include "aurora/aurorafile.h"

namespace Aurora {

void AuroraFile::cleanupPath(Common::UString &path) {
	path.replaceAll('\\', '/');
}


AuroraBase::AuroraBase() {
	clear();
}

void AuroraBase::clear() {
	_id      = 0;
	_version = 0;
	_utf16le = false;
}

uint32 AuroraBase::getID() const {
	return _id;
}

uint32 AuroraBase::getVersion() const {
	return _version;
}

bool AuroraBase::isUTF16LE() const {
	return _utf16le;
}

void AuroraBase::readHeader(Common::SeekableReadStream &stream) {
	_id      = stream.readUint32BE();
	_version = stream.readUint32BE();

	if (((_id & 0x00FF00FF) == 0) && ((_version & 0x00FF00FF) == 0)) {
		// There's 0-bytes in the ID and version, this looks like little-endian UTF-16

		_utf16le = true;

		_id = convertUTF16LE(_id, _version);

		uint32 version1 = stream.readUint32BE();
		uint32 version2 = stream.readUint32BE();

		_version = convertUTF16LE(version1, version2);
	} else
		_utf16le = false;
}

uint32 AuroraBase::convertUTF16LE(uint32 x1, uint32 x2) {
	// Take 8 byte and remove every second byte

	return ((x1 & 0xFF000000)      ) | ((x1 & 0x0000FF00) << 8) |
	       ((x2 & 0xFF000000) >> 16) | ((x2 & 0x0000FF00) >> 8);
}

} // End of namespace Aurora
