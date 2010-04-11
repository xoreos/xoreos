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
	ImageDecoder() { }
	virtual ~ImageDecoder() { }

	virtual void load() = 0;

	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;

	virtual PixelFormat getFormat() const = 0;

	virtual const byte *getData() const = 0;
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_DECODER_H
