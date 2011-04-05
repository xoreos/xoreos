/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/dumptga.h
 *  A simple TGA image dumper.
 */

#ifndef GRAPHICS_IMAGES_DUMPTGA_H
#define GRAPHICS_IMAGES_DUMPTGA_H

#include "common/types.h"

#include "graphics/types.h"

namespace Common {
	class UString;
}

namespace Graphics {

class ImageDecoder;

/** Dump raw image data into a TGA file. */
void dumpTGA(const Common::UString &fileName, const byte *data, int width, int height, PixelFormat format);
/** Dump image into a TGA file. */
void dumpTGA(const Common::UString &fileName, const ImageDecoder *image);

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_DUMPTGA_H
