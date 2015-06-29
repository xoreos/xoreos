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
 *  Handling BioWare's BZFs (resource data files), used in the iOS version of
 *  Knights of the Old Republic.
 *
 *  Essentially, they are BIF files with LZMA-compressed data.
 */

#include <cassert>

#include <lzma.h>

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/aurora/bzffile.h"
#include "src/aurora/keyfile.h"

static const uint32 kBZFID     = MKTAG('B', 'I', 'F', 'F');
static const uint32 kVersion1  = MKTAG('V', '1', ' ', ' ');

namespace Aurora {

BZFFile::BZFFile(Common::SeekableReadStream *bzf) : _bzf(bzf) {
	assert(_bzf);

	try {
		load(*_bzf);
	} catch (...) {
		delete _bzf;
		throw;
	}
}

BZFFile::~BZFFile() {
	delete _bzf;
}

void BZFFile::load(Common::SeekableReadStream &bzf) {
	readHeader(bzf);

	if (_id != kBZFID)
		throw Common::Exception("Not a BZF file (%s)", Common::debugTag(_id).c_str());

	if (_version != kVersion1)
		throw Common::Exception("Unsupported BZF file version %s", Common::debugTag(_version).c_str());

	uint32 varResCount = bzf.readUint32LE();
	uint32 fixResCount = bzf.readUint32LE();

	if (fixResCount != 0)
		throw Common::Exception("TODO: Fixed BZF resources");

	_iResources.resize(varResCount);

	uint32 offVarResTable = bzf.readUint32LE();

	try {

		readVarResTable(bzf, offVarResTable);

	} catch (Common::Exception &e) {
		e.add("Failed reading BZF file");
		throw;
	}

}

void BZFFile::readVarResTable(Common::SeekableReadStream &bzf, uint32 offset) {
	bzf.seek(offset);

	for (uint32 i = 0; i < _iResources.size(); i++) {
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
		throw Common::Exception("Resource index out of range (%u/%u)", index, (uint)_iResources.size());

	return _iResources[index];
}

uint32 BZFFile::getResourceSize(uint32 index) const {
	return getIResource(index).size;
}

Common::SeekableReadStream *BZFFile::getResource(uint32 index, bool UNUSED(tryNoCopy)) const {
	const IResource &res = getIResource(index);

	_bzf->seek(res.offset);

	Common::MemoryReadStream   *packedStream = _bzf->readStream(res.packedSize);
	Common::SeekableReadStream *resStream    = 0;

	try {
		resStream = decompress(*packedStream, res.size);
	} catch (...) {
		delete packedStream;
		throw;
	}

	delete packedStream;
	return resStream;
}

Common::SeekableReadStream *BZFFile::decompress(Common::MemoryReadStream &packedStream,
                                                uint32 unpackedSize) const {
	lzma_filter filters[2];
	filters[0].id      = LZMA_FILTER_LZMA1;
	filters[0].options = 0;
	filters[1].id      = LZMA_VLI_UNKNOWN;
	filters[1].options = 0;

	const byte *compressedData = packedStream.getData();
	uint32 packedSize = packedStream.size();

	if (!lzma_filter_decoder_is_supported(filters[0].id))
		throw Common::Exception("LZMA1 compression not supported");

	uint32 propsSize;
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

	/* Ignore LZMA_DATA_ERROR and LZMA_BUF_ERROR thrown from the uncompressor.
	 * LZMA data in BZF may or may not contain an end marker.
	 * - If there is no end marker, LZMA_BUF_ERROR is thrown
	 * - If there is an end marker, LZMA_DATA_ERROR is thrown because we already
	 *   know the size of the uncompressed data
	 */
	if ((decodeRet != LZMA_OK) && (decodeRet != LZMA_DATA_ERROR) && (decodeRet != LZMA_BUF_ERROR)) {
		delete[] uncompressedData;
		throw Common::Exception("Failed to uncompress LZMA data: %d", (int) decodeRet);
	}

	return new Common::MemoryReadStream(uncompressedData, unpackedSize, true);
}

} // End of namespace Aurora
