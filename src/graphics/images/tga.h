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
 *  Decoding TGA (TarGa) images.
 */

/* Based on ScummVM (<http://scummvm.org>) code, which is released
 * under the terms of version 2 or later of the GNU General Public
 * License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
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
 */

#ifndef GRAPHICS_IMAGES_TGA_H
#define GRAPHICS_IMAGES_TGA_H

#include "src/graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** TarGa image.
 *
 *  This format is used in the two Neverwinter Nights games, the two
 *  Knights of the Old Republic games, Jade Empire and Sonic.
 */
class TGA : public ImageDecoder {
public:
	TGA(Common::SeekableReadStream &tga, bool cubeMap = false);
	~TGA();

private:
	// Format-spec from http://www.ludorg.net/amnesia/TGA_File_Format_Spec.html
	enum ImageType {
		kImageTypeCMap         =  1,
		kImageTypeTrueColor    =  2,
		kImageTypeBW           =  3,
		kImageTypeRLECMap      =  9,
		kImageTypeRLETrueColor = 10,
		kImageTypeRLEBW        = 11
	};

	// Loading helpers
	void load(Common::SeekableReadStream &tga);
	void readHeader(Common::SeekableReadStream &tga, ImageType &imageType, byte &pixelDepth, byte &imageDesc);
	void readData(Common::SeekableReadStream &tga, ImageType imageType, byte pixelDepth, byte imageDesc);
	void readRLE(Common::SeekableReadStream &tga, byte pixelDepth, size_t layer);

	bool isSupportedImageType(ImageType type) const;
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TGA_H
