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

#include <cstring>

#include "src/common/util.h"
#include "src/common/readstream.h"
#include "src/common/error.h"

#include "src/graphics/images/tga.h"
#include "src/graphics/images/util.h"

namespace Graphics {

TGA::TGA(Common::SeekableReadStream &tga, bool cubeMap) {
	if (cubeMap) {
		_layerCount = 6;
		_isCubeMap  = true;
	}

	_compressed = false;

	load(tga);
}

TGA::~TGA() {
}

void TGA::load(Common::SeekableReadStream &tga) {
	try {

		ImageType imageType;
		byte pixelDepth, imageDesc;
		readHeader(tga, imageType, pixelDepth, imageDesc);
		readData  (tga, imageType, pixelDepth, imageDesc);

	} catch (Common::Exception &e) {
		e.add("Failed reading TGA file");
		throw;
	}
}

void TGA::readHeader(Common::SeekableReadStream &tga, ImageType &imageType, byte &pixelDepth, byte &imageDesc) {
	tga.seek(0);

	// TGAs have an optional "id" string in the header
	uint32_t idLength = tga.readByte();

	// Number of colors in the color map / palette
	if (tga.readByte() != 0)
		throw Common::Exception("Unsupported feature: Color map");

	// Image type. 2 == unmapped RGB, 3 == Grayscale
	imageType = (ImageType)tga.readByte();
	if (!isSupportedImageType(imageType))
		throw Common::Exception("Unsupported image type: %d", imageType);

	if ((imageType == kImageTypeRLECMap)      ||
	    (imageType == kImageTypeRLETrueColor) ||
	    (imageType == kImageTypeRLEBW)) {

		/* Multi-layer images (e.g. cube maps) are split into multiple images.
		 * RLE compression adds a complexity there, since the split might be
		 * in the middle of a run. We need a file to test this on first.
		 */

		if (_layerCount > 1)
			throw Common::Exception("TODO: RLE-compressed multi-layer TGA");
	}

	// Color map specifications + X + Y
	tga.skip(5 + 2 + 2);

	int32_t width  = tga.readUint16LE();
	int32_t height = tga.readUint16LE();

	if ((width >= 0x8000) || (height >= 0x8000))
		throw Common::Exception("Unsupported image dimensions (%dx%d)", width, height);

	if ((height % _layerCount) != 0)
		throw Common::Exception("TGA with %u layers but size of %dx%d", (uint) _layerCount, width, height);

	height /= _layerCount;

	_mipMaps.resize(_layerCount, 0);
	for (size_t i = 0; i < _layerCount; i++) {
		_mipMaps[i] = new MipMap(this);

		_mipMaps[i]->width  = width;
		_mipMaps[i]->height = height;
	}

	// Bits per pixel
	pixelDepth = tga.readByte();

	if (imageType == kImageTypeTrueColor || imageType == kImageTypeRLETrueColor) {
		if (pixelDepth == 24) {
			_hasAlpha  = false;
			_format    = kPixelFormatBGR;
			_formatRaw = kPixelFormatRGB8;
			_dataType  = kPixelDataType8;
		} else if (pixelDepth == 16 || pixelDepth == 32) {
			_hasAlpha  = true;
			_format    = kPixelFormatBGRA;
			_formatRaw = kPixelFormatRGBA8;
			_dataType  = kPixelDataType8;
		} else if (pixelDepth == 8) {
			imageType = kImageTypeBW;

			_hasAlpha  = false;
			_format    = kPixelFormatBGRA;
			_formatRaw = kPixelFormatRGBA8;
			_dataType  = kPixelDataType8;
		} else
			throw Common::Exception("Unsupported pixel depth: %d, %d", imageType, pixelDepth);
	} else if (imageType == kImageTypeBW) {
		if (pixelDepth != 8)
			throw Common::Exception("Unsupported pixel depth: %d, %d", imageType, pixelDepth);

		_hasAlpha  = false;
		_format    = kPixelFormatBGRA;
		_formatRaw = kPixelFormatRGBA8;
		_dataType  = kPixelDataType8;
	}

	// Image descriptor
	imageDesc = tga.readByte();

	// Skip the id string
	tga.skip(idLength);
}

void TGA::readData(Common::SeekableReadStream &tga, ImageType imageType, byte pixelDepth, byte imageDesc) {
	for (size_t i = 0; i < _layerCount; i++) {
		if (imageType == kImageTypeTrueColor || imageType == kImageTypeRLETrueColor) {
			_mipMaps[i]->size = _mipMaps[i]->width * _mipMaps[i]->height;
			if      (_format == kPixelFormatBGR)
				_mipMaps[i]->size *= 3;
			else if (_format == kPixelFormatBGRA)
				_mipMaps[i]->size *= 4;

			_mipMaps[i]->data = std::make_unique<byte[]>(_mipMaps[i]->size);

			if (imageType == kImageTypeTrueColor) {
				if (pixelDepth == 16) {
					// Convert from 16bpp to 32bpp.
					// 16bpp TGA is usually ARGB1555, but Sonic's are AGBR1555.
					// Hopefully Sonic is the only game that needs 16bpp TGAs.

					uint16_t count = _mipMaps[i]->width * _mipMaps[i]->height;
					byte *dst = _mipMaps[i]->data.get();

					while (count--) {
						uint16_t pixel = tga.readUint16LE();

						*dst++ = (pixel & 0x7C00) >> 7;
						*dst++ = (pixel & 0x03E0) >> 2;
						*dst++ = (pixel & 0x001F) << 3;
						*dst++ = (pixel & 0x8000) ? 0xFF : 0x00;
					}
				} else {
					// Read it in raw
					tga.read(_mipMaps[i]->data.get(), _mipMaps[i]->size);
				}
			} else {
				readRLE(tga, pixelDepth, i);
			}
		} else if (imageType == kImageTypeBW) {
			_mipMaps[i]->size = _mipMaps[i]->width * _mipMaps[i]->height * 4;
			_mipMaps[i]->data = std::make_unique<byte[]>(_mipMaps[i]->size);

			byte  *data  = _mipMaps[i]->data.get();
			uint32_t count = _mipMaps[i]->width * _mipMaps[i]->height;

			while (count-- > 0) {
				byte g = tga.readByte();

				std::memset(data, g, 3);
				data[3] = 0xFF;

				data += 4;
			}

		}

		// Bit 5 of imageDesc set means the origin in upper-left corner
		if (imageDesc & 0x20)
			flipVertically(_mipMaps[i]->data.get(), _mipMaps[i]->width, _mipMaps[i]->height, pixelDepth / 8);
	}
}

void TGA::readRLE(Common::SeekableReadStream &tga, byte pixelDepth, size_t layer) {
	if (pixelDepth != 24 && pixelDepth != 32)
		throw Common::Exception("Unhandled RLE depth %d", pixelDepth);

	byte  *data  = _mipMaps[layer]->data.get();
	uint32_t count = _mipMaps[layer]->width * _mipMaps[layer]->height;

	while (count > 0) {
		byte code = tga.readByte();
		byte length = MIN<uint32_t>((code & 0x7F) + 1, count);

		count -= length;

		if (code & 0x80) {
			if (pixelDepth == 32) {
				uint32_t color = tga.readUint32BE();

				while (length--) {
					WRITE_BE_UINT32(data, color);
					data += 4;
				}
			} else if (pixelDepth == 24) {
				byte b = tga.readByte();
				byte g = tga.readByte();
				byte r = tga.readByte();

				while (length--) {
					*data++ = b;
					*data++ = g;
					*data++ = r;
				}
			}
		} else {
			if (pixelDepth == 32) {
				while (length--) {
					WRITE_BE_UINT32(data, tga.readUint32BE());
					data += 4;
				}
			} else if (pixelDepth == 24) {
				while (length--) {
					*data++ = tga.readByte();
					*data++ = tga.readByte();
					*data++ = tga.readByte();
				}
			}
		}
	}
}

bool TGA::isSupportedImageType(ImageType type) const {
	// We currently only support a limited number of types
	switch (type) {
	case kImageTypeTrueColor:
	case kImageTypeBW:
	case kImageTypeRLETrueColor:
		return true;
	default:
		break;
	}

	return false;
}

} // End of namespace Graphics
