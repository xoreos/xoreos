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
 *  TXB (another one of BioWare's own texture formats) loading.
 */

#ifndef GRAPHICS_IMAGES_TXB_H
#define GRAPHICS_IMAGES_TXB_H

#include "src/graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** Another one of BioWare's own texture formats, TXB.
 *
 *  This format is used by Jade Empire.
 *
 *  Even though the Xbox versions of the Knights of the Old Republic games
 *  features textures with a .txb extension, these are actually in the TPC
 *  format, not this TXB format.
 */
class TXB : public ImageDecoder {
public:
	TXB(Common::SeekableReadStream &txb);
	~TXB();

private:
	// Loading helpers
	void load(Common::SeekableReadStream &txb);
	void readHeader(Common::SeekableReadStream &txb, byte &encoding, uint32_t &dataSize);
	void readData(Common::SeekableReadStream &txb, byte encoding);
	void readTXI(Common::SeekableReadStream &txb);

	static void deSwizzle(byte *dst, const byte *src, uint32_t width, uint32_t height, uint8_t bpp);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TXB_H
