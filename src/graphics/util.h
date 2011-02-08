/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/util.h
 *  Graphics related utility functions.
 */

#ifndef GRAPHICS_UTIL_H
#define GRAPHICS_UTIL_H

#include "common/types.h"
#include "common/util.h"

namespace Graphics {

/** Converting a color from YUV to RGB colorspace. */
inline static void YUV2RGB(byte y, byte u, byte v, byte &r, byte &g, byte &b) {
	r = CLIP<int>(y + ((1357 * (v - 128)) >> 10), 0, 255);
	g = CLIP<int>(y - (( 691 * (v - 128)) >> 10) - ((333 * (u - 128)) >> 10), 0, 255);
	b = CLIP<int>(y + ((1715 * (u - 128)) >> 10), 0, 255);
}

inline static void flipHorizontally(byte *data, int width, int height, int bpp) {
	int halfWidth = width / 2;
	int pitch     = bpp * width;

	byte *buffer = new byte[bpp];

	while (height-- > 0) {
		byte *dataStart = data;
		byte *dataEnd   = data + pitch - bpp;

		for (int j = 0; j < halfWidth; j++) {
			memcpy(buffer   , dataStart, bpp);
			memcpy(dataStart, dataEnd  , bpp);
			memcpy(dataEnd  , buffer   , bpp);

			dataStart += bpp;
			dataEnd   -= bpp;
		}

		data += pitch;
	}

	delete[] buffer;
}

inline static void flipVertically(byte *data, int width, int height, int bpp) {
	int halfHeight = height / 2;
	int pitch      = bpp * width;

	byte *dataStart = data;
	byte *dataEnd   = data + (pitch * height) - pitch;

	byte *buffer = new byte[pitch];

	while (halfHeight--) {
		memcpy(buffer   , dataStart, pitch);
		memcpy(dataStart, dataEnd  , pitch);
		memcpy(dataEnd  , buffer   , pitch);

		dataStart += pitch;
		dataEnd   -= pitch;
	}

	delete[] buffer;
}

} // End of namespace Graphics

#endif // GRAPHICS_UTIL_H
