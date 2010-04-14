/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/tpc.cpp
 *  TPC (BioWare's own texture format) loading.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "graphics/images/tpc.h"
#include "graphics/graphics.h"

static const byte kEncodingRGB  = 0x02;
static const byte kEncodingRGBA = 0x04;

namespace Graphics {

TPC::TPC(Common::SeekableReadStream *tpc) : _tpc(tpc), _compressed(true),
	_format(kPixelFormatRGB), _formatRaw(kPixelFormatDXT1), _dataType(kPixelDataType8) {

	assert(_tpc);
}

TPC::~TPC() {
	for (std::vector<MipMap *>::iterator mipMap = _mipMaps.begin(); mipMap != _mipMaps.end(); ++mipMap)
		delete[] *mipMap;
}

void TPC::load() {
	if (!_tpc)
		return;

	try {

		readHeader(*_tpc);
		readData(*_tpc);

		if (_tpc->err())
			throw Common::Exception(Common::kReadError);

		if (GfxMan.needManualDeS3TC())
			uncompress();

	} catch (Common::Exception &e) {
		e.add("Failed reading TPC file");
		throw e;
	}

	delete _tpc;
	_tpc = 0;
}

bool TPC::isCompressed() const {
	return _compressed;
}

PixelFormat TPC::getFormat() const {
	return _format;
}

PixelFormatRaw TPC::getFormatRaw() const {
	return _formatRaw;
}

PixelDataType TPC::getDataType() const {
	return _dataType;
}

int TPC::getMipMapCount() const {
	return _mipMaps.size();
}

const TPC::MipMap &TPC::getMipMap(int mipMap) const {
	return *_mipMaps[mipMap];
}

TPC::MipMap &TPC::getMipMap(int mipMap) {
	return *_mipMaps[mipMap];
}

void TPC::setFormat(PixelFormat format, PixelFormatRaw formatRaw, PixelDataType dataType) {
	_format    = format;
	_formatRaw = formatRaw;
	_dataType  = dataType;
}

void TPC::readHeader(Common::SeekableReadStream &tpc) {
	uint32 dataSize = tpc.readUint32LE();

	tpc.skip(4); // Reserved

	uint32 width  = tpc.readUint16LE();
	uint32 height = tpc.readUint16LE();

	byte encoding = tpc.readByte();

	tpc.skip(115); // Reserved / TXI?

	uint32 minDataSize = 0;
	if (dataSize == 0) {
		// Uncompressed

		_compressed = false;

		if        (encoding == kEncodingRGB) {
			_format     = kPixelFormatRGB;
			_formatRaw  = kPixelFormatRGB8;
			_dataType   = kPixelDataType8;

			minDataSize = 3;
			dataSize    = width * height * 3;
		} else if (encoding == kEncodingRGBA) {
			_format     = kPixelFormatRGBA;
			_formatRaw  = kPixelFormatRGBA8;
			_dataType   = kPixelDataType8;

			minDataSize = 4;
			dataSize    = width * height * 4;
		} else
			throw Common::Exception("Unkown TPC raw encoding: %d (%d)", encoding, dataSize);


	} else if (encoding == kEncodingRGB) {
		// S3TC DXT1

		_compressed = true;
		_format     = kPixelFormatBGR;
		_formatRaw  = kPixelFormatDXT1;
		_dataType   = kPixelDataType8;

		minDataSize = 8;

	} else if (encoding == kEncodingRGBA) {
		// S3TC DXT5

		_compressed = true;
		_format     = kPixelFormatBGRA;
		_formatRaw  = kPixelFormatDXT5;
		_dataType   = kPixelDataType8;

		minDataSize = 16;

	} else
		throw Common::Exception("Unkown TPC encoding: %d (%d)", encoding, dataSize);

	uint32 fullDataSize = tpc.size() - 128;

	do {
		MipMap *mipMap = new MipMap;

		mipMap->width  = MAX<uint32>(width,  1);
		mipMap->height = MAX<uint32>(height, 1);

		mipMap->size = MAX<uint32>(dataSize, minDataSize);

		mipMap->data = 0;

		if (fullDataSize < mipMap->size)
			// Wouldn't fit
			break;

		fullDataSize -= mipMap->size;

		_mipMaps.push_back(mipMap);

		width    >>= 1;
		height   >>= 1;
		dataSize >>= 2;

	} while ((width >= 1) && (height >= 1));
}

void TPC::readData(Common::SeekableReadStream &tpc) {
	for (std::vector<MipMap *>::iterator mipMap = _mipMaps.begin(); mipMap != _mipMaps.end(); ++mipMap) {
		(*mipMap)->data = new byte[(*mipMap)->size];

		if (tpc.read((*mipMap)->data, (*mipMap)->size) != (*mipMap)->size)
			throw Common::Exception(Common::kReadError);
	}
}

} // End of namespace Graphics
