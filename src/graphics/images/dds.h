/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/dds.h
 *  DDS (DirectDraw Surface) loading.
 */

#ifndef GRAPHICS_IMAGES_DDS_H
#define GRAPHICS_IMAGES_DDS_H

#include <vector>

#include "common/types.h"

#include "graphics/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

class DDS {
public:
	struct MipMap {
		int    width;
		int    height;
		uint32 size;
		byte  *data;
	};

	DDS(Common::SeekableReadStream *dds);
	~DDS();

	void load();

	bool isCompressed() const;

	PixelFormat    getFormat() const;
	PixelFormatRaw getFormatRaw() const;
	PixelDataType  getDataType() const;

	int getMipMapCount() const;

	const MipMap &getMipMap(int mipMap) const;

private:
	struct DDSPixelFormat {
		uint32 size;
		uint32 flags;
		uint32 fourCC;
		uint32 bitCount;
		uint32 rBitMask;
		uint32 gBitMask;
		uint32 bBitMask;
		uint32 aBitMask;
	};

	Common::SeekableReadStream *_dds;

	bool _compressed;

	PixelFormat    _format;
	PixelFormatRaw _formatRaw;
	PixelDataType  _dataType;

	std::vector<MipMap> _mipMaps;

	void readHeader(Common::SeekableReadStream &dds);
	void readStandardHeader(Common::SeekableReadStream &dds);
	void readBioWareHeader(Common::SeekableReadStream &dds);
	void readData(Common::SeekableReadStream &dds);

	void detectFormat(const DDSPixelFormat &format);

	void setSize(MipMap &mipMap);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_DDS_H
