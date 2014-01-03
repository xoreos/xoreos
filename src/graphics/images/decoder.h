/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/images/decoder.h
 *  Generic image decoder interface.
 */

#ifndef GRAPHICS_IMAGES_DECODER_H
#define GRAPHICS_IMAGES_DECODER_H

#include <vector>

#include "common/types.h"

#include "graphics/types.h"

namespace Common {
	class SeekableReadStream;
	class UString;
}

namespace Graphics {

/** A generic interface for image decoders. */
class ImageDecoder {
public:
	ImageDecoder();
	virtual ~ImageDecoder();

	/** A mip map. */
	struct MipMap {
		int    width;  ///< The mip map's width.
		int    height; ///< The mip map's height.
		uint32 size;   ///< The mip map's size in bytes.
		byte  *data;   ///< The mip map's data.

		MipMap();
		~MipMap();

		void swap(MipMap &right);
	};

	/** Is the image data compressed? */
	bool isCompressed() const;

	/** Does the image data have alpha? .*/
	bool hasAlpha() const;

	/** Return the image data's format. */
	PixelFormat getFormat() const;

	/** Return the number of mip maps contained in the image. */
	uint32 getMipMapCount() const;

	/** Return a mip map. */
	const MipMap &getMipMap(uint32 mipMap) const;

	/** Manually decompress the texture image data. */
	void decompress();

	/** Return TXI data, if embedded in the image. */
	virtual Common::SeekableReadStream *getTXI() const;

	/** Dump the image into a TGA. */
	bool dumpTGA(const Common::UString &fileName) const;


	/** Calculate the size in bytes of an image of the given size and pixel format. */
	static uint32 calculateSizeInBytes(int width, int height, PixelFormat format);
	/** Check whether a pixel format is compressed. */
	static bool isCompressed(PixelFormat format);
	/** Check whether a pixel format has an alpha channel. */
	static bool hasAlpha(PixelFormat format);

protected:
	PixelFormat _format;

	std::vector<MipMap *> _mipMaps;

	static void decompress(MipMap &out, const MipMap &in, PixelFormat format);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_DECODER_H
