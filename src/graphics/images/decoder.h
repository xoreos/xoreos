/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/decoder.h
 *  Generic image decoder interface.
 */

#ifndef GRAPHICS_IMAGES_DECODER_H
#define GRAPHICS_IMAGES_DECODER_H

#include "common/types.h"

#include "graphics/types.h"

namespace Common {
	class SeekableReadStream;
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

	/** Load the image. */
	virtual void load() = 0;

	/** Is the image data compressed? */
	bool isCompressed() const;

	/** Does the image data have alpha? .*/
	bool hasAlpha() const;

	/** Return the image data's general format. */
	PixelFormat    getFormat() const;
	/** Return the image data's raw format. */
	PixelFormatRaw getFormatRaw() const;
	/** Return the image data pixel's type. */
	PixelDataType  getDataType() const;

	/** Return the number of mip maps contained in the image. */
	uint32 getMipMapCount() const;

	/** Return a mip map. */
	const MipMap &getMipMap(uint32 mipMap) const;

	/** Manually decompress the texture image data. */
	void decompress();

	/** Return TXI data, if embedded in the image. */
	virtual Common::SeekableReadStream *getTXI() const;

protected:
	bool _compressed;
	bool _hasAlpha;

	PixelFormat    _format;
	PixelFormatRaw _formatRaw;
	PixelDataType  _dataType;

	std::vector<MipMap *> _mipMaps;

	static void decompress(MipMap &out, const MipMap &in, PixelFormatRaw format);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_DECODER_H
