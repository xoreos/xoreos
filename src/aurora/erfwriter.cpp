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
 *  Writing BioWare's ERFs (encapsulated resource file).
 */

#include <ctime>

#include "src/aurora/erfwriter.h"

namespace Aurora {

static const uint32 kVersion10 = MKTAG('V', '1', '.', '0');

ERFWriter::ERFWriter(uint32 id, uint32 fileCount, Common::SeekableWriteStream &stream, Version version,
                     LocString description) : _stream(stream), _currentFileCount(0), _fileCount(fileCount) {
	if (version != kERFVersion10)
		throw Common::Exception("Unsupported ERF version");

	stream.writeUint32BE(id);
	stream.writeUint32BE(kVersion10);

	// Write Header
	stream.writeUint32LE(description.getNumStrings()); // Language count
	stream.writeUint32LE(description.getWrittenSize()); // Localized string size

	stream.writeUint32LE(_fileCount); // Entry Count

	// The size of the ERF header, which is immediately followed by the LocString table
	static const uint32 kLocStringTableOffset = 160;

	_keyTableOffset = kLocStringTableOffset + description.getWrittenSize();
	_resourceTableOffset = _keyTableOffset + _fileCount * 24;

	stream.writeUint32LE(kLocStringTableOffset); // LocString offset
	stream.writeUint32LE(_keyTableOffset); // Key List offset
	stream.writeUint32LE(_resourceTableOffset); // Resource offset

	// Write the creation time of the file
	std::time_t now = std::time(0);
	std::tm *timepoint = std::localtime(&now);
	stream.writeUint32LE(timepoint->tm_year);
	stream.writeUint32LE(timepoint->tm_yday);

	// Write the description string reference
	if (description.getNumStrings())
		stream.writeUint32LE(description.getID());
	else
		stream.writeUint32LE(0);

	// Write 116 bytes of reserved header data
	stream.writeZeros(116);

	// Write the Localized string table
	description.writeLocString(stream);

	// Write the empty key list
	stream.writeZeros(_fileCount * 24);

	// The offset to the resource table plus the size of the source table
	_offsetToResourceData = _resourceTableOffset + 8 * _fileCount;

	// Write the empty resource list
	stream.writeZeros(8 * _fileCount);
}

ERFWriter::~ERFWriter() {
}

void ERFWriter::add(const Common::UString &resRef, FileType resType, Common::ReadStream &stream) {
	if (_currentFileCount == _fileCount)
		throw Common::Exception("More files added than expected");

	// Write the key table entry
	_stream.seek(_keyTableOffset + _currentFileCount * 24);

	// TODO: Handle file type aliases and "virtual" file types

	_stream.write(resRef.c_str(), MIN<size_t>(resRef.size(), 16));
	_stream.writeZeros(16 - MIN<size_t>(resRef.size(), 16));
	_stream.writeUint32LE(_currentFileCount);
	_stream.writeUint16LE(resType);
	_stream.writeUint16LE(0); // Unused

	// Write the actual resource data
	_stream.seek(_offsetToResourceData);
	const size_t size = _stream.writeStream(stream);

	// Write the resource table entry
	_stream.seek(_resourceTableOffset + _currentFileCount * 8);

	_stream.writeUint32LE(_offsetToResourceData);
	_stream.writeUint32LE(size);

	// Advance data offset and file count
	_offsetToResourceData += size;
	_currentFileCount += 1;
}

} // End of namespace Aurora
