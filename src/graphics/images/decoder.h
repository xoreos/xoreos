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
		int    width;    ///< The mip map's width.
		int    height;   ///< The mip map's height.
		uint32 size;     ///< The mip map's size in bytes.
		byte  *data;     ///< The mip map's data.
		byte  *dataOrig; ///< The mip map's original data.

		MipMap();
		~MipMap();
	};

	/** Load the image. */
	virtual void load() = 0;

	/** Is the image data compressed? */
	virtual bool isCompressed() const = 0;

	/** Return the image data's general format. */
	virtual PixelFormat    getFormat() const = 0;
	/** Return the image data's raw format. */
	virtual PixelFormatRaw getFormatRaw() const = 0;
	/** Return the image data pixel's type. */
	virtual PixelDataType  getDataType() const = 0;

	/** Return the number of mip maps contained in the image. */
	virtual int getMipMapCount() const = 0;

	/** Return a mip map. */
	virtual const MipMap &getMipMap(int mipMap) const = 0;

	/** Return TXI data, if embedded in the image. */
	virtual Common::SeekableReadStream *getTXI() const;

protected:
	/** Return a mip map. */
	virtual MipMap &getMipMap(int mipMap) = 0;

	/** Set the format. */
	virtual void setFormat(PixelFormat format, PixelFormatRaw formatRaw, PixelDataType dataType);

	/** Manually decompress texture image data. */
	void decompress();
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_DECODER_H
