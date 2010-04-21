/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/ndsrom.cpp
 *  Nintendo DS ROM parsing.
 */

// Based on http://dsibrew.org/wiki/NDS_Format

#include "boost/algorithm/string.hpp"

#include "common/error.h"
#include "common/stream.h"

#include "aurora/aurorafile.h"
#include "aurora/ndsrom.h"

namespace Aurora {

NDSFile::NDSFile(Common::SeekableReadStream *stream) : _stream(0) {
	if (!stream)
		throw Common::Exception("NDSFile::NDSFile(): stream is 0");

	_stream = stream;

	std::string gameName = AuroraFile::readRawString(*_stream, 12);
	if (gameName != "SONICCHRON") // Should be the only game we will accept.
		throw Common::Exception("NDSFile::NDSFile(): ROM is not Sonic");

	_stream->seek(0x40);
	uint32 fileNameTableOffset = _stream->readUint32LE();
	uint32 fileNameTableLength = _stream->readUint32LE();
	uint32 fatOffset = _stream->readUint32LE();
	//uint32 fatLength = _stream->readUint32LE();

	_stream->seek(fileNameTableOffset + 8);

	while ((uint32)_stream->pos() < fileNameTableOffset + fileNameTableLength) {
		byte stringLength = _stream->readByte();
		std::string filename = AuroraFile::readRawString(*_stream, stringLength);
		boost::to_lower(filename);
		_fileList.push_front(filename);
	}

	// Reverse so it will be easy to add the offsets/sizes
	_fileList.reverse();

	_stream->seek(fatOffset);
	for (FileList::const_iterator it = _fileList.begin(); it != _fileList.end(); it++) {
		FileRecord record;

		record.offset = _stream->readUint32LE();
		record.size = _stream->readUint32LE() - record.offset; // The value is the end offset

		_fileMap[*it] = record;
	}
}

NDSFile::~NDSFile() {
	delete _stream;
}

Common::SeekableReadStream *NDSFile::open(const std::string &filename) {
	std::string lowercaseString = boost::to_lower_copy(filename);

	FileMap::const_iterator it = _fileMap.find(filename);
	if (it == _fileMap.end())
		return 0;

	_stream->seek(it->second.offset);
	return _stream->readStream(it->second.size);
}

const NDSFile::FileList &NDSFile::getFileList() const {
	return _fileList;
}

} // End of namespace Aurora
