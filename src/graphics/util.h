/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/util.h
 *  Graphics related utility functions.
 */

#ifndef GRAPHICS_UTIL_H
#define GRAPHICS_UTIL_H

#include "common/types.h"
#include "common/util.h"

namespace Graphics {

/** Flip an image horizontally. */
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

/** Flip an image vertically. */
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
