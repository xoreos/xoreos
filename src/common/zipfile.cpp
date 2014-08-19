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

/** @file common/zipfile.cpp
 *  ZIP file decompresssion.
 */

#include <boost/algorithm/string.hpp>

#include "common/zipfile.h"
#include "common/error.h"
#include "common/util.h"
#include "common/stream.h"
#include "common/file.h"

#include <zlib.h>

namespace Common {

ZipFile::ZipFile(const UString &fileName) : _fileName(fileName) {
	load();
}

ZipFile::~ZipFile() {
}

void ZipFile::clear() {
	_files.clear();
}

void ZipFile::load() {
	Common::File zip;
	open(zip);

	uint32 endPos = findCentralDirectoryEnd(zip);
	if (endPos == 0)
		throw Exception("End of central directory record not found");

	zip.seek(endPos);

	zip.skip(4); // Header, already checked

	uint16 curDisk        = zip.readUint16LE();
	uint16 centralDirDisk = zip.readUint16LE();

	uint16 curDiskDirs = zip.readUint16LE();
	uint16 totalDirs   = zip.readUint16LE();

	if ((curDisk != 0) || (curDisk != centralDirDisk) || (curDiskDirs != totalDirs))
		throw Exception("Unsupported multi-disk ZIP file");

	zip.skip(4); // Size of central directory

	uint32 centralDirPos = zip.readUint32LE();
	if (!zip.seek(centralDirPos))
		throw Exception(kSeekError);

	uint32 tag = zip.readUint32LE();
	if (tag != 0x02014B50)
		throw Exception("Unknown ZIP record %08X", tag);

	_iFiles.reserve(totalDirs);
	while (tag == 0x02014B50) {
		 File  file;
		IFile iFile;

		zip.skip(20);

		iFile.size = zip.readUint32LE();

		uint16 nameLength    = zip.readUint16LE();
		uint16 extraLength   = zip.readUint16LE();
		uint16 commentLength = zip.readUint16LE();
		uint16 diskNum       = zip.readUint16LE();

		if (diskNum != 0)
			throw Exception("Unsupported multi-disk ZIP file");

		zip.skip(6); // File attributes

		iFile.offset = zip.readUint32LE();

		file.name.readFixedASCII(zip, nameLength);
		file.name.tolower();

		zip.skip(extraLength);
		zip.skip(commentLength);

		tag = zip.readUint32LE();
		if ((tag != 0x02014B50) && (tag != 0x06054B50))
			throw Exception("Unknown ZIP record %08X", tag);

		// Ignore empty file names
		if (!file.name.empty()) {
			// HACK: Skip any filename with a trailing slash because it's
			// a directory. The proper solution would be to interpret the
			// file attributes.

			if (*(--file.name.end()) != '/') {
				file.index = _iFiles.size();

				_files.push_back(file);
				_iFiles.push_back(iFile);
			}
		}
	}

	if (zip.err())
		throw Exception(kReadError);
}

const ZipFile::FileList &ZipFile::getFiles() const {
	return _files;
}

const ZipFile::IFile &ZipFile::getIFile(uint32 index) const {
	if (index >= _iFiles.size())
		throw Exception("File index out of range (%d/%d)", index, _iFiles.size());

	return _iFiles[index];
}

void ZipFile::getFileProperties(Common::SeekableReadStream &zip, const IFile &file,
		uint16 &compMethod, uint32 &compSize, uint32 &realSize) const {

	if (!zip.seek(file.offset))
		throw Exception(kSeekError);

	uint32 tag = zip.readUint32LE();
	if (tag != 0x04034B50)
		throw Exception("Unknown ZIP record %08X", tag);

	zip.skip(4);

	compMethod = zip.readUint16LE();

	zip.skip(8);

	compSize = zip.readUint32LE();
	realSize = zip.readUint32LE();

	uint16 nameLength  = zip.readUint16LE();
	uint16 extraLength = zip.readUint16LE();

	zip.skip(nameLength);
	zip.skip(extraLength);

	if (zip.err())
		throw Exception(kReadError);
}

uint32 ZipFile::getFileSize(uint32 index) const {
	const IFile &file = getIFile(index);

	Common::File zip;
	open(zip);

	uint16 compMethod;
	uint32 compSize;
	uint32 realSize;

	getFileProperties(zip, file, compMethod, compSize, realSize);

	return realSize;
}

SeekableReadStream *ZipFile::getFile(uint32 index) const {
	const IFile &file = getIFile(index);

	Common::File zip;
	open(zip);

	uint16 compMethod;
	uint32 compSize;
	uint32 realSize;

	getFileProperties(zip, file, compMethod, compSize, realSize);

	return decompressFile(zip, compMethod, compSize, realSize);
}

void ZipFile::open(Common::File &file) const {
	if (!file.open(_fileName))
		throw Exception(kOpenError);
}

SeekableReadStream *ZipFile::decompressFile(SeekableReadStream &zip, uint32 method,
		uint32 compSize, uint32 realSize) {

	if (method == 0) {
		// Uncompressed

		return zip.readStream(compSize);
	}

	if (method != 8)
		throw Exception("Unhandled Zip compression %d", method);

	// Allocate the decompressed data
	byte *decompressedData = new byte[realSize];

	// Read in the compressed data
	byte *compressedData = new byte[compSize];
	if (zip.read(compressedData, compSize) != compSize) {
		delete[] decompressedData;
		delete[] compressedData;

		return 0;
	}

	z_stream strm;
	strm.zalloc   = Z_NULL;
	strm.zfree    = Z_NULL;
	strm.opaque   = Z_NULL;
	strm.avail_in = compSize;
	strm.next_in  = compressedData;

	// Negative windows bits means there is no zlib header present in the data.
	int zResult = inflateInit2(&strm, -MAX_WBITS);
	if (zResult != Z_OK) {
		delete[] decompressedData;
		delete[] compressedData;
		throw Exception("Could not initialize zlib inflate");
	}

	strm.avail_out = realSize;
	strm.next_out = decompressedData;

	zResult = inflate(&strm, Z_SYNC_FLUSH);
	if (zResult != Z_OK && zResult != Z_STREAM_END) {
		delete[] decompressedData;
		delete[] compressedData;
		throw Exception("Failed to inflate: %d", zResult);
	}

	delete[] compressedData;
	return new MemoryReadStream(decompressedData, realSize, true);
}

#define BUFREADCOMMENT (0x400)
uint32 ZipFile::findCentralDirectoryEnd(SeekableReadStream &zip) {
	uint32 uSizeFile = zip.size();
	if (zip.err())
		return 0;

	uint32 uMaxBack = MIN<uint32>(0xFFFF, uSizeFile); // Maximum size of global comment

	byte *buf = new byte[BUFREADCOMMENT + 4];

	uint32 uPosFound = 0;
	uint32 uBackRead = 4;
	while ((uPosFound == 0) && (uBackRead < uMaxBack)) {
		uint32 uReadSize, uReadPos;

		uBackRead = MIN<uint32>(uMaxBack, uBackRead + BUFREADCOMMENT);

		uReadPos  = uSizeFile - uBackRead;

		uReadSize = MIN<uint32>(BUFREADCOMMENT + 4, uSizeFile - uReadPos);

		if (!zip.seek(uReadPos)) {
			uPosFound = 0;
			break;
		}

		if (zip.read(buf, uReadSize) != uReadSize) {
			uPosFound = 0;
			break;
		}

		for (int i = (uReadSize - 3); (i--) > 0;)
			if (READ_LE_UINT32(buf + i) == 0x06054B50) {
				uPosFound = uReadPos + i;
				break;
			}

	}

	delete[] buf;
	return uPosFound;
}

} // End of namespace Common
