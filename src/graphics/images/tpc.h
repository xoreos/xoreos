/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/tpc.h
 *  TPC (BioWare's own texture format) loading.
 */

#ifndef GRAPHICS_IMAGES_TPC_H
#define GRAPHICS_IMAGES_TPC_H

#include <vector>

#include "graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

class TPC : public ImageDecoder {
public:
	TPC(Common::SeekableReadStream *tpc);
	~TPC();

	void load();

	bool isCompressed() const;

	PixelFormat    getFormat() const;
	PixelFormatRaw getFormatRaw() const;
	PixelDataType  getDataType() const;

	int getMipMapCount() const;

	const MipMap &getMipMap(int mipMap) const;

private:
	Common::SeekableReadStream *_tpc;

	bool _compressed;

	PixelFormat    _format;
	PixelFormatRaw _formatRaw;
	PixelDataType  _dataType;

	std::vector<MipMap *> _mipMaps;

	MipMap &getMipMap(int mipMap);

	void setFormat(PixelFormat format, PixelFormatRaw formatRaw, PixelDataType dataType);

	void readHeader(Common::SeekableReadStream &tpc);
	void readData(Common::SeekableReadStream &tpc);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TPC_H
