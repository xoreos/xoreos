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
 *  Nitro Basic File Screen, a simple raw Nintendo DS image.
 */

#ifndef GRAPHICS_IMAGES_NBFS_H
#define GRAPHICS_IMAGES_NBFS_H

#include "src/graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

class NBFS : public ImageDecoder {
public:
	/** NBFS are raw paletted images and need a palette, width and height. */
	NBFS(Common::SeekableReadStream &nbfs, Common::SeekableReadStream &nbfp, uint32_t width, uint32_t height);
	~NBFS();

private:
	void load(Common::SeekableReadStream &nbfs, Common::SeekableReadStream &nbfp, uint32_t width, uint32_t height);

	const byte *readPalette(Common::SeekableReadStream &nbfp);
	void readImage(Common::SeekableReadStream &nbfs, const byte *palette, uint32_t width, uint32_t height);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_NBFS_H
