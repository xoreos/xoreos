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

#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "src/aurora/smallfile.h"

namespace Aurora {

static void readSmallHeader(Common::SeekableReadStream &small, uint32 &type, uint32 &size) {
	uint32 data = small.readUint32LE();

	type = data & 0x000000FF;
	size = data >> 8;
}

static void decompress00(Common::SeekableReadStream &small, Common::WriteStream &out, uint32 size) {
	out.writeStream(small, size);
}

/* Simple LZSS decompression.
 *
 * Code loosely based on DSDecmp by Barubary, released under the terms of the MIT license.
 *
 * See <https://github.com/gravgun/dsdecmp/blob/master/CSharp/DSDecmp/Formats/Nitro/LZ10.cs#L121>
 * and <https://code.google.com/p/dsdecmp/>.
 */
static void decompress10(Common::SeekableReadStream &small, Common::WriteStream &out, uint32 size) {
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

static void decompress(Common::SeekableReadStream &small, Common::WriteStream &out,
                       uint32 type, uint32 size) {

	if      (type == 0x00)
		decompress00(small, out, size);
	else if (type == 0x10)
		decompress10(small, out, size);
	else
		throw Common::Exception("Unsupported type 0x%08X", (uint) type);
}

void Small::decompress(Common::SeekableReadStream &small, Common::WriteStream &out) {
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
	uint32 type, size;
	readSmallHeader(*small, type, size);

	if (type == 0x00)
		// Uncompressed. Just return a sub stream for the raw data
		return new Common::SeekableSubReadStream(small, small->pos(), small->pos() + size, true);

	Common::MemoryWriteStreamDynamic out(false, size);

	try {
		::Aurora::decompress(*small, out, type, size);
	} catch (Common::Exception &e) {
		out.dispose();

		e.add("Failed to decompress \"small\" file");
		throw e;
	}

	delete small;
	return new Common::MemoryReadStream(out.getData(), out.size(), true);
}

Common::SeekableReadStream *Small::decompress(Common::SeekableReadStream &small) {
	uint32 type, size;
	readSmallHeader(small, type, size);

	Common::MemoryWriteStreamDynamic out(false, size);

	try {
		::Aurora::decompress(small, out, type, size);
	} catch (Common::Exception &e) {
		out.dispose();

		e.add("Failed to decompress \"small\" file");
		throw e;
	}

	return new Common::MemoryReadStream(out.getData(), out.size(), true);
}

} // End of namespace Aurora
