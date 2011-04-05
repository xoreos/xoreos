/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** DirectDraw Surface.
 *
 *  Both standard DDS files and BioWare's own version are supported.
 */
class DDS : public ImageDecoder {
public:
	DDS(Common::SeekableReadStream *dds);
	~DDS();

	void load();

private:
	/** The specific pixel format of the included image data. */
	struct DDSPixelFormat {
		uint32 size;     ///< The size of the image data in bytes.
		uint32 flags;    ///< Features of the image data.
		uint32 fourCC;   ///< The FourCC to detect the format by.
		uint32 bitCount; ///< Number of bits per pixel.
		uint32 rBitMask; ///< Bit mask for the red color component.
		uint32 gBitMask; ///< Bit mask for the green color component.
		uint32 bBitMask; ///< Bit mask for the blue color component.
		uint32 aBitMask; ///< Bit mask for the alpha component.
	};

	Common::SeekableReadStream *_dds;

	// Loading helpers
	void readHeader(Common::SeekableReadStream &dds);
	void readStandardHeader(Common::SeekableReadStream &dds);
	void readBioWareHeader(Common::SeekableReadStream &dds);
	void readData(Common::SeekableReadStream &dds);

	void detectFormat(const DDSPixelFormat &format);

	void setSize(MipMap &mipMap);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_DDS_H
