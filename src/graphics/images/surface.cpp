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
 *  An image surface, in BGRA format.
 */

#include <cassert>
#include <cstring>

#include "src/common/maths.h"

#include "src/graphics/images/surface.h"

namespace Graphics {

Surface::Surface(int width, int height) {
	assert((width > 0) && (height > 0));

	_compressed = false;
	_hasAlpha   = true;
	_format     = kPixelFormatBGRA;
	_formatRaw  = kPixelFormatRGBA8;
	_dataType   = kPixelDataType8;

	_mipMaps.push_back(new MipMap(this));

	_mipMaps[0]->width  = width;
	_mipMaps[0]->height = height;
	_mipMaps[0]->size   = _mipMaps[0]->width * _mipMaps[0]->height * 4;

	_mipMaps[0]->data = std::make_unique<byte[]>(_mipMaps[0]->size);
}

Surface::~Surface() {
}

int Surface::getWidth() const {
	return _mipMaps[0]->width;
}

int Surface::getHeight() const {
	return _mipMaps[0]->height;
}

byte *Surface::getData() {
	return _mipMaps[0]->data.get();
}

const byte *Surface::getData() const {
	return _mipMaps[0]->data.get();
}

void Surface::fill(byte r, byte g, byte b, byte a) {
	if ((r == g) && (r == b) && (r == a)) {
		std::memset(_mipMaps[0]->data.get(), r, _mipMaps[0]->size);
		return;
	}

	byte  *data = _mipMaps[0]->data.get();
	uint32_t size = _mipMaps[0]->size / 4;
	while (size-- > 0) {
		*data++ = b;
		*data++ = g;
		*data++ = r;
		*data++ = a;
	}
}

void Surface::resize(unsigned int newWidth, unsigned int newHeight) {
	assert((newWidth > 0) && (newHeight > 0));

	unsigned int oldWidth = _mipMaps[0]->width;
	unsigned int oldHeight = _mipMaps[0]->height;

	std::unique_ptr<byte[]> oldData(_mipMaps[0]->data.release());

	_mipMaps[0]->width = newWidth;
	_mipMaps[0]->height = newHeight;
	_mipMaps[0]->size   = _mipMaps[0]->width * _mipMaps[0]->height * 4;

	_mipMaps[0]->data = std::make_unique<byte[]>(_mipMaps[0]->size);

	double xRatio = static_cast<double>(oldWidth) / static_cast<double>(newWidth);
	double yRatio = static_cast<double>(oldHeight) / static_cast<double>(newHeight);

	for (unsigned int y = 0; y < newHeight; ++y) {
		for (unsigned int x = 0; x < newWidth; ++x) {
			double px = std::floor(static_cast<double>(x) * xRatio);
			double py = std::floor(static_cast<double>(y) * yRatio);

			_mipMaps[0]->data[(y * newWidth + x) * 4] = oldData[(py * oldWidth + px) * 4];
			_mipMaps[0]->data[(y * newWidth + x) * 4 + 1] = oldData[(py * oldWidth + px) * 4 + 1];
			_mipMaps[0]->data[(y * newWidth + x) * 4 + 2] = oldData[(py * oldWidth + px) * 4 + 2];
			_mipMaps[0]->data[(y * newWidth + x) * 4 + 3] = oldData[(py * oldWidth + px) * 4 + 3];
		}
	}
}

const ImageDecoder::MipMap &Surface::getMipMap(size_t mipMap) const {
	return ImageDecoder::getMipMap(mipMap);
}

const ImageDecoder::MipMap &Surface::getMipMap() const {
	return *_mipMaps[0];
}

ImageDecoder::MipMap &Surface::getMipMap() {
	return *_mipMaps[0];
}

} // End of namespace Graphics
