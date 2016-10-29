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

#include "src/common/scopedptr.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/graphics/graphics.h"

#include "src/graphics/images/decoder.h"
#include "src/graphics/images/util.h"
#include "src/graphics/images/s3tc.h"
#include "src/graphics/images/dumptga.h"

namespace Graphics {

ImageDecoder::MipMap::MipMap(const ImageDecoder *i) : width(0), height(0), size(0), image(i) {
}

ImageDecoder::MipMap::MipMap(const MipMap &mipMap, const ImageDecoder *i) :
	width(mipMap.width), height(mipMap.height), size(mipMap.size), image(i) {

	data.reset(new byte[size]);

	std::memcpy(data.get(), mipMap.data.get(), size);
}

ImageDecoder::MipMap::~MipMap() {
}

void ImageDecoder::MipMap::swap(MipMap &right) {
	SWAP(width , right.width );
	SWAP(height, right.height);
	SWAP(size  , right.size  );
	SWAP(image , right.image );

	data.swap(right.data);
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
	_format(kPixelFormatBGRA), _formatRaw(kPixelFormatRGBA8), _dataType(kPixelDataType8),
	_layerCount(1), _isCubeMap(false) {

}

ImageDecoder::ImageDecoder(const ImageDecoder &image) {
	*this = image;
}

ImageDecoder::~ImageDecoder() {
}

ImageDecoder &ImageDecoder::operator=(const ImageDecoder &image) {
	_compressed = image._compressed;
	_hasAlpha   = image._hasAlpha;

	_format    = image._format;
	_formatRaw = image._formatRaw;
	_dataType  = image._dataType;

	_layerCount = image._layerCount;
	_isCubeMap  = image._isCubeMap;

	_txi = image._txi;

	_mipMaps.clear();
	_mipMaps.reserve(image._mipMaps.size());

	for (MipMaps::const_iterator m = image._mipMaps.begin(); m != image._mipMaps.end(); ++m)
		_mipMaps.push_back(new MipMap(**m));

	return *this;
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
	assert((_mipMaps.size() % _layerCount) == 0);

	return _mipMaps.size() / _layerCount;
}

size_t ImageDecoder::getLayerCount() const {
	return _layerCount;
}

bool ImageDecoder::isCubeMap() const {
	assert(!_isCubeMap || (_layerCount == 6));

	return _isCubeMap;
}

const ImageDecoder::MipMap &ImageDecoder::getMipMap(size_t mipMap, size_t layer) const {
	assert(layer < _layerCount);
	assert((_mipMaps.size() % _layerCount) == 0);

	const size_t index = layer * getMipMapCount() + mipMap;

	assert(index < _mipMaps.size());

	return *_mipMaps[index];
}

void ImageDecoder::decompress(MipMap &out, const MipMap &in, PixelFormatRaw format) {
	if ((format != kPixelFormatDXT1) &&
	    (format != kPixelFormatDXT3) &&
	    (format != kPixelFormatDXT5))
		throw Common::Exception("Unknown compressed format %d", format);

	/* The DXT algorithms work on 4x4 pixel blocks. Textures smaller than one
	 * block will be padded, but larger textures need to be correctly aligned. */
	if (!hasValidDimensions(format, in.width, in.height))
		throw Common::Exception("Invalid dimensions (%dx%d) for format %d", in.width, in.height, format);

	out.width  = in.width;
	out.height = in.height;
	out.size   = out.width * out.height * 4;

	out.data.reset(new byte[out.size]);

	Common::ScopedPtr<Common::MemoryReadStream> stream(new Common::MemoryReadStream(in.data.get(), in.size));

	if      (format == kPixelFormatDXT1)
		decompressDXT1(out.data.get(), *stream, out.width, out.height, out.width * 4);
	else if (format == kPixelFormatDXT3)
		decompressDXT3(out.data.get(), *stream, out.width, out.height, out.width * 4);
	else if (format == kPixelFormatDXT5)
		decompressDXT5(out.data.get(), *stream, out.width, out.height, out.width * 4);
}

void ImageDecoder::decompress() {
	if (!_compressed)
		return;

	for (MipMaps::iterator m = _mipMaps.begin(); m != _mipMaps.end(); ++m) {
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

	ImageDecoder image(*this);
	image.decompress();

	Graphics::dumpTGA(fileName, &image);
	return true;
}

} // End of namespace Graphics
