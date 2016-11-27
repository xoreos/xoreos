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
 *  Decompressing "small" files, Nintendo DS LZSS (types 0x00 and 0x10), found in Sonic.
 */

#include "src/common/scopedptr.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "src/aurora/smallfile.h"

namespace Aurora {

static void readSmallHeader(Common::ReadStream &small, uint32 &type, uint32 &size) {
	uint32 data = small.readUint32LE();

	type = data & 0x000000FF;
	size = data >> 8;
}

static void writeSmallHeader(Common::WriteStream &small, uint32 type, uint32 size) {
	type &= 0x000000FF;
	size &= 0x00FFFFFF;

	small.writeUint32LE((size << 8) | type);
}

static void decompress00(Common::ReadStream &small, Common::WriteStream &out, uint32 size) {
	out.writeStream(small, size);
}

static void compress00(Common::ReadStream &in, Common::WriteStream &small, uint32 size) {
	small.writeStream(in, size);
}

/* Simple LZSS 0x10 decompression.
 *
 * Code loosely based on DSDecmp by Barubary, released under the terms of the MIT license.
 *
 * See <https://github.com/gravgun/dsdecmp/blob/master/CSharp/DSDecmp/Formats/Nitro/LZ10.cs#L121>
 * and <https://code.google.com/p/dsdecmp/>.
 */
static void decompress10(Common::ReadStream &small, Common::WriteStream &out, uint32 size) {
	byte   buffer[0x10000];
	uint32 bufferPos = 0;

	uint16 flags = 0xFF00;

	uint32 outSize = 0;
	while (outSize < size) {
		// Only our canaries left => Read flags for the next 8 blocks
		if (flags == 0xFF00)
			flags = (small.readByte() << 8) | 0x00FF;

		if (flags & 0x8000) {
			// Copy from buffer

			const byte data1 = small.readByte();
			const byte data2 = small.readByte();

			// Copy how many bytes from where (relative) in the buffer?
			const uint8  length = (data1 >> 4) + 3;
			const uint16 offset = (((data1 & 0x0F) << 8) | data2) + 1;

			// Direct offset. Add size of the buffer once, to protect from overroll
			uint32 copyOffset = bufferPos + sizeof(buffer) - offset;

			// Copy length bytes (and store each back into the buffer)
			for (uint8 i = 0; i < length; i++, copyOffset++) {
				if ((copyOffset % sizeof(buffer)) >= outSize)
					throw Common::Exception("Tried to copy past the buffer");

				const byte data = buffer[copyOffset % sizeof(buffer)];

				out.writeByte(data);
				outSize++;

				buffer[bufferPos] = data;
				bufferPos = (bufferPos + 1) % sizeof(buffer);
			}

		} else {
			// Read literal byte

			const byte data = small.readByte();

			out.writeByte(data);
			outSize++;

			buffer[bufferPos] = data;
			bufferPos = (bufferPos + 1) % sizeof(buffer);
		}

		flags <<= 1;
	}

	if (outSize != size)
		throw Common::Exception("Invalid \"small\" data");
}

/** Determine the maximum size of an LZSS-compressed block.
 *
 *  Since this function supports continuously copying bytes from the "edge"
 *  of the current compression pointer (which is the main difference between
 *  LZSS and vanilla LZ77), both newPtr and oldPtr need to point into the
 *  same memory region.
 *
 *  Takes O(newLength * oldLength) = O(n^2) time.
 *
 *  @param  newPtr The start of the data that needs to be compressed.
 *  @param  newLength The number of bytes that still need to be compressed.
 *  @param  oldPtr The start of the raw file.
 *  @param  oldLength The number of bytes already compressed.
 *  @param  displacement The offset of the start of the longest block to refer to.
 *  @param  minDisplacement The minimum allowed value for a valid displacement.
 *  @return The length of the longest sequence of bytes that can be copied from oldPtr.
 */
size_t getOccurrenceLength(const byte *newPtr, size_t newLength,
                           const byte *oldPtr, size_t oldLength,
                           size_t &displacement, size_t minDisplacement = 1) {
	displacement = 0;

	// Can we actually do anything with the data?
	if ((minDisplacement > oldLength) || (newLength == 0))
		return 0;

	size_t maxLength = 0;
	for (size_t i = 0; i < oldLength - minDisplacement; i++) {
		/* Try out every single possible displacement value, from the
		 * start of the old data to the end. */

		const byte *currentOldPtr = oldPtr + i;
		size_t currentLength = 0;

		/* Determine the length we can copy if we go back (oldLength - i) bytes.
		 *  Always check the next newLength bytes, because we do LZSS. */
		for (size_t j = 0; j < newLength; j++, currentLength++)
			if (currentOldPtr[j] != newPtr[j])
				break;

		if (currentLength > maxLength) {
			// We've bettered our last try
			maxLength = currentLength;
			displacement = oldLength - i;

			// If we can't get any better than that, stop
			if (maxLength == newLength)
				break;
		}
	}

	return maxLength;
}

/* Simple LZSS 0x10 compression.
 *
 * Code loosely based on DSDecmp by Barubary, released under the terms of the MIT license.
 *
 * See <https://github.com/gravgun/dsdecmp/blob/master/CSharp/DSDecmp/Formats/Nitro/LZ10.cs#L249>
 * and <https://code.google.com/p/dsdecmp/>.
 */
static void compress10(Common::ReadStream &in, Common::WriteStream &small, uint32 size) {
	Common::ScopedArray<byte> inBuffer(new byte[size]);
	if (in.read(inBuffer.get(), size) != size)
		throw Common::Exception(Common::kReadError);

	// Buffer for 8 blocks (max. 2 bytes each), plus their flags byte
	byte outBuffer[8 * 2 + 1] = { 0 };
	size_t bufferedBlocks = 0, bufferLength = 1;

	size_t inRead = 0;
	while (inRead < size) {
		// If 8 blocks have been buffered, write them and reset the buffer
		if (bufferedBlocks == 8) {
			if (small.write(outBuffer, bufferLength) != bufferLength)
				throw Common::Exception(Common::kWriteError);

			bufferedBlocks = 0;
			bufferLength   = 1;

			outBuffer[0] = 0x00;
		}

		/* Look for duplications in the input data:
		 * Try to find an occurrence of data starting from the current place in the
		 * data within the last 0x1000 bytes bytes (the maximum displacement the
		 * format supports) of the already compressed data, but only check the next
		 * 0x12 bytes (the maximum copy length). */

		const size_t newLength = MIN<size_t>(size - inRead, 0x12);
		const size_t oldLength = MIN<size_t>(inRead, 0x1000);

		size_t displacement = 0;
		const size_t length =
			getOccurrenceLength(inBuffer.get() + inRead            , newLength,
			                    inBuffer.get() + inRead - oldLength, oldLength,
			                    displacement);

		/* If the length of the occurrence is at least 3 bytes, we safe space by
		 * referring to the earlier place in the data. If it's shorter (or even
		 * non-existent), then just encode the next byte literally. */

		if (length >= 3) {
			inRead += length;

			// Mark the block as compressed
			outBuffer[0] |= 1 << (7 - bufferedBlocks);

			outBuffer[bufferLength  ]  = ((length       - 3) << 4) & 0xF0;
			outBuffer[bufferLength++] |= ((displacement - 1) >> 8) & 0x0F;
			outBuffer[bufferLength++]  =  (displacement - 1)       & 0xFF;
		} else
			outBuffer[bufferLength++] = inBuffer[inRead++];

		bufferedBlocks++;
	}

	// Write the remaining blocks
	if (bufferedBlocks > 0)
		if (small.write(outBuffer, bufferLength) != bufferLength)
			throw Common::Exception(Common::kWriteError);
}

static void decompress(Common::ReadStream &small, Common::WriteStream &out,
                       uint32 type, uint32 size) {

	if      (type == 0x00)
		decompress00(small, out, size);
	else if (type == 0x10)
		decompress10(small, out, size);
	else
		throw Common::Exception("Unsupported type 0x%08X", (uint) type);
}

void Small::decompress(Common::ReadStream &small, Common::WriteStream &out) {
	uint32 type, size;
	readSmallHeader(small, type, size);

	try {
		::Aurora::decompress(small, out, type, size);
	} catch (Common::Exception &e) {
		e.add("Failed to decompress \"small\" file");
		throw e;
	}

}

Common::SeekableReadStream *Small::decompress(Common::SeekableReadStream *small) {
	Common::ScopedPtr<Common::SeekableReadStream> in(small);

	assert(in);

	uint32 type, size;
	readSmallHeader(*in, type, size);

	const size_t pos = in->pos();

	if (type == 0x00)
		// Uncompressed. Just return a sub stream for the raw data
		return new Common::SeekableSubReadStream(in.release(), pos, pos + size, true);

	Common::MemoryWriteStreamDynamic out(true, size);

	try {
		::Aurora::decompress(*in, out, type, size);
	} catch (Common::Exception &e) {
		e.add("Failed to decompress \"small\" file");
		throw e;
	}

	out.setDisposable(false);
	return new Common::MemoryReadStream(out.getData(), out.size(), true);
}

Common::SeekableReadStream *Small::decompress(Common::ReadStream &small) {
	uint32 type, size;
	readSmallHeader(small, type, size);

	Common::MemoryWriteStreamDynamic out(true, size);

	try {
		::Aurora::decompress(small, out, type, size);
	} catch (Common::Exception &e) {
		e.add("Failed to decompress \"small\" file");
		throw e;
	}

	out.setDisposable(false);
	return new Common::MemoryReadStream(out.getData(), out.size(), true);
}

Common::SeekableReadStream *Small::decompress(Common::ReadStream *small) {
	Common::ScopedPtr<Common::ReadStream> in(small);

	assert(in);
	return decompress(*in);
}

void Small::compress00(Common::SeekableReadStream &in, Common::WriteStream &small) {
	const size_t size = in.size() - in.pos();
	if (size >= 0xFFFFFF)
		throw Common::Exception("Small::compress00(): Input stream too large");

	writeSmallHeader(small, 0x00, size);
	::Aurora::compress00(in, small, size);
}

void Small::compress10(Common::SeekableReadStream &in, Common::WriteStream &small) {
	const size_t size = in.size() - in.pos();
	if (size >= 0xFFFFFF)
		throw Common::Exception("Small::compress10(): Input stream too large");

	writeSmallHeader(small, 0x10, size);
	::Aurora::compress10(in, small, size);
}

} // End of namespace Aurora
