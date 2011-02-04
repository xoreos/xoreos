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

#include "common/error.h"
#include "common/stream.h"

#include "graphics/images/decoder.h"
#include "graphics/images/s3tc.h"

namespace Graphics {

ImageDecoder::MipMap::MipMap() {
	width    = 0;
	height   = 0;
	size     = 0;
	data     = 0;
	dataOrig = 0;
}

ImageDecoder::MipMap::~MipMap() {
	delete[] data;
	delete[] dataOrig;
}


ImageDecoder::ImageDecoder() {
}

ImageDecoder::~ImageDecoder() {
}

Common::SeekableReadStream *ImageDecoder::getTXI() const {
	return 0;
}

void ImageDecoder::setFormat(PixelFormat format, PixelFormatRaw formatRaw, PixelDataType dataType) {
	throw Common::Exception("Setting the format is not implemented for this image decoder");
}

void ImageDecoder::decompress() {
	if (!isCompressed())
		return;

	int count = getMipMapCount();
	PixelFormatRaw format = getFormatRaw();

	for (int i = 0; i < count; i++) {
		MipMap &mipMap = getMipMap(i);

		Common::MemoryReadStream *stream = new Common::MemoryReadStream(mipMap.data, mipMap.size);

		mipMap.size     = mipMap.width * mipMap.height * 4;
		mipMap.dataOrig = mipMap.data;
		mipMap.data     = new byte[mipMap.size];

		if (format == kPixelFormatDXT1)
			decompressDXT1(mipMap.data, *stream, mipMap.width, mipMap.height, mipMap.width * 4);
		else if (format == kPixelFormatDXT3)
			decompressDXT3(mipMap.data, *stream, mipMap.width, mipMap.height, mipMap.width * 4);
		else if (format == kPixelFormatDXT5)
			decompressDXT5(mipMap.data, *stream, mipMap.width, mipMap.height, mipMap.width * 4);

		setFormat(kPixelFormatRGBA, kPixelFormatRGBA8, kPixelDataType8);
		delete stream;
	}
}

} // End of namespace Graphics
