/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

namespace Graphics {

/** A generic interface for image decoders. */
class ImageDecoder {
public:
	ImageDecoder();
	virtual ~ImageDecoder();

	struct MipMap {
		int    width;
		int    height;
		uint32 size;
		byte  *data;

		MipMap();
		~MipMap();
	};

	virtual void load() = 0;

	virtual bool isCompressed() const = 0;

	virtual PixelFormat    getFormat() const = 0;
	virtual PixelFormatRaw getFormatRaw() const = 0;
	virtual PixelDataType  getDataType() const = 0;

	virtual int getMipMapCount() const = 0;

	virtual const MipMap &getMipMap(int mipMap) const = 0;
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_DECODER_H
