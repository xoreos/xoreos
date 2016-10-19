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
 *  BioWare's Packed Layered Texture.
 */

#ifndef GRAPHICS_AURORA_PLTFILE_H
#define GRAPHICS_AURORA_PLTFILE_H

#include "src/common/scopedptr.h"

#include "src/aurora/aurorafile.h"

#include "src/graphics/aurora/texture.h"

namespace Graphics {

class Surface;

namespace Aurora {

class PLTFile : public ::Aurora::AuroraFile, public Texture {
public:
	enum Layer {
		kLayerSkin     = 0,
		kLayerHair     = 1,
		kLayerMetal1   = 2,
		kLayerMetal2   = 3,
		kLayerCloth1   = 4,
		kLayerCloth2   = 5,
		kLayerLeather1 = 6,
		kLayerLeather2 = 7,
		kLayerTattoo1  = 8,
		kLayerTattoo2  = 9,
		kLayerMAX
	};

	~PLTFile();

	/** Set the color of one layer within this layer texture. */
	void setLayerColor(Layer layer, uint8 color);
	/** Rebuild the combined texture image. */
	void rebuild();

	bool isDynamic() const;
	bool reload();


private:
	Common::UString _name;

	Surface *_surface;

	Common::ScopedArray<uint8> _dataImage;
	Common::ScopedArray<uint8> _dataLayers;

	uint8 _colors[kLayerMAX];


	PLTFile(const Common::UString &name, Common::SeekableReadStream &plt);

	void load(Common::SeekableReadStream &plt);
	void build();

	static ImageDecoder *getLayerPalette(uint32 layer, uint8 row);
	static void getColorRows(byte rows[4 * 256 * kLayerMAX], const uint8 colors[kLayerMAX]);

	friend class Texture;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_PLTFILE_H
