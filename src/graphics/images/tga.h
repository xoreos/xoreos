/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/tga.h
 *  Decoding TGA (TarGa) images.
 */

#ifndef GRAPHICS_IMAGES_TGA_H
#define GRAPHICS_IMAGES_TGA_H

#include "graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

class TGA : public ImageDecoder {
public:
	TGA();
	~TGA();

	void clear();

	void load(Common::SeekableReadStream &tga);

	int getWidth() const;
	int getHeight() const;

	PixelFormat getFormat() const;

	const byte *getData() const;

private:
	int _width;
	int _height;

	PixelFormat _format;

	byte *_data;

	void readHeader(Common::SeekableReadStream &tga);
	void readData(Common::SeekableReadStream &tga);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TGA_H
