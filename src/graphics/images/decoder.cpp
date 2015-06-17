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
 *  Generic image decoder interface.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/graphics/graphics.h"

#include "src/graphics/images/decoder.h"
#include "src/graphics/images/s3tc.h"
#include "src/graphics/images/dumptga.h"

namespace Graphics {

ImageDecoder::MipMap::MipMap(const ImageDecoder *i) : width(0), height(0), size(0), data(0), image(i) {
}

ImageDecoder::MipMap::~MipMap() {
	delete[] data;
}

void ImageDecoder::MipMap::swap(MipMap &right) {
	SWAP(width , right.width );
	SWAP(height, right.height);
	SWAP(size  , right.size  );
	SWAP(data  , right.data  );
	SWAP(image , right.image );
}

void ImageDecoder::MipMap::getPixel(int x, int y, float &r, float &g, float &b, float &a) const {
	getPixel(y * width + x, r, g, b, a);
}

void ImageDecoder::MipMap::getPixel(int n, float &r, float &g, float &b, float &a) const {
	assert(n < (int)size);
	assert(image);

	if        (image->getFormat() == kPixelFormatRGB) {
		r = data[n * 3 + 0] / 255.0f;
		g = data[n * 3 + 1] / 255.0f;
		b = data[n * 3 + 2] / 255.0f;
		a = 1.0f;
	} else if (image->getFormat() == kPixelFormatBGR) {
		r = data[n * 3 + 2] / 255.0f;
		g = data[n * 3 + 1] / 255.0f;
		b = data[n * 3 + 0] / 255.0f;
		a = 1.0f;
	} else if (image->getFormat() == kPixelFormatRGBA) {
		r = data[n * 4 + 0] / 255.0f;
		g = data[n * 4 + 1] / 255.0f;
		b = data[n * 4 + 2] / 255.0f;
		a = data[n * 4 + 3] / 255.0f;
	} else if (image->getFormat() == kPixelFormatBGRA) {
		r = data[n * 4 + 2] / 255.0f;
		g = data[n * 4 + 1] / 255.0f;
		b = data[n * 4 + 0] / 255.0f;
		a = data[n * 4 + 3] / 255.0f;
	} else
		throw Common::Exception("ImageDecoder::MipMap::getPixel(): Unsupported pixel format %d",
		                        image->getFormat());
}

void ImageDecoder::MipMap::setPixel(int x, int y, float r, float g, float b, float a) {
	getPixel(y * width + x, r, g, b, a);
}

void ImageDecoder::MipMap::setPixel(int n, float r, float g, float b, float a) {
	assert(n < (int)size);
	assert(image);

	if        (image->getFormat() == kPixelFormatRGB) {
		data[n * 3 + 0] = CLIP(r, 0.0f, 1.0f) * 255.0f;
		data[n * 3 + 1] = CLIP(g, 0.0f, 1.0f) * 255.0f;
		data[n * 3 + 2] = CLIP(b, 0.0f, 1.0f) * 255.0f;
	} else if (image->getFormat() == kPixelFormatBGR) {
		data[n * 3 + 2] = CLIP(r, 0.0f, 1.0f) * 255.0f;
		data[n * 3 + 1] = CLIP(g, 0.0f, 1.0f) * 255.0f;
		data[n * 3 + 0] = CLIP(b, 0.0f, 1.0f) * 255.0f;
	} else if (image->getFormat() == kPixelFormatRGBA) {
		data[n * 4 + 0] = CLIP(r, 0.0f, 1.0f) * 255.0f;
		data[n * 4 + 1] = CLIP(g, 0.0f, 1.0f) * 255.0f;
		data[n * 4 + 2] = CLIP(b, 0.0f, 1.0f) * 255.0f;
		data[n * 4 + 3] = CLIP(a, 0.0f, 1.0f) * 255.0f;
	} else if (image->getFormat() == kPixelFormatBGRA) {
		data[n * 4 + 2] = CLIP(r, 0.0f, 1.0f) * 255.0f;
		data[n * 4 + 1] = CLIP(g, 0.0f, 1.0f) * 255.0f;
		data[n * 4 + 0] = CLIP(b, 0.0f, 1.0f) * 255.0f;
		data[n * 4 + 3] = CLIP(a, 0.0f, 1.0f) * 255.0f;
	} else
		throw Common::Exception("ImageDecoder::MipMap::setPixel(): Unsupported pixel format %d",
		                        image->getFormat());
}


ImageDecoder::ImageDecoder() : _compressed(false), _hasAlpha(false),
	_format(kPixelFormatBGRA), _formatRaw(kPixelFormatRGBA8), _dataType(kPixelDataType8) {

}

ImageDecoder::~ImageDecoder() {
	clear();
}

void ImageDecoder::clear() {
	for (std::vector<MipMap *>::iterator m = _mipMaps.begin(); m != _mipMaps.end(); ++m)
		delete *m;

	_mipMaps.clear();
}

const TXI &ImageDecoder::getTXI() const {
	return _txi;
}

bool ImageDecoder::isCompressed() const {
	return _compressed;
}

bool ImageDecoder::hasAlpha() const {
	return _hasAlpha;
}

PixelFormat ImageDecoder::getFormat() const {
	return _format;
}

PixelFormatRaw ImageDecoder::getFormatRaw() const {
	return _formatRaw;
}

PixelDataType ImageDecoder::getDataType() const {
	return _dataType;
}

size_t ImageDecoder::getMipMapCount() const {
	return _mipMaps.size();
}

const ImageDecoder::MipMap &ImageDecoder::getMipMap(size_t mipMap) const {
	assert(mipMap < _mipMaps.size());

	return *_mipMaps[mipMap];
}

void ImageDecoder::decompress(MipMap &out, const MipMap &in, PixelFormatRaw format) {
	if ((format != kPixelFormatDXT1) &&
	    (format != kPixelFormatDXT3) &&
	    (format != kPixelFormatDXT5))
		throw Common::Exception("Unknown compressed format %d", format);

	out.width  = in.width;
	out.height = in.height;
	out.size   = out.width * out.height * 4;
	out.data   = new byte[out.size];

	Common::MemoryReadStream *stream = new Common::MemoryReadStream(in.data, in.size);

	if      (format == kPixelFormatDXT1)
		decompressDXT1(out.data, *stream, out.width, out.height, out.width * 4);
	else if (format == kPixelFormatDXT3)
		decompressDXT3(out.data, *stream, out.width, out.height, out.width * 4);
	else if (format == kPixelFormatDXT5)
		decompressDXT5(out.data, *stream, out.width, out.height, out.width * 4);

	delete stream;
}

void ImageDecoder::decompress() {
	if (!_compressed)
		return;

	for (std::vector<MipMap *>::iterator m = _mipMaps.begin(); m != _mipMaps.end(); ++m) {
		MipMap decompressed(this);

		decompress(decompressed, **m, _formatRaw);

		decompressed.swap(**m);
	}

	_format     = kPixelFormatRGBA;
	_formatRaw  = kPixelFormatRGBA8;
	_dataType   = kPixelDataType8;
	_compressed = false;
}

bool ImageDecoder::dumpTGA(const Common::UString &fileName) const {
	if (_mipMaps.size() < 1)
		return false;

	if (!_compressed) {
		Graphics::dumpTGA(fileName, this);
		return true;
	}

	MipMap mipMap(this);
	decompress(mipMap, *_mipMaps[0], _formatRaw);
	Graphics::dumpTGA(fileName, mipMap.data, mipMap.width, mipMap.height, kPixelFormatRGBA);

	return true;
}

} // End of namespace Graphics
