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
 *  TPC (BioWare's own texture format) loading.
 */

#include <cstring>

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/graphics/util.h"

#include "src/graphics/images/tpc.h"

static const byte kEncodingGray         = 0x01;
static const byte kEncodingRGB          = 0x02;
static const byte kEncodingRGBA         = 0x04;
static const byte kEncodingSwizzledBGRA = 0x0C;

namespace Graphics {

TPC::TPC(Common::SeekableReadStream &tpc) {
	load(tpc);
}

TPC::~TPC() {
}

void TPC::load(Common::SeekableReadStream &tpc) {
	try {

		byte encoding;

		readHeader(tpc, encoding);
		readData  (tpc, encoding);
		readTXI   (tpc);

	} catch (Common::Exception &e) {
		clear();

		e.add("Failed reading TPC file");
		throw;
	}
}

void TPC::readHeader(Common::SeekableReadStream &tpc, byte &encoding) {
	// Number of bytes for the pixel data in one full image
	uint32 dataSize = tpc.readUint32LE();

	tpc.skip(4); // Some float

	// Image dimensions
	uint32 width  = tpc.readUint16LE();
	uint32 height = tpc.readUint16LE();

	// How's the pixel data encoded?
	encoding = tpc.readByte();

	// Number of mip maps in the image
	byte mipMapCount = tpc.readByte();

	tpc.skip(114); // Reserved

	uint32 minDataSize = 0;
	if (dataSize == 0) {
		// Uncompressed

		_compressed = false;

		if        (encoding == kEncodingGray) {
			// 8bpp grayscale

			_hasAlpha   = false;
			_format     = kPixelFormatRGB;
			_formatRaw  = kPixelFormatRGB8;
			_dataType   = kPixelDataType8;

			minDataSize = 1;
			dataSize    = width * height;
		} else if (encoding == kEncodingRGB) {
			// RGB, no alpha channel

			_hasAlpha   = false;
			_format     = kPixelFormatRGB;
			_formatRaw  = kPixelFormatRGB8;
			_dataType   = kPixelDataType8;

			minDataSize = 3;
			dataSize    = width * height * 3;
		} else if (encoding == kEncodingRGBA) {
			// RGBA, alpha channel

			_hasAlpha   = true;
			_format     = kPixelFormatRGBA;
			_formatRaw  = kPixelFormatRGBA8;
			_dataType   = kPixelDataType8;

			minDataSize = 4;
			dataSize    = width * height * 4;
		} else if (encoding == kEncodingSwizzledBGRA) {
			// BGRA, alpha channel, texture memory layout is "swizzled"

			_hasAlpha   = true;
			_format     = kPixelFormatBGRA;
			_formatRaw  = kPixelFormatRGBA8;
			_dataType   = kPixelDataType8;

			minDataSize = 4;
			dataSize    = width * height * 4;
		} else
			throw Common::Exception("Unknown TPC raw encoding: %d (%d), %dx%d, %d", encoding, dataSize, width, height, mipMapCount);

	} else if (encoding == kEncodingRGB) {
		// S3TC DXT1

		_compressed = true;
		_hasAlpha   = false;
		_format     = kPixelFormatBGR;
		_formatRaw  = kPixelFormatDXT1;
		_dataType   = kPixelDataType8;

		minDataSize = 8;

	} else if (encoding == kEncodingRGBA) {
		// S3TC DXT5

		_compressed = true;
		_hasAlpha   = true;
		_format     = kPixelFormatBGRA;
		_formatRaw  = kPixelFormatDXT5;
		_dataType   = kPixelDataType8;

		minDataSize = 16;

	} else
		throw Common::Exception("Unknown TPC encoding: %d (%d)", encoding, dataSize);

	size_t fullDataSize = tpc.size() - 128;

	_mipMaps.reserve(mipMapCount);
	for (uint32 i = 0; i < mipMapCount; i++) {
		MipMap *mipMap = new MipMap(this);

		mipMap->width  = MAX<uint32>(width,  1);
		mipMap->height = MAX<uint32>(height, 1);

		mipMap->size = MAX<uint32>(dataSize, minDataSize);

		mipMap->data = 0;

		if (fullDataSize < mipMap->size) {
			// Wouldn't fit
			delete mipMap;
			break;
		}

		fullDataSize -= mipMap->size;

		_mipMaps.push_back(mipMap);

		width    >>= 1;
		height   >>= 1;
		dataSize >>= 2;

		if ((width < 1) && (height < 1))
			break;
	}

}

void TPC::deSwizzle(byte *dst, const byte *src, uint32 width, uint32 height) {
	for (uint32 y = 0; y < height; y++) {
		for (uint32 x = 0; x < width; x++) {
			const uint32 offset = deSwizzleOffset(x, y, width, height) * 4;

			*dst++ = src[offset + 0];
			*dst++ = src[offset + 1];
			*dst++ = src[offset + 2];
			*dst++ = src[offset + 3];
		}
	}
}

void TPC::readData(Common::SeekableReadStream &tpc, byte encoding) {
	for (std::vector<MipMap *>::iterator mipMap = _mipMaps.begin(); mipMap != _mipMaps.end(); ++mipMap) {

		// If the texture width is a power of two, the texture memory layout is "swizzled"
		const bool widthPOT = ((*mipMap)->width & ((*mipMap)->width - 1)) == 0;
		const bool swizzled = (encoding == kEncodingSwizzledBGRA) && widthPOT;

		(*mipMap)->data = new byte[(*mipMap)->size];

		if (swizzled) {
			std::vector<byte> tmp((*mipMap)->size);

			if (tpc.read(&tmp[0], (*mipMap)->size) != (*mipMap)->size)
				throw Common::Exception(Common::kReadError);

			deSwizzle((*mipMap)->data, &tmp[0], (*mipMap)->width, (*mipMap)->height);

		} else {
			if (tpc.read((*mipMap)->data, (*mipMap)->size) != (*mipMap)->size)
				throw Common::Exception(Common::kReadError);

			// Unpacking 8bpp grayscale data into RGB
			if (encoding == kEncodingGray) {
				byte *dataGray = (*mipMap)->data;

				(*mipMap)->size = (*mipMap)->width * (*mipMap)->height * 3;
				(*mipMap)->data = new byte[(*mipMap)->size];

				for (int i = 0; i < ((*mipMap)->width * (*mipMap)->height); i++)
					memset((*mipMap)->data + i * 3, dataGray[i], 3);

				delete[] dataGray;
			}
		}

	}
}

void TPC::readTXI(Common::SeekableReadStream &tpc) {
	const size_t txiDataSize = tpc.size() - tpc.pos();
	if (txiDataSize == 0)
		return;

	Common::SeekableReadStream *txiData = tpc.readStream(txiDataSize);

	try {
		_txi.load(*txiData);
	} catch (...) {
	}

	delete txiData;
}

} // End of namespace Graphics
