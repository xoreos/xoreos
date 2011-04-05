/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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


ImageDecoder::ImageDecoder() : _compressed(false), _hasAlpha(false),
	_format(kPixelFormatBGRA), _formatRaw(kPixelFormatRGBA8), _dataType(kPixelDataType8) {

}

ImageDecoder::~ImageDecoder() {
	for (std::vector<MipMap *>::iterator m = _mipMaps.begin(); m != _mipMaps.end(); ++m)
		delete *m;
}

Common::SeekableReadStream *ImageDecoder::getTXI() const {
	return 0;
}

bool ImageDecoder::isCompressed() const {
	return _compressed;
}

bool ImageDecoder::hasAlpha() const {
	return _hasAlpha;
}

PixelFormat ImageDecoder::getFormat() const {
	return _format;
}

PixelFormatRaw ImageDecoder::getFormatRaw() const {
	return _formatRaw;
}

PixelDataType ImageDecoder::getDataType() const {
	return _dataType;
}

uint32 ImageDecoder::getMipMapCount() const {
	return _mipMaps.size();
}

const ImageDecoder::MipMap &ImageDecoder::getMipMap(uint32 mipMap) const {
	assert(mipMap < _mipMaps.size());

	return *_mipMaps[mipMap];
}

void ImageDecoder::decompress(MipMap &out, const MipMap &in, PixelFormatRaw format) {
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
	if (!_compressed)
		return;

	for (std::vector<MipMap *>::iterator m = _mipMaps.begin(); m != _mipMaps.end(); ++m) {
		MipMap decompressed;

		decompress(decompressed, **m, _formatRaw);

		decompressed.swap(**m);
	}

	_format     = kPixelFormatRGBA;
	_formatRaw  = kPixelFormatRGBA8;
	_dataType   = kPixelDataType8;
	_compressed = false;
}

} // End of namespace Graphics
