/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
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
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/aurora/pltfile.cpp
 *  BioWare's Packed Layered Texture.
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/resman.h"

#include "graphics/images/tga.h"

#include "graphics/aurora/pltfile.h"
#include "graphics/aurora/texture.h"

static const uint32 kPLTID     = MKTAG('P', 'L', 'T', ' ');
static const uint32 kVersion1  = MKTAG('V', '1', ' ', ' ');

namespace Graphics {

namespace Aurora {

static const char *kPalettes[PLTFile::kLayerMAX] = {
	"pal_skin01",
	"pal_hair01",
	"pal_armor01",
	"pal_armor02",
	"pal_cloth01",
	"pal_cloth01",
	"pal_leath01",
	"pal_leath01",
	"pal_tattoo01",
	"pal_tattoo01"
};

PLTFile::PLTFile(const Common::UString &fileName) : _name(fileName),
	_dataImage(0), _dataLayers(0) {

	assert(!_name.empty());

	for (uint i = 0; i < kLayerMAX; i++)
		_colors[i] = 0;

	load();
}

PLTFile::~PLTFile() {
	delete[] _dataImage;
	delete[] _dataLayers;
}

bool PLTFile::reload() {
	delete _dataImage;
	delete _dataLayers;

	_dataImage  = 0;
	_dataLayers = 0;

	load();
	rebuild();

	return true;
}

void PLTFile::load() {
	Common::SeekableReadStream *plt = 0;

	try {

		plt = ResMan.getResource(_name, ::Aurora::kFileTypePLT);
		if (!plt)
			throw Common::Exception("No such PLT");

		readHeader(*plt);
		readData  (*plt);

		if (plt->err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		delete plt;

		e.add("Failed reading PLT file \"%s\"", _name.c_str());
		throw;
	}

	delete plt;

	if (_texture.empty())
		_texture = TextureMan.add(new Texture(0));
}

void PLTFile::readHeader(Common::SeekableReadStream &plt) {
	AuroraBase::readHeader(plt);

	if (_id != kPLTID)
		throw Common::Exception("Not a PLT file");
	if (_version != kVersion1)
		throw Common::Exception("Unsupported PLT file version %08X", _version);

	uint32 layers = plt.readUint32LE();
	if (layers > kLayerMAX)
		throw Common::Exception("Too many layers (%d)", layers);

	plt.skip(4); // Unknown

	_width  = plt.readUint32LE();
	_height = plt.readUint32LE();

	if ((plt.size() - plt.pos()) < (int32) (2 * _width * _height))
		throw Common::Exception("Not enough data");
}

void PLTFile::readData(Common::SeekableReadStream &plt) {
	uint32 size = _width * _height;

	_dataImage  = new uint8[size];
	_dataLayers = new uint8[size];

	uint8 *image = _dataImage;
	uint8 *layer = _dataLayers;
	while (size-- > 0) {
		*image++ = plt.readByte();
		*layer++ = MIN<uint8>(plt.readByte(), kLayerMAX - 1);
	}
}

void PLTFile::setLayerColor(Layer layer, uint8 color) {
	assert((layer >= 0) && (layer < kLayerMAX));

	_colors[layer] = color;
}

void PLTFile::rebuild() {
	if (_texture.empty())
		return;

	PLTImage *t = new PLTImage(*this);

	_texture.getTexture().reload(t);
}

TextureHandle PLTFile::getTexture() const {
	return _texture;
}


PLTImage::PLTImage(const PLTFile &parent) {
	_compressed = false;
	_hasAlpha   = true;

	_format    = kPixelFormatBGRA;
	_formatRaw = kPixelFormatRGBA8;
	_dataType  = kPixelDataType8;

	create(parent);
}

PLTImage::~PLTImage() {
}

void PLTImage::create(const PLTFile &parent) {
	_mipMaps.push_back(new MipMap);

	_mipMaps[0]->width  = parent._width;
	_mipMaps[0]->height = parent._height;
	_mipMaps[0]->size   = _mipMaps[0]->width * _mipMaps[0]->height * 4;
	_mipMaps[0]->data   = new byte[_mipMaps[0]->size];

	byte rows[4 * 256 * PLTFile::kLayerMAX];
	getColorRows(parent, rows);

	uint32 pixels = parent._width * parent._height;
	const byte *image = parent._dataImage;
	const byte *layer = parent._dataLayers;
	      byte *dst   = _mipMaps[0]->data;

	for (uint32 i = 0; i < pixels; i++, image++, layer++, dst += 4)
		memcpy(dst, rows + (*layer * 4 * 256) + (*image * 4), 4);
}

void PLTImage::getColorRows(const PLTFile &parent, byte *rows) {
	for (uint i = 0; i < PLTFile::kLayerMAX; i++, rows += 4 * 256) {
		Common::SeekableReadStream *tgaFile = 0;

		try {
			tgaFile = ResMan.getResource(kPalettes[i], ::Aurora::kFileTypeTGA);
			if (!tgaFile)
				throw std::exception();

			TGA tga(*tgaFile);
			if (tga.getFormat() != kPixelFormatBGRA)
				throw std::exception();

			const MipMap &mipMap = tga.getMipMap(0);
			if (mipMap.width != 256)
				throw std::exception();

			uint8 row = parent._colors[i];
			if (row >= mipMap.height)
				throw std::exception();

			row = mipMap.height - 1 - row;

			memcpy(rows, mipMap.data + (row * 4 * 256), 4 * 256);

		} catch (...) {
			memset(rows, 0, 4 * 256);
		}

		delete tgaFile;
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
