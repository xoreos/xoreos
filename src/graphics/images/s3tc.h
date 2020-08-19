/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Manual S3TC DXTn decompression methods.
 */

#ifndef GRAPHICS_IMAGES_S3TC_H
#define GRAPHICS_IMAGES_S3TC_H

#include "src/common/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

void decompressDXT1(byte *dest, Common::SeekableReadStream &src, uint32_t width, uint32_t height, uint32_t pitch);
void decompressDXT3(byte *dest, Common::SeekableReadStream &src, uint32_t width, uint32_t height, uint32_t pitch);
void decompressDXT5(byte *dest, Common::SeekableReadStream &src, uint32_t width, uint32_t height, uint32_t pitch);

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_S3TC_H
