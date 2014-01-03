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

/** @file graphics/images/decoder.cpp
 *  Generic image decoder interface.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "graphics/graphics.h"

#include "graphics/images/decoder.h"
#include "graphics/images/s3tc.h"
#include "graphics/images/dumptga.h"

namespace Graphics {

ImageDecoder::MipMap::MipMap() : width(0), height(0), size(0), data(0) {
}

ImageDecoder::MipMap::~MipMap() {
	delete[] data;
}

void ImageDecoder::MipMap::swap(MipMap &right) {
	SWAP(width , right.width );
	SWAP(height, right.height);
	SWAP(size  , right.size  );
	SWAP(data  , right.data  );
}


ImageDecoder::ImageDecoder() : _format(kPixelFormatNone) {
}

ImageDecoder::~ImageDecoder() {
	for (std::vector<MipMap *>::iterator m = _mipMaps.begin(); m != _mipMaps.end(); ++m)
		delete *m;
}

Common::SeekableReadStream *ImageDecoder::getTXI() const {
	return 0;
}

bool ImageDecoder::isCompressed() const {
	return isCompressed(_format);
}

bool ImageDecoder::hasAlpha() const {
	return hasAlpha(_format);
}

PixelFormat ImageDecoder::getFormat() const {
	return _format;
}

uint32 ImageDecoder::getMipMapCount() const {
	return _mipMaps.size();
}

const ImageDecoder::MipMap &ImageDecoder::getMipMap(uint32 mipMap) const {
	assert(mipMap < _mipMaps.size());

	return *_mipMaps[mipMap];
}

void ImageDecoder::decompress(MipMap &out, const MipMap &in, PixelFormat format) {
	if ((format != kPixelFormatDXT1) &&
	    (format != kPixelFormatDXT3) &&
	    (format != kPixelFormatDXT5))
		throw Common::Exception("Unknown compressed format %d", format);

	out.width  = in.width;
	out.height = in.height;
	out.size   = out.width * out.height * 4;
	out.data   = new byte[out.size];

	Common::MemoryReadStream *stream = new Common::MemoryReadStream(in.data, in.size);

	if      (format == kPixelFormatDXT1)
		decompressDXT1(out.data, *stream, out.width, out.height, out.width * 4);
	else if (format == kPixelFormatDXT3)
		decompressDXT3(out.data, *stream, out.width, out.height, out.width * 4);
	else if (format == kPixelFormatDXT5)
		decompressDXT5(out.data, *stream, out.width, out.height, out.width * 4);

	delete stream;
}

void ImageDecoder::decompress() {
	if (!isCompressed())
		return;

	for (std::vector<MipMap *>::iterator m = _mipMaps.begin(); m != _mipMaps.end(); ++m) {
		MipMap decompressed;

		decompress(decompressed, **m, _format);

		decompressed.swap(**m);
	}

	_format = kPixelFormatR8G8B8A8;
}

bool ImageDecoder::dumpTGA(const Common::UString &fileName) const {
	if (_mipMaps.size() < 1)
		return false;

	if (!isCompressed()) {
		Graphics::dumpTGA(fileName, this);
		return true;
	}

	MipMap mipMap;
	decompress(mipMap, *_mipMaps[0], _format);
	Graphics::dumpTGA(fileName, mipMap.data, mipMap.width, mipMap.height, kPixelFormatR8G8B8A8);

	return true;
}

uint32 ImageDecoder::calculateSizeInBytes(int width, int height, PixelFormat format) {
	switch (format) {
		case kPixelFormatR8G8B8:
		case kPixelFormatB8G8R8:
			return width * height * 3;

		case kPixelFormatR8G8B8A8:
		case kPixelFormatB8G8R8A8:
			return width * height * 4;

		case kPixelFormatA1R5G5B5:
		case kPixelFormatR5G6B5:
			return width * height * 2;

		case kPixelFormatDXT1:
			return ((width + 3) / 4) * ((height + 3) / 4)  * 8;

		case kPixelFormatDXT3:
			return ((width + 3) / 4) * ((height + 3) / 4) * 16;

		case kPixelFormatDXT5:
			return ((width + 3) / 4) * ((height + 3) / 4) * 16;

		default:
			throw Common::Exception("Can't calculate the size of an image with an invalid pixel format");
	};

	return 0;
}

bool ImageDecoder::isCompressed(PixelFormat format) {
	return (format == kPixelFormatDXT1) ||
	       (format == kPixelFormatDXT3) ||
	       (format == kPixelFormatDXT5);
}

bool ImageDecoder::hasAlpha(PixelFormat format) {
	return (kPixelFormatR8G8B8A8) ||
	       (kPixelFormatB8G8R8A8) ||
	       (kPixelFormatA1R5G5B5) ||
	       (kPixelFormatDXT3) ||
	       (kPixelFormatDXT5);
}

} // End of namespace Graphics
