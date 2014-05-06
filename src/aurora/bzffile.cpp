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
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/bzffile.cpp
 *  Handling BioWare's BZFs (resource data files), used in the iOS version of
 *  Knights of the Old Republic.
 *
 *  Essentially, they are BIF files with LZMA-compressed data.
 */

#include <lzma.h>

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"
#include "common/file.h"

#include "aurora/bzffile.h"
#include "aurora/keyfile.h"
#include "aurora/error.h"

static const uint32 kBZFID     = MKTAG('B', 'I', 'F', 'F');
static const uint32 kVersion1  = MKTAG('V', '1', ' ', ' ');

namespace Aurora {

BZFFile::BZFFile(const Common::UString &fileName) : _fileName(fileName) {
	load();
}

BZFFile::~BZFFile() {
}

void BZFFile::clear() {
	_resources.clear();
}

void BZFFile::load() {
	Common::File bzf;
	open(bzf);

	readHeader(bzf);

	if (_id != kBZFID)
		throw Common::Exception("Not a BZF file");

	if (_version != kVersion1)
		throw Common::Exception("Unsupported BZF file version %08X", _version);

	uint32 varResCount = bzf.readUint32LE();
	uint32 fixResCount = bzf.readUint32LE();

	if (fixResCount != 0)
		throw Common::Exception("TODO: Fixed BZF resources");

	_iResources.resize(varResCount);

	uint32 offVarResTable = bzf.readUint32LE();

	try {

		readVarResTable(bzf, offVarResTable);

		if (bzf.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading BZF file");
		throw;
	}

}

void BZFFile::readVarResTable(Common::SeekableReadStream &bzf, uint32 offset) {
	if (!bzf.seek(offset))
		throw Common::Exception(Common::kSeekError);

	for (uint32_t i = 0; i < _iResources.size(); i++) {
		bzf.skip(4); // ID

		_iResources[i].offset = bzf.readUint32LE();
		_iResources[i].size   = bzf.readUint32LE();
		_iResources[i].type   = (FileType) bzf.readUint32LE();

		if (i > 0)
			_iResources[i - 1].packedSize = _iResources[i].offset - _iResources[i - 1].offset;
	}

	if (!_iResources.empty())
		_iResources.back().packedSize = bzf.size() - _iResources.back().offset;
}

void BZFFile::mergeKEY(const KEYFile &key, uint32 bifIndex) {
	const KEYFile::ResourceList &keyResList = key.getResources();

	for (KEYFile::ResourceList::const_iterator keyRes = keyResList.begin(); keyRes != keyResList.end(); ++keyRes) {
		if (keyRes->bifIndex != bifIndex)
			continue;

		if (keyRes->resIndex >= _iResources.size()) {
			warning("Resource index out of range (%d/%d)", keyRes->resIndex, (int) _iResources.size());
			continue;
		}

		if (keyRes->type != _iResources[keyRes->resIndex].type)
			warning("KEY and BZF disagree on the type of the resource \"%s\" (%d, %d). Trusting the BZF",
			        keyRes->name.c_str(), keyRes->type, _iResources[keyRes->resIndex].type);

		Resource res;

		res.name  = keyRes->name;
		res.type  = _iResources[keyRes->resIndex].type;
		res.index = keyRes->resIndex;

		_resources.push_back(res);
	}

}

const Archive::ResourceList &BZFFile::getResources() const {
	return _resources;
}

const BZFFile::IResource &BZFFile::getIResource(uint32 index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%d/%d)", index, _iResources.size());

	return _iResources[index];
}

uint32 BZFFile::getResourceSize(uint32 index) const {
	return getIResource(index).size;
}

Common::SeekableReadStream *BZFFile::getResource(uint32 index) const {
	const IResource &res = getIResource(index);
	if ((res.packedSize == 0) || (res.size == 0))
		return new Common::MemoryReadStream(0, 0);

	Common::File bzf;
	open(bzf);

	if (!bzf.seek(res.offset))
		throw Common::Exception(Common::kSeekError);

	byte *compressedData = new byte[res.packedSize];

	Common::SeekableReadStream *resStream = 0;
	try {
		if (bzf.read(compressedData, res.packedSize) != res.packedSize)
			throw Common::Exception(Common::kReadError);

		resStream = decompress(compressedData, res.packedSize, res.size);

	} catch (...) {
		delete[] compressedData;
		throw;
	}

	delete[] compressedData;
	return resStream;
}

void BZFFile::open(Common::File &file) const {
	if (!file.open(_fileName))
		throw Common::Exception(Common::kOpenError);
}

Common::SeekableReadStream *BZFFile::decompress(byte *compressedData, uint32 packedSize, uint32 unpackedSize) const {

	lzma_filter filters[2];
	filters[0].id      = LZMA_FILTER_LZMA1;
	filters[0].options = 0;
	filters[1].id      = LZMA_VLI_UNKNOWN;
	filters[1].options = 0;

	if (!lzma_filter_decoder_is_supported(filters[0].id))
		throw Common::Exception("LZMA1 compression not supported");

	uint32_t propsSize;
	if (lzma_properties_size(&propsSize, &filters[0]) != LZMA_OK)
		throw Common::Exception("Can't get LZMA1 properties size");

	if (lzma_properties_decode(&filters[0], 0, compressedData, propsSize) != LZMA_OK)
		throw Common::Exception("Failed to decode LZMA properties");

	compressedData += propsSize;
	packedSize     -= propsSize;

	byte *uncompressedData = new byte[unpackedSize];
	size_t posIn = 0, posOut = 0;

	lzma_ret decodeRet = lzma_raw_buffer_decode(filters, 0,
			compressedData  , &posIn , packedSize,
			uncompressedData, &posOut, unpackedSize);

	if (decodeRet != LZMA_OK) {
		delete[] uncompressedData;
		throw Common::Exception("Failed to uncompress LZMA data: %d", (int) decodeRet);
	}

	return new Common::MemoryReadStream(uncompressedData, unpackedSize, true);
}

} // End of namespace Aurora
