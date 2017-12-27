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
 *  Decoding SBM (font bitmap data).
 */

#ifndef GRAPHICS_IMAGES_SBM_H
#define GRAPHICS_IMAGES_SBM_H

#include "src/graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** SBM font bitmap data.
 *
 *  This format is used for fonts in Jade Empire.
 */
class SBM : public ImageDecoder {
public:
	SBM(Common::SeekableReadStream &sbm);
	~SBM();

private:
	// Loading helpers
	void load(Common::SeekableReadStream &sbm);
	void readData(Common::SeekableReadStream &sbm);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_SBM_H
