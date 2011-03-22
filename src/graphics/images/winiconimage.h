/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/winiconimage.h
 *  Decoding Windows icon and cursor files (.ICO and .CUR).
 */

#ifndef GRAPHICS_IMAGES_WINICONIMAGE_H
#define GRAPHICS_IMAGES_WINICONIMAGE_H

#include "graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** Windows cursor. */
class WinIconImage : public ImageDecoder {
public:
	WinIconImage(Common::SeekableReadStream *cur);
	~WinIconImage();

	void load();

	bool isCompressed() const;
	bool hasAlpha() const;

	PixelFormat    getFormat() const;
	PixelFormatRaw getFormatRaw() const;
	PixelDataType  getDataType() const;

	int getMipMapCount() const;

	const MipMap &getMipMap(int mipMap) const;

	int getHotspotX() const;
	int getHotspotY() const;

private:
	Common::SeekableReadStream *_cur;

	PixelFormat    _format;
	PixelFormatRaw _formatRaw;

	MipMap _image;

	MipMap &getMipMap(int mipMap);

	uint16 _imageCount;
	uint16 _iconType;

	int _hotspotX;
	int _hotspotY;

	// Loading helpers
	void readHeader(Common::SeekableReadStream &tga);
	void readData(Common::SeekableReadStream &tga);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_WINICONIMAGE_H
