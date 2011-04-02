/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/sbm.cpp
 *  Decoding SBM (font bitmap data).
 */

#include "common/util.h"
#include "common/stream.h"
#include "common/error.h"

#include "graphics/images/sbm.h"

namespace Graphics {

SBM::SBM(Common::SeekableReadStream *sbm) : _sbm(sbm) {
	assert(_sbm);
}

SBM::~SBM() {
}

void SBM::load() {
	if (!_sbm)
		return;

	try {

		readData(*_sbm);

		if (_sbm->err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading SBM file");
		throw e;
	}

	delete _sbm;
	_sbm = 0;
}

void SBM::readData(Common::SeekableReadStream &sbm) {
	if ((sbm.size() % 1024) != 0)
		throw Common::Exception("Invalid SBM (%d)", sbm.size());

	uint32 rowCount = (sbm.size() / 1024);

	_image.width  = 4 * 32;
	_image.height = NEXTPOWER2((uint32) rowCount * 32);
	_image.size   = _image.width * _image.height * 4;

	_image.data = new byte[_image.size];

	// SBM data consists of character sized 32 * 32 pixels, with 2 bits per pixel.
	// 4 characters each are on top of each other, occupying the same x/y
	// coordinates but different planes.
	// We'll unpack them and draw them next to each other instead.

	int masks [4] = { 0x03, 0x0C, 0x30, 0xC0 };
	int shifts[4] = {    0,    2,    4,    6 };

	byte *data = _image.data;
	byte buffer[1024];
	for (uint32 c = 0; c < rowCount; c++) {

		if (sbm.read(buffer, 1024) != 1024)
			throw Common::Exception(Common::kReadError);

		for (int y = 0; y < 32; y++) {
			byte *src = buffer + y * 32;

			for (int plane = 0; plane < 4; plane++) {
				for (int x = 0; x < 32; x++) {
					byte a = ((src[x] & masks[plane]) >> shifts[plane]) * 0x55;

					*data++ = 0xFF; // B
					*data++ = 0xFF; // G
					*data++ = 0xFF; // R
					*data++ = a;    // A
				}
			}
		}
	}

	byte *dataEnd = _image.data + _image.size;
	memset(data, dataEnd - data, 0);
}

bool SBM::isCompressed() const {
	// SBMs are never compressed
	return false;
}

bool SBM::hasAlpha() const {
	// SBMs always have alpha
	return true;
}

PixelFormat SBM::getFormat() const {
	// SBM pixels always BGRA
	return kPixelFormatBGRA;
}

PixelFormatRaw SBM::getFormatRaw() const {
	// SBM pixels always RGBA8
	return kPixelFormatRGBA8;
}

PixelDataType SBM::getDataType() const {
	// SBM pixels always 888(8)
	return kPixelDataType8;
}

int SBM::getMipMapCount() const {
	// Just one image in SBMs
	return 1;
}

const SBM::MipMap &SBM::getMipMap(int mipMap) const {
	return _image;
}

SBM::MipMap &SBM::getMipMap(int mipMap) {
	return _image;
}

} // End of namespace Graphics
