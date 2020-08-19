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
 *  Handling Aspyr's OBB virtual filesystems.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/memreadstream.h"
#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/deflate.h"

#include "src/aurora/obbfile.h"
#include "src/aurora/util.h"

namespace Aurora {

OBBFile::OBBFile(Common::SeekableReadStream *obb) : _obb(obb) {
	assert(_obb);

	load(*_obb);
}

OBBFile::~OBBFile() {
}

void OBBFile::load(Common::SeekableReadStream &obb) {
	/* OBB files have no actual header. But they're made up of zlib compressed chunks,
	 * so we just check if we find a zlib header at the start of the file. */
	if (obb.readUint16BE() != 0x789C)
		throw Common::Exception("No zlib header, this doesn't look like an Aspyr OBB virtual filesystem");

	try {
		/* Extract the resource index and read the resource list out of it. */

		std::unique_ptr<Common::SeekableReadStream> obbIndex(getIndex(obb));
		readResList(*obbIndex);

	} catch (Common::Exception &e) {
		e.add("Failed reading OBB file");
		throw;
	}
}

void OBBFile::readResList(Common::SeekableReadStream &index) {
	const uint32_t resCount = index.readUint32LE();
	index.skip(4); // Always 0. Possibly space for uint64_t?

	_iResources.reserve(resCount);

	uint32_t resIndex = 0;
	for (uint32_t i = 0; i < resCount; i++) {
		const uint32_t nameLength = index.readUint32LE();
		index.skip(4); // Always 0. Possibly space for uint64_t?

		const Common::UString name = Common::readStringFixed(index, Common::kEncodingASCII, nameLength);

		IResource iRes;

		iRes.offset = index.readUint32LE();
		index.skip(4); // Always 0. Possibly space for uint64_t?

		iRes.uncompressedSize = index.readUint32LE();
		index.skip(4); // Always 0. Possibly space for uint64_t?

		// Unreliable. See note about the compressed size in getResource()
		iRes.compressedSize = index.readUint32LE();
		index.skip(4); // Always 0. Possibly space for uint64_t?

		// Entries with a size of 0 are directories. We don't care about directories
		if (iRes.uncompressedSize == 0)
			continue;

		Resource res;

		res.name  = TypeMan.setFileType(name, kFileTypeNone);
		res.type  = TypeMan.getFileType(name);
		res.index = resIndex++;

		_resources.push_back(res);
		_iResources.push_back(iRes);
	}
}

Common::SeekableReadStream *OBBFile::getIndex(Common::SeekableReadStream &obb) {
	/* Find and decompress the resource index.
	 *
	 * It's the last compressed chunk in the OBB file, so we're searching
	 * backwards for 0x78 0x9C (the usual zlib header). That's a bit short,
	 * and can lead to false positives.
	 *
	 * But we also know that each file, after the last chunk, has another
	 * 16 bytes with some sort of meta data, of which the last four bytes
	 * are always 0x00. Immediately afterwards, the next compressed chunk
	 * start. So we can add that to our "marker" to look for.
	 *
	 * That gives us the start of the compressed resource list. To make
	 * sure we're not decompression garbage, we're also looking for the
	 * end of the list. The resource index is always one chunk, and at
	 * the end, there's the usual 16 bytes. For the resource index, the
	 * first four of those is the offset of that start of the chunk, and
	 * the next four bytes are 0x00. We can use that to figure out end.
	 *
	 * With that full range, we can decompress the resource index and
	 * return the decompressed data.
	 *
	 * NOTE: Yes, we're taking quite some shortcuts here. The original
	 * code probably does it differently and more robust. */

	static const byte kZlibHeader[6] = { 0x00, 0x00, 0x00, 0x00, 0x78, 0x9C };
	static const size_t kMaxReadBack = 0xFFFFFF; // Should be enough

	const size_t lastZlib = Common::searchBackwards(obb, kZlibHeader, sizeof(kZlibHeader), kMaxReadBack);
	if (lastZlib == SIZE_MAX)
		throw Common::Exception("Couldn't find the last zlib header");

	byte offsetData[8];
	WRITE_LE_UINT32(offsetData + 0, lastZlib + 4);
	WRITE_LE_UINT32(offsetData + 4, 0);

	Common::SeekableSubReadStream obbZIndexStart(&obb, lastZlib + 4, obb.size());

	const size_t indexSize = Common::searchBackwards(obbZIndexStart, offsetData, sizeof(offsetData), 0xFFFFFF);
	if (indexSize == SIZE_MAX)
		throw Common::Exception("Couldn't find the index end marker");

	obbZIndexStart.seek(0);

	return Common::decompressDeflateWithoutOutputSize(obbZIndexStart, indexSize, Common::kWindowBitsMax);
}

const Archive::ResourceList &OBBFile::getResources() const {
	return _resources;
}

const OBBFile::IResource &OBBFile::getIResource(uint32_t index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%u/%u)", index, (uint)_iResources.size());

	return _iResources[index];
}

uint32_t OBBFile::getResourceSize(uint32_t index) const {
	return getIResource(index).uncompressedSize;
}

Common::SeekableReadStream *OBBFile::getResource(uint32_t index, bool UNUSED(tryNoCopy)) const {
	/* Decompress a single file.
	 *
	 * Files in OBB virtual filesystems are split up in zlib compressed chunks.
	 * Each chunk, after decompression, takes up 4096 bytes (except for the last
	 * one, which can be shorter). The compressed size of the chunk is variable.
	 *
	 * Since we know the starting offset of the first chunk of the file, and
	 * the uncompressed data size, we simple decompress one chunk after the
	 * other, starting with the first of the file. Once we have decompressed
	 * as many bytes as the uncompressed size, we know we're done.
	 *
	 * The OBB virtual filesystem also has a chunk list and extra 16 bytes of
	 * meta data at the end of the last compressed chunk, but we don't really
	 * care about any of that (also, we don't really know how either of those
	 * work).
	 *
	 * We also can't use the compressed size, because that includes the extra
	 * 16 bytes. And for the first file in the OBB, it even includes the
	 * compressed size of the chunk list (which is always the second file
	 * in the OBB).
	 *
	 * NOTE: Again, lots of shortcuts. Unlike what the original does. */

	const IResource &res = getIResource(index);

	_obb->seek(res.offset);

	std::unique_ptr<byte[]> data = std::make_unique<byte[]>(res.uncompressedSize);

	size_t offset = 0;
	size_t bytesLeft = res.uncompressedSize;

	while (bytesLeft > 0) {
		const size_t bytesChunk =
			Common::decompressDeflateChunk(*_obb, Common::kWindowBitsMax,
			                               data.get() + offset, bytesLeft, 4096);

		offset    += bytesChunk;
		bytesLeft -= bytesChunk;
	}

	return new Common::MemoryReadStream(data.release(), res.uncompressedSize, true);
}

} // End of namespace Aurora
