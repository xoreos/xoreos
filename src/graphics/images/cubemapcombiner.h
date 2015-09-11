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
 *  A class creating a cube map by combining six images.
 */

#ifndef GRAPHICS_IMAGES_CUBEMAPCOMBINER_H
#define GRAPHICS_IMAGES_CUBEMAPCOMBINER_H

#include "src/graphics/images/decoder.h"

namespace Graphics {

class CubeMapCombiner : public ImageDecoder {
public:
	/** Take over this six images and combine them into a single cube map. */
	CubeMapCombiner(ImageDecoder *(&sides)[6]);
	~CubeMapCombiner();
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_CUBEMAPCOMBINER_H
