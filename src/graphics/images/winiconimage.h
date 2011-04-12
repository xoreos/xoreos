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
	WinIconImage(Common::SeekableReadStream &cur);
	~WinIconImage();

	int getHotspotX() const;
	int getHotspotY() const;

private:
	uint16 _imageCount;
	uint16 _iconType;

	int _hotspotX;
	int _hotspotY;

	// Loading helpers
	void load(Common::SeekableReadStream &cur);
	void readHeader(Common::SeekableReadStream &cur);
	void readData(Common::SeekableReadStream &cur);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_WINICONIMAGE_H
