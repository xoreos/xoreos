/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/txb.h
 *  TXB (another one of BioWare's own texture formats) loading.
 */

#ifndef GRAPHICS_IMAGES_TXB_H
#define GRAPHICS_IMAGES_TXB_H

#include <vector>

#include "graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** Another one of BioWare's own texture formats, TXB. */
class TXB : public ImageDecoder {
public:
	TXB(Common::SeekableReadStream *txb);
	~TXB();

	void load();

	bool isCompressed() const;
	bool hasAlpha() const;

	PixelFormat    getFormat() const;
	PixelFormatRaw getFormatRaw() const;
	PixelDataType  getDataType() const;

	int getMipMapCount() const;

	const MipMap &getMipMap(int mipMap) const;

	/** Return the enclosed TXI data. */
	Common::SeekableReadStream *getTXI() const;

private:
	Common::SeekableReadStream *_txb;

	bool _compressed;
	bool _hasAlpha;

	PixelFormat    _format;
	PixelFormatRaw _formatRaw;
	PixelDataType  _dataType;

	std::vector<MipMap *> _mipMaps;

	uint32 _dataSize;

	byte  *_txiData;
	uint32 _txiDataSize;

	MipMap &getMipMap(int mipMap);

	void setFormat(PixelFormat format, PixelFormatRaw formatRaw, PixelDataType dataType);

	// Loading helpers
	void readHeader(Common::SeekableReadStream &txb);
	void readData(Common::SeekableReadStream &txb);
	void readTXIData(Common::SeekableReadStream &txb);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TXB_H
