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

#include <OgrePixelFormat.h>

namespace Graphics {

/** Hinting at a materials transparency. */
enum TransparencyHint {
	kTransparencyHintOpaque      = 0, ///< This material is opaque.
	kTransparencyHintTransparent = 1, ///< This material is transparent.
	kTransparencyHintUnknown     = 2  ///< Unknown whether the material is transparent.
};

enum PixelFormat {
	kPixelFormatNone = Ogre::PF_UNKNOWN,

	kPixelFormatR8G8B8   = Ogre::PF_BYTE_RGB,
	kPixelFormatB8G8R8   = Ogre::PF_BYTE_BGR,
	kPixelFormatR8G8B8A8 = Ogre::PF_BYTE_RGBA,
	kPixelFormatB8G8R8A8 = Ogre::PF_BYTE_BGRA,

	kPixelFormatA1R5G5B5 = Ogre::PF_A1R5G5B5,
	kPixelFormatR5G6B5   = Ogre::PF_R5G6B5,

	kPixelFormatDXT1 = Ogre::PF_DXT1,
	kPixelFormatDXT3 = Ogre::PF_DXT3,
	kPixelFormatDXT5 = Ogre::PF_DXT5
};

} // End of namespace Graphics

#endif // GRAPHICS_TYPES_H
