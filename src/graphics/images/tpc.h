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
 *  TPC (BioWare's own texture format) loading.
 */

#ifndef GRAPHICS_IMAGES_TPC_H
#define GRAPHICS_IMAGES_TPC_H

#include "src/graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** BioWare's own texture format, TPC.
 *
 *  This format is used by the two Knights of the Old Republic games.
 *  In the Xbox versions, these files have a .txb extension, but
 *  are still this format (not the TXB format used in Jade Empire).
 */
class TPC : public ImageDecoder {
public:
	TPC(Common::SeekableReadStream &tpc);
	~TPC();

private:
	bool _isAnimated;

	// Loading helpers
	void load(Common::SeekableReadStream &tpc);
	void readHeader(Common::SeekableReadStream &tpc, byte &encoding);
	void readData(Common::SeekableReadStream &tpc, byte encoding);
	void readTXI(Common::SeekableReadStream &tpc);

	uint32_t getMinDataSize(bool uncompressed, byte encoding);
	PixelFormatRaw getPixelFormat(bool uncompressed, byte encoding);

	bool checkCubeMap(uint32_t &width, uint32_t &height);
	bool checkAnimated(uint32_t &width, uint32_t &height, uint32_t &dataSize);
	void fixupCubeMap();

	static void deSwizzle(byte *dst, const byte *src, uint32_t width, uint32_t height);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TPC_H
