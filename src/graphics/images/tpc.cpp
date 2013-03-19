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

/** @file graphics/images/tpc.cpp
 *  TPC (BioWare's own texture format) loading.
 */

#include "common/util.h"
#include "common/maths.h"
#include "common/error.h"
#include "common/stream.h"

#include "graphics/util.h"

#include "graphics/images/tpc.h"

static const byte kEncodingRGB          = 0x02;
static const byte kEncodingRGBA         = 0x04;
static const byte kEncodingSwizzledBGRA = 0x0C;

namespace Graphics {

TPC::TPC(Common::SeekableReadStream &tpc) : _txiData(0), _txiDataSize(0) {
	load(tpc);
}

TPC::~TPC() {
	delete[] _txiData;
}

void TPC::load(Common::SeekableReadStream &tpc) {
	try {

		bool needDeSwizzle = false;

		readHeader (tpc, needDeSwizzle);
		readData   (tpc, needDeSwizzle);
		readTXIData(tpc);

		if (tpc.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading TPC file");
		throw;
	}
}

Common::SeekableReadStream *TPC::getTXI() const {
	if (!_txiData || (_txiDataSize == 0))
		return 0;

	return new Common::MemoryReadStream(_txiData, _txiDataSize);
}

void TPC::readHeader(Common::SeekableReadStream &tpc, bool &needDeSwizzle) {
	// Number of bytes for the pixel data in one full image
	uint32 dataSize = tpc.readUint32LE();

	tpc.skip(4); // Some float

	// Image dimensions
	uint32 width  = tpc.readUint16LE();
	uint32 height = tpc.readUint16LE();

	// How's the pixel data encoded?
	byte encoding    = tpc.readByte();
	// Number of mip maps in the image
	byte mipMapCount = tpc.readByte();

	tpc.skip(114); // Reserved

	needDeSwizzle = false;

	uint32 minDataSize = 0;
	if (dataSize == 0) {
		// Uncompressed

		_compressed = false;

		if        (encoding == kEncodingRGB) {
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

			needDeSwizzle = true;

			_hasAlpha   = true;
			_format     = kPixelFormatBGRA;
			_formatRaw  = kPixelFormatRGBA8;
			_dataType   = kPixelDataType8;

			minDataSize = 4;
			dataSize    = width * height * 4;
		} else
			throw Common::Exception("Unknown TPC raw encoding: %d (%d)", encoding, dataSize);

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

	uint32 fullDataSize = tpc.size() - 128;

	_mipMaps.reserve(mipMapCount);
	for (uint32 i = 0; i < mipMapCount; i++) {
		MipMap *mipMap = new MipMap;

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

void TPC::readData(Common::SeekableReadStream &tpc, bool needDeSwizzle) {
	for (std::vector<MipMap *>::iterator mipMap = _mipMaps.begin(); mipMap != _mipMaps.end(); ++mipMap) {

		// If the texture width is a power of two, the texture memory layout is "swizzled"
		const bool widthPOT = ((*mipMap)->width & ((*mipMap)->width - 1)) == 0;
		const bool swizzled = needDeSwizzle && widthPOT;

		(*mipMap)->data = new byte[(*mipMap)->size];

		if (swizzled) {
			std::vector<byte> tmp((*mipMap)->size);

			if (tpc.read(&tmp[0], (*mipMap)->size) != (*mipMap)->size)
				throw Common::Exception(Common::kReadError);

			deSwizzle((*mipMap)->data, &tmp[0], (*mipMap)->width, (*mipMap)->height);

		} else {
			if (tpc.read((*mipMap)->data, (*mipMap)->size) != (*mipMap)->size)
				throw Common::Exception(Common::kReadError);
		}

	}
}

void TPC::readTXIData(Common::SeekableReadStream &tpc) {
	// TXI data for the rest of the TPC
	_txiDataSize = tpc.size() - tpc.pos();

	if (_txiDataSize == 0)
		return;

	_txiData = new byte[_txiDataSize];

	if (tpc.read(_txiData, _txiDataSize) != _txiDataSize)
		throw Common::Exception(Common::kReadError);
}

} // End of namespace Graphics
