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
 *  Our very own intermediate texture format.
 *  Currently used by NSBTX.
 */

#ifndef GRAPHICS_IMAGES_XOREOSITEX_H
#define GRAPHICS_IMAGES_XOREOSITEX_H

#include "src/graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

class XEOSITEX : public ImageDecoder {
public:
	XEOSITEX(Common::SeekableReadStream &xeositex);
	~XEOSITEX();

private:
	bool _wrapX;
	bool _wrapY;
	bool _flipX;
	bool _flipY;

	uint8_t _coordTransform;

	void load(Common::SeekableReadStream &xeositex);
	void readHeader(Common::SeekableReadStream &xeositex);
	void readMipMaps(Common::SeekableReadStream &xeositex);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_XOREOSITEX_H
