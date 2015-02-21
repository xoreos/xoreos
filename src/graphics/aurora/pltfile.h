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

#include "src/common/ustring.h"

#include "src/aurora/aurorafile.h"

#include "src/graphics/images/decoder.h"

#include "src/graphics/aurora/textureman.h"

namespace Graphics {

namespace Aurora {

/** Packed Layered Texture. */
class PLTFile : public ::Aurora::AuroraBase {
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

	PLTFile(const Common::UString &fileName);
	~PLTFile();

	bool reload();

	void setLayerColor(Layer layer, uint8 color);
	void rebuild();

private:
	Common::UString _name;

	uint32 _width;
	uint32 _height;

	uint8 *_dataImage;
	uint8 *_dataLayers;

	uint8 _colors[kLayerMAX];

	TextureHandle _texture;


	TextureHandle getTexture() const;

	void load();
	void readHeader(Common::SeekableReadStream &plt);
	void readData(Common::SeekableReadStream &plt);

	void clear();


	friend class PLTImage;
	friend class TextureManager;
};

class PLTImage : public ImageDecoder {
public:
	~PLTImage();

private:
	PLTImage(const PLTFile &parent);

	void create(const PLTFile &parent);
	void getColorRows(const PLTFile &parent, byte *rows);

	friend class PLTFile;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_PLTFILE_H
