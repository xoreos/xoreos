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
 *  Decoding Windows icon and cursor files (.ICO and .CUR).
 */

#include <memory>

#include "src/common/util.h"
#include "src/common/readstream.h"
#include "src/common/error.h"

#include "src/graphics/images/winiconimage.h"

namespace Graphics {

WinIconImage::WinIconImage(Common::SeekableReadStream &cur) : _hotspotX(0), _hotspotY(0) {
	_compressed = false;
	_hasAlpha   = true;
	_dataType   = kPixelDataType8;

	load(cur);
}

WinIconImage::~WinIconImage() {
}

void WinIconImage::load(Common::SeekableReadStream &cur) {
	try {

		readHeader(cur);
		readData  (cur);

	} catch (Common::Exception &e) {
		e.add("Failed reading CUR file");
		throw;
	}
}

void WinIconImage::readHeader(Common::SeekableReadStream &cur) {
	cur.seek(0);

	if (cur.readUint16LE() != 0)
		throw Common::Exception("Reserved bytes != 0");

	_iconType = cur.readUint16LE();
	if (_iconType != 1 && _iconType != 2)
		throw Common::Exception("CUR/ICO resource is not a cursor/icon");

	_imageCount = cur.readUint16LE();

	if (!_imageCount)
		throw Common::Exception("No image count");
}

void WinIconImage::readData(Common::SeekableReadStream &cur) {
	// For now, use the last image (which should be the highest quality)
	// TODO: Detection of which image to use
	cur.skip((_imageCount - 1) * 16);

	uint width  = cur.readByte();
	uint height = cur.readByte();
	/* byte colorDepth = */ cur.readByte();

	// Not terrible to handle, but whatever :P
	if (width & 3)
		throw Common::Exception("Non-divisible-by-4 images not handled");

	if (width == 0)
		width = 256;
	if (height == 0)
		height = 256;

	if (cur.readByte() != 0)
		throw Common::Exception("Reserved byte != 0");

	if (_iconType == 2) {
		_hotspotX = cur.readUint16LE();
		_hotspotY = cur.readUint16LE();
	} else {
		// Icons have a 'planes' field and a 'bits per pixel' pixel
		// field, but both are unused.
		cur.readUint16LE();
		cur.readUint16LE();
	}

	/* uint32_t size = */ cur.readUint32LE();
	uint32_t offset = cur.readUint32LE();

	// Welcome to the cursor data. Let's ride through the Win bitmap header v3
	cur.seek(offset);

	/* uint32_t headerSize = */ cur.readUint32LE();
	/* uint32_t bitmapWidth = */ cur.readUint32LE();
	/* uint32_t bitmapHeight = */ cur.readUint32LE();
	/* uint16_t planes = */ cur.readUint16LE();
	uint16_t bitsPerPixel = cur.readUint16LE();
	/* uint32_t compression = */ cur.readUint32LE();
	/* uint32_t imageSize = */ cur.readUint32LE();
	cur.skip(16); // Skip the rest

	// We're only using 8bpp/24bpp for now. If more is required, DrMcCoy will
	// volunteer to add it.
	if (bitsPerPixel != 8 && bitsPerPixel != 24)
		throw Common::Exception("Unhandled bpp %d", bitsPerPixel);

	const int pitch = width * (bitsPerPixel / 8);

	// Now we're at the palette. Read it in for 8bpp
	byte palette[256 * 4];

	if (bitsPerPixel == 8)
		cur.read(palette, 256 * 4);

	// The XOR map
	std::unique_ptr<byte[]> xorMap = std::make_unique<byte[]>(pitch * height);
	if (cur.read(xorMap.get(), pitch * height) != (pitch * height))
		throw Common::Exception(Common::kReadError);

	// The AND map
	const uint32_t andWidth = (width + 7) / 8;
	std::unique_ptr<byte[]> andMap = std::make_unique<byte[]>(andWidth * height);
	if (cur.read(andMap.get(), andWidth * height) != (andWidth * height))
		throw Common::Exception(Common::kReadError);

	_format    = kPixelFormatBGRA;
	_formatRaw = kPixelFormatRGBA8;

	_mipMaps.push_back(new MipMap(this));

	_mipMaps[0]->width  = width;
	_mipMaps[0]->height = height;
	_mipMaps[0]->size   = width * height * 4;

	_mipMaps[0]->data = std::make_unique<byte[]>(_mipMaps[0]->size);

	const byte *xorSrc = xorMap.get();
	      byte *dst    = _mipMaps[0]->data.get();

	for (uint32_t y = 0; y < height; y++) {
		const byte *andSrc = andMap.get() + andWidth * y;

		for (uint32_t x = 0; x < width; x++) {
			if (bitsPerPixel == 8) {
				const byte pixel = *xorSrc++;

				*dst++ = palette[pixel * 4];
				*dst++ = palette[pixel * 4 + 1];
				*dst++ = palette[pixel * 4 + 2];
			} else {
				*dst++ = *xorSrc++;
				*dst++ = *xorSrc++;
				*dst++ = *xorSrc++;
			}

			*dst++ = (andSrc[x / 8] & (1 << (7 - x % 8))) ? 0 : 0xff;
		}
	}
}

int WinIconImage::getHotspotX() const {
	return _hotspotX;
}

int WinIconImage::getHotspotY() const {
	return _hotspotY;
}

} // End of namespace Graphics
