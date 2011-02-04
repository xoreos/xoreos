/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/dumpppm.h
 *  A simple PPM image dumper.
 */

#ifndef GRAPHICS_IMAGESDUMPPPM_H
#define GRAPHICS_IMAGESDUMPPPM_H

#include "common/types.h"

#include "graphics/types.h"

namespace Common {
	class UString;
}

namespace Graphics {

class ImageDecoder;

void dumpPPM(const Common::UString &fileName, const byte *data, int width, int height, PixelFormat format);
void dumpPPM(const Common::UString &fileName, const ImageDecoder *image);

} // End of namespace Graphics

#endif // GRAPHICS_IMAGESDUMPPPM_H
