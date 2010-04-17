/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/dds.cpp
 *  DDS (DirectDraw Surface) loading.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "graphics/images/dds.h"
#include "graphics/graphics.h"

static const uint32 kDDSID  = MKID_BE('DDS ');
static const uint32 kDXT1ID = MKID_BE('DXT1');
static const uint32 kDXT3ID = MKID_BE('DX13');
static const uint32 kDXT5ID = MKID_BE('DXT5');

static const uint32 kHeaderFlagsHasMipMaps = 0x00020000;

static const uint32 kPixelFlagsHasAlpha  = 0x00000001;
static const uint32 kPixelFlagsHasFourCC = 0x00000004;
static const uint32 kPixelFlagsIsIndexed = 0x00000020;
static const uint32 kPixelFlagsIsRGB     = 0x00000040;

namespace Graphics {

DDS::DDS(Common::SeekableReadStream *dds) : _dds(dds), _compressed(false),
	_format(kPixelFormatRGB), _formatRaw(kPixelFormatDXT1), _dataType(kPixelDataType8) {

	assert(_dds);
}

DDS::~DDS() {
	for (std::vector<MipMap *>::iterator mipMap = _mipMaps.begin(); mipMap != _mipMaps.end(); ++mipMap)
		delete *mipMap;
}

void DDS::load() {
	if (!_dds)
		return;

	try {

		readHeader(*_dds);
		readData(*_dds);

		if (_dds->err())
			throw Common::Exception(Common::kReadError);

		if (GfxMan.needManualDeS3TC()) {
			decompress();
			_compressed = false;
		}

	} catch (Common::Exception &e) {
		e.add("Failed reading DDS file");
		throw e;
	}

	delete _dds;
	_dds = 0;
}

bool DDS::isCompressed() const {
	return _compressed;
}

PixelFormat DDS::getFormat() const {
	return _format;
}

PixelFormatRaw DDS::getFormatRaw() const {
	return _formatRaw;
}

PixelDataType DDS::getDataType() const {
	return _dataType;
}

int DDS::getMipMapCount() const {
	return _mipMaps.size();
}

const DDS::MipMap &DDS::getMipMap(int mipMap) const {
	return *_mipMaps[mipMap];
}

DDS::MipMap &DDS::getMipMap(int mipMap) {
	return *_mipMaps[mipMap];
}

void DDS::setFormat(PixelFormat format, PixelFormatRaw formatRaw, PixelDataType dataType) {
	_format    = format;
	_formatRaw = formatRaw;
	_dataType  = dataType;
}

void DDS::readHeader(Common::SeekableReadStream &dds) {
	if (dds.readUint32BE() == kDDSID)
		// We found the FourCC of a standard DDS
		readStandardHeader(dds);
	else
		// FourCC not found, should be a BioWare DDS then
		readBioWareHeader(dds);
}

void DDS::readStandardHeader(Common::SeekableReadStream &dds) {
	// All DDS header should be 124 bytes (+ 4 for the FourCC) big
	if (dds.readUint32LE() != 124)
		throw Common::Exception("Header size invalid");

	// DDS features
	uint32 flags = dds.readUint32LE();

	// Image dimensions
	uint32 height = dds.readUint32LE();
	uint32 width  = dds.readUint32LE();

	dds.skip(4 + 4); // Pitch + Depth
	//uint32 pitchOrLineSize = dds.readUint32LE();
	//uint32 depth           = dds.readUint32LE();
	uint32 mipMapCount     = dds.readUint32LE();

	// DDS doesn't provide any mip maps, only one full-size image
	if ((flags & kHeaderFlagsHasMipMaps) == 0)
		mipMapCount = 1;

	dds.skip(44); // Reserved

	// Read the pixel data format
	DDSPixelFormat format;
	format.size     = dds.readUint32LE();
	format.flags    = dds.readUint32LE();
	format.fourCC   = dds.readUint32BE();
	format.bitCount = dds.readUint32LE();
	format.rBitMask = dds.readUint32LE();
	format.gBitMask = dds.readUint32LE();
	format.bBitMask = dds.readUint32LE();
	format.aBitMask = dds.readUint32LE();

	// Detect which specific format it describes
	detectFormat(format);

	dds.skip(16 + 4); // DDCAPS2 + Reserved

	_mipMaps.reserve(mipMapCount);
	for (uint32 i = 0; i < mipMapCount; i++) {
		MipMap *mipMap = new MipMap;

		mipMap->width  = MAX<uint32>(width , 1);
		mipMap->height = MAX<uint32>(height, 1);

		setSize(*mipMap);

		mipMap->data = 0;

		width  >>= 1;
		height >>= 1;

		_mipMaps.push_back(mipMap);
	}

}

#define IsPower2(x) ((x) && (((x) & ((x) - 1)) == 0))
void DDS::readBioWareHeader(Common::SeekableReadStream &dds) {
	dds.seek(0);

	// Image dimensions
	uint32 width  = dds.readUint32LE();
	uint32 height = dds.readUint32LE();

	// Check that the width and height are really powers of 2
	if (!IsPower2(width) || !IsPower2(height))
		throw Common::Exception("Width and height must be powers of 2");

	// Always compressed
	_compressed = true;

	// Check which compression
	uint32 bpp = dds.readUint32LE();
	if      (bpp == 3)
		_formatRaw = kPixelFormatDXT1;
	else if (bpp == 4)
		_formatRaw = kPixelFormatDXT5;
	else
		throw Common::Exception("Unsupported bytes per pixel value (%d)", bpp);

	// Sanity check for the image data size
	uint32 dataSize = dds.readUint32LE();
	if (((bpp == 3) && (dataSize != ((width * height) / 2))) ||
		  ((bpp == 4) && (dataSize != ((width * height)    ))))
		throw Common::Exception("Invalid data size (%dx%dx%d %d)", width, height, bpp, dataSize);

	dds.skip(4); // Some float

	// Number of bytes left for the image data
	uint32 fullDataSize = dds.size() - dds.pos();

	// Detect how many mip maps are in the DDS
	do {
		MipMap *mipMap = new MipMap;

		mipMap->width  = MAX<uint32>(width,  1);
		mipMap->height = MAX<uint32>(height, 1);

		setSize(*mipMap);

		if (fullDataSize < mipMap->size) {
			// Wouldn't fit
			delete mipMap;
			break;
		}

		fullDataSize -= mipMap->size;

		_mipMaps.push_back(mipMap);

		width  >>= 1;
		height >>= 1;

	} while ((width >= 1) && (height >= 1));
}

void DDS::setSize(MipMap &mipMap) {
	// Depending on the pixel format, set the image data size in bytes

	if (_formatRaw == kPixelFormatDXT1) {
		mipMap.size = ((mipMap.width + 3) / 4) * ((mipMap.height + 3) / 4) *  8;
	} else if (_formatRaw == kPixelFormatDXT3) {
		mipMap.size = ((mipMap.width + 3) / 4) * ((mipMap.height + 3) / 4) * 16;
	} else if (_formatRaw == kPixelFormatDXT5) {
		mipMap.size = ((mipMap.width + 3) / 4) * ((mipMap.height + 3) / 4) * 16;
	} else if (_formatRaw == kPixelFormatRGBA8) {
		mipMap.size = mipMap.width * mipMap.height * 4;
	} else if (_formatRaw == kPixelFormatRGB8) {
		mipMap.size = mipMap.width * mipMap.height * 3;
	} else if (_formatRaw == kPixelFormatRGB5A1) {
		mipMap.size = mipMap.width * mipMap.height * 2;
	} else if (_formatRaw == kPixelFormatRGB5) {
		mipMap.size = mipMap.width * mipMap.height * 2;
	} else
		mipMap.size = 0;
}

void DDS::readData(Common::SeekableReadStream &dds) {
	// TODO: Do we need to flip the data?

	for (std::vector<MipMap *>::iterator mipMap = _mipMaps.begin(); mipMap != _mipMaps.end(); ++mipMap) {
		(*mipMap)->data = new byte[(*mipMap)->size];

		if (dds.read((*mipMap)->data, (*mipMap)->size) != (*mipMap)->size)
			throw Common::Exception(Common::kReadError);
	}
}

void DDS::detectFormat(const DDSPixelFormat &format) {
	// Big, ugly big pixel format description => format mapping

	if        ((format.flags & kPixelFlagsHasFourCC) && (format.fourCC == kDXT1ID)) {
		_compressed = true;
		_format     = kPixelFormatBGRA;
		_formatRaw  = kPixelFormatDXT1;
		_dataType   = kPixelDataType8;
	} else if ((format.flags & kPixelFlagsHasFourCC) && (format.fourCC == kDXT3ID)) {
		_compressed = true;
		_format     = kPixelFormatBGRA;
		_formatRaw  = kPixelFormatDXT3;
		_dataType   = kPixelDataType8;
	} else if ((format.flags & kPixelFlagsHasFourCC) && (format.fourCC == kDXT5ID)) {
		_compressed = true;
		_format     = kPixelFormatBGRA;
		_formatRaw  = kPixelFormatDXT5;
		_dataType   = kPixelDataType8;
	} else if ((format.flags & kPixelFlagsIsRGB) && (format.flags & kPixelFlagsHasAlpha) &&
	           (format.bitCount == 32) &&
	           (format.rBitMask == 0x00FF0000) && (format.gBitMask == 0x0000FF00) &&
	           (format.bBitMask == 0x000000FF) && (format.aBitMask == 0xFF000000)) {
		_compressed = false;
		_format     = kPixelFormatBGRA;
		_formatRaw  = kPixelFormatRGBA8;
		_dataType   = kPixelDataType8;
	} else if ((format.flags & kPixelFlagsIsRGB) && !(format.flags & kPixelFlagsHasAlpha) &&
	           (format.bitCount == 24) &&
	           (format.rBitMask == 0x00FF0000) && (format.gBitMask == 0x0000FF00) &&
	           (format.bBitMask == 0x000000FF)) {
		_compressed = false;
		_format     = kPixelFormatBGR;
		_formatRaw  = kPixelFormatRGB8;
		_dataType   = kPixelDataType8;

		warning("Found untested DDS RGB8 data");

	} else if ((format.flags & kPixelFlagsIsRGB) && (format.flags & kPixelFlagsHasAlpha) &&
	           (format.bitCount == 16) &&
	           (format.rBitMask == 0x00007C00) && (format.gBitMask == 0x000003E0) &&
	           (format.bBitMask == 0x0000001F) && (format.aBitMask == 0x00008000)) {
		_compressed = false;
		_format     = kPixelFormatBGRA;
		_formatRaw  = kPixelFormatRGB5A1;
		_dataType   = kPixelDataType1555;

		warning("Found untested DDS RGB5A1 data");

	} else if ((format.flags & kPixelFlagsIsRGB) && !(format.flags & kPixelFlagsHasAlpha) &&
	           (format.bitCount == 16) &&
	           (format.rBitMask == 0x0000F800) && (format.gBitMask == 0x000007E0) &&
	           (format.bBitMask == 0x0000001F)) {
		_compressed = false;
		_format     = kPixelFormatBGR;
		_formatRaw  = kPixelFormatRGB5;
		_dataType   = kPixelDataType565;

		warning("Found untested DDS RGB5 data");

	} else if (format.flags & kPixelFlagsIsIndexed)
		// Hopefully, we'll never need to support that :P
		throw Common::Exception("Unsupported feature: Palette");
	else
		// We'll see if there's more formats in the data files :P
		throw Common::Exception("Unknown pixel format");
}

} // End of namespace Graphics
