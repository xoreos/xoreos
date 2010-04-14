/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/s3tc.h
 *  Manual S3TC DXTn decompression methods.
 */

#ifndef GRAPHICS_IMAGES_S3TC_H
#define GRAPHICS_IMAGES_S3TC_H

#include "common/types.h"

namespace Graphics {

void decompressDXT1(byte *dest, const byte *src, uint32 size);
void decompressDXT3(byte *dest, const byte *src, uint32 size);
void decompressDXT5(byte *dest, const byte *src, uint32 size);

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_S3TC_H
