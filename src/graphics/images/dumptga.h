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
