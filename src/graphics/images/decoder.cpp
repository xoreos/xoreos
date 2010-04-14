/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

void ImageDecoder::setFormat(PixelFormat format, PixelFormatRaw formatRaw, PixelDataType dataType) {
	throw Common::Exception("Setting the format is not implemented for this image decoder");
}

void ImageDecoder::uncompress() {
	int count = getMipMapCount();
	for (int i = 0; i < count; i++) {
		MipMap &mipMap = getMipMap(i);

		if        (getFormatRaw() == kPixelFormatDXT1) {
			uint32 origSize = mipMap.size;

			mipMap.size     = mipMap.width * mipMap.height * 3;
			mipMap.dataOrig = mipMap.data;
			mipMap.data     = new byte[mipMap.size];

			decompressDXT1(mipMap.data, mipMap.dataOrig, mipMap.size);

			setFormat(kPixelFormatBGR, kPixelFormatRGB8, kPixelDataType8);

		} else if (getFormatRaw() == kPixelFormatDXT3) {
			uint32 origSize = mipMap.size;

			mipMap.size     = mipMap.width * mipMap.height * 4;
			mipMap.dataOrig = mipMap.data;
			mipMap.data     = new byte[mipMap.size];

			decompressDXT3(mipMap.data, mipMap.dataOrig, mipMap.size);

			setFormat(kPixelFormatBGRA, kPixelFormatRGBA8, kPixelDataType8);

		} else if (getFormatRaw() == kPixelFormatDXT5) {
			uint32 origSize = mipMap.size;

			mipMap.size     = mipMap.width * mipMap.height * 4;
			mipMap.dataOrig = mipMap.data;
			mipMap.data     = new byte[mipMap.size];

			decompressDXT5(mipMap.data, mipMap.dataOrig, mipMap.size);

			setFormat(kPixelFormatBGRA, kPixelFormatRGBA8, kPixelDataType8);

		}

	}
}

} // End of namespace Graphics
