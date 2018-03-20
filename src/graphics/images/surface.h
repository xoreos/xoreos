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
 *  An image surface, in BGRA format.
 */

#ifndef GRAPHICS_IMAGES_SURFACE_H
#define GRAPHICS_IMAGES_SURFACE_H

#include "src/graphics/images/decoder.h"

namespace Graphics {

class Surface : public ImageDecoder {
public:
	Surface(int width, int height);
	~Surface();

	int getWidth () const;
	int getHeight() const;
	int getPitch() const { return getWidth() * 4; }

	byte *getData();
	const byte *getData() const;

	void fill(byte r, byte g, byte b, byte a);

	/** Return a mip map. */
	const MipMap &getMipMap(size_t mipMap) const;
	/** Return a mip map. */
	const MipMap &getMipMap() const;
	/** Return a mip map. */
	MipMap &getMipMap();
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_SURFACE_H
