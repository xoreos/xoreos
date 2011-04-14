/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/pltfile.h
 *  BioWare's Packed Layered Texture.
 */

#ifndef GRAPHICS_AURORA_PLTFILE_H
#define GRAPHICS_AURORA_PLTFILE_H

#include "common/ustring.h"

#include "aurora/aurorafile.h"

#include "graphics/images/decoder.h"

#include "graphics/aurora/textureman.h"

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
