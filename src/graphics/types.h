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
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/types.h
 *  Basic graphics types.
 */

#ifndef GRAPHICS_TYPES_H
#define GRAPHICS_TYPES_H

#include <SDL.h>

#include "glew/glew.h"

namespace Graphics {

enum PixelFormat {
	kPixelFormatRGB  = GL_RGB ,
	kPixelFormatRGBA = GL_RGBA,
	kPixelFormatBGR  = GL_BGR ,
	kPixelFormatBGRA = GL_BGRA,
};

enum PixelFormatRaw {
	kPixelFormatRGBA8  = GL_RGBA8,
	kPixelFormatRGB8   = GL_RGB8,
	kPixelFormatRGB5A1 = GL_RGB5_A1,
	kPixelFormatRGB5   = GL_RGB5,
	kPixelFormatDXT1   = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
	kPixelFormatDXT3   = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
	kPixelFormatDXT5   = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
};

enum PixelDataType {
	kPixelDataType8    = GL_UNSIGNED_BYTE,
	kPixelDataType1555 = GL_UNSIGNED_SHORT_1_5_5_5_REV,
	kPixelDataType565  = GL_UNSIGNED_SHORT_5_6_5
};

} // End of namespace Graphics

#endif // GRAPHICS_TYPES_H
