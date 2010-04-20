/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/zipfile.cpp
 *  ZIP file decompresssion.
 */

#include "boost/algorithm/string.hpp"

#include "common/error.h"
#include "common/stream.h"
#include "common/zipfile.h"

#include "aurora/aurorafile.h"

#include <zlib.h>

namespace Common {

ZipFile::ZipFile(SeekableReadStream *stream) {
	if (!stream)
		throw Exception("ZipFile: stream is 0");

	_stream = stream;

	uint32 tag = _stream->readUint32LE();
	if (tag != 0x04034B50)
		throw Exception("Invalid Zip file");

	while (tag != 0x02014b50 && _stream->pos() < _stream->size()) {
		if (tag == 0x08064b50) {          // Archive extra data record
			// Ignore!
			_stream->skip(_stream->readUint32LE());
		} else if (tag == 0x04034b50) {
			FileRecord fileRecord;

			_stream->readUint16LE();
			uint16 gpbFlag = _stream->readUint16LE();
			fileRecord.compMethod = _stream->readUint16LE();
			_stream->skip(8);
			fileRecord.compSize = _stream->readUint32LE();
			fileRecord.size = _stream->readUint32LE();
			uint16 fileNameLength = _stream->readUint16LE();
			uint16 extraFieldLength = _stream->readUint16LE();
			std::string filename = Aurora::AuroraFile::readRawString(*_stream, fileNameLength);
			_stream->skip(extraFieldLength);
			fileRecord.offset = _stream->pos();
			_stream->skip(fileRecord.compSize);

			// If we have a data descriptor, we've gotten screwed. That means
			// that the system the zip file encoder was run on didn't support
			// rb+ mode and couldn't seek while writing. This means that the
			// compressed size from above is 9. Fortunately, BioWare didn't
			// use a sucky system like that. On the other hand, take a look
			// at some of their own formats...
			if (gpbFlag & (1 << 3))
				throw Common::Exception("Unhandled: Data descriptor present in zip file");

			// HACK: Skip any filename with a trailing slash because it's
			// a directory. The proper solution would be to dump this code
			// and read from the central directory and interpret the external
			// file attributes. Since neither DrMcCoy nor I want to do this,
			// we're sticking with this hack.

			if (filename[filename.size() - 1] != '/') {
				// Convert to lowercase for simple comparisons later
				boost::to_lower(filename);

				// Add it to our file list
				_fileList.push_front(filename);

				// Add it to the map as well
				_fileMap[filename] = fileRecord;
			}
		} else
			throw Exception("Unknown Zip file chunk");

		tag = _stream->readUint32LE();
	}
}

ZipFile::~ZipFile() {
	delete _stream;
}

SeekableReadStream *ZipFile::open(const std::string &filename) {
	std::string lowercaseString = boost::to_lower_copy(filename);

	FileMap::const_iterator it = _fileMap.find(filename);
	if (it == _fileMap.end())
		return 0;

	return decompressFile(it->second);
}

const ZipFile::FileList &ZipFile::getFileList() const {
	return _fileList;
}

SeekableReadStream *ZipFile::decompressFile(const FileRecord &fileRecord) {
	_stream->seek(fileRecord.offset);

	if (fileRecord.compMethod == 0) {
		// Uncompressed
		return _stream->readStream(fileRecord.compSize);
	} else if (fileRecord.compMethod == 8) {
		// Allocate the decompressed data
		byte *decompressedData = new byte[fileRecord.size];

		// Read in the compressed data
		byte *compressedData = new byte[fileRecord.compSize];
		_stream->read(compressedData, fileRecord.compSize);

		z_stream strm;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = fileRecord.compSize;
		strm.next_in = compressedData;

		// Negative windows bits means there is no zlib header present in the data.
		int zResult = inflateInit2(&strm, -MAX_WBITS);
		if (zResult != Z_OK)
			throw Exception("Could not initialize zlib inflate");

		strm.avail_out = fileRecord.size;
		strm.next_out = decompressedData;

		zResult = inflate(&strm, Z_SYNC_FLUSH);
		if (zResult != Z_OK && zResult != Z_STREAM_END)
			throw Exception("Failed to inflate: %d", zResult);

		delete[] compressedData;
		return new MemoryReadStream(decompressedData, fileRecord.size, DisposeAfterUse::YES);
	}

	throw Exception("Unhandled Zip compression %d", fileRecord.compMethod);
	return 0;
}

} // End of namespace Common
