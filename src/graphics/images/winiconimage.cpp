/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/winiconimage.cpp
 *  Decoding Windows icon and cursor files (.ICO and .CUR).
 */

#include "common/util.h"
#include "common/stream.h"
#include "common/error.h"
#include "common/strutil.h"

#include "graphics/images/winiconimage.h"

namespace Graphics {

WinIconImage::WinIconImage(Common::SeekableReadStream *cur) :
	_cur(cur), _format(kPixelFormatBGRA), _formatRaw(kPixelFormatRGBA8),
	_hotspotX(0), _hotspotY(0) {

	assert(_cur);
}

WinIconImage::~WinIconImage() {
}

void WinIconImage::load() {
	if (!_cur)
		return;

	try {

		readHeader(*_cur);
		readData(*_cur);

		if (_cur->err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading CUR file");
		throw e;
	}

	delete _cur;
	_cur = 0;
}

void WinIconImage::readHeader(Common::SeekableReadStream &cur) {
	if (!cur.seek(0))
		throw Common::Exception(Common::kSeekError);

	if (cur.readUint16LE() != 0)
		throw Common::Exception("Reserved bytes != 0");

	_iconType = cur.readUint16LE();
	if (_iconType != 1 && _iconType != 2)
		throw Common::Exception("CUR/ICO resource is not a cursor/icon");

	_imageCount = cur.readUint16LE();

	if (!_imageCount)
		throw Common::Exception("No image count");

	warning("-> imageCount: %d", _imageCount);
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
		warning("reading hotspot?");
		_hotspotX = cur.readUint16LE();
		_hotspotY = cur.readUint16LE();
	} else {
		// Icons have a 'planes' field and a 'bits per pixel' pixel
		// field, but both are unused.
		cur.readUint16LE();
		cur.readUint16LE();
	}

	/* uint32 size = */ cur.readUint32LE();
	uint32 offset = cur.readUint32LE();

	// Welcome to the cursor data. Let's ride through the Win bitmap header v3
	cur.seek(offset);

	/* uint32 headerSize = */ cur.readUint32LE();
	/* uint32 bitmapWidth = */ cur.readUint32LE();
	/* uint32 bitmapHeight = */ cur.readUint32LE();
	/* uint16 planes = */ cur.readUint16LE();
	uint16 bitsPerPixel = cur.readUint16LE();
	/* uint32 compression = */ cur.readUint32LE();
	/* uint32 imageSize = */ cur.readUint32LE();
	cur.skip(16); // Skip the rest

	// We're only using 8bpp for now. If more is required, DrMcCoy will
	// volunteer to add it.
	if (bitsPerPixel != 8)
		throw Common::Exception("Unhandled bpp %d", bitsPerPixel);

	// Now we're at the palette. Read it in.
	byte palette[256 * 4];
	cur.read(palette, 256 * 4);

	// The XOR map
	byte *xorMap = new byte[width * height];
	cur.read(xorMap, width * height);

	// The AND map
	uint32 andWidth = (width + 7) / 8;
	byte *andMap = new byte[andWidth * height];
	cur.read(andMap, andWidth * height);

	_format    = kPixelFormatBGRA;
	_formatRaw = kPixelFormatRGBA8;
	
	_image.width = width;
	_image.height = height;
	_image.size = width * height * 4;
	_image.data = new byte[_image.size];

	byte *xorSrc = xorMap;
	byte *dst = _image.data;
	

	for (uint32 y = 0; y < height; y++) {
		byte *andSrc = andMap + andWidth * y;

		for (uint32 x = 0; x < width; x++) {
			byte pixel = *xorSrc++;

			*dst++ = palette[pixel * 4];
			*dst++ = palette[pixel * 4 + 1];
			*dst++ = palette[pixel * 4 + 2];
			*dst++ = (andSrc[x / 8] & (1 << (7 - x % 8))) ? 0 : 0xff;
		}
	}

	delete[] xorMap;
	delete[] andMap;
}

bool WinIconImage::isCompressed() const {
	// We've got no compression here
	return false;
}

PixelFormat WinIconImage::getFormat() const {
	return _format;
}

PixelFormatRaw WinIconImage::getFormatRaw() const {
	return _formatRaw;
}

PixelDataType WinIconImage::getDataType() const {
	// The pixels are always 888(8)
	return kPixelDataType8;
}

int WinIconImage::getMipMapCount() const {
	// We're just using one image from here
	return 1;
}

const WinIconImage::MipMap &WinIconImage::getMipMap(int mipMap) const {
	return _image;
}

WinIconImage::MipMap &WinIconImage::getMipMap(int mipMap) {
	return _image;
}

int WinIconImage::getHotspotX() const {
	return _hotspotX;
}

int WinIconImage::getHotspotY() const {
	return _hotspotY;
}

} // End of namespace Graphics
