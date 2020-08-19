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
 *  A texture as used in the Aurora engines.
 */

#include <cassert>

#include "src/common/types.h"
#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/graphics/aurora/texture.h"
#include "src/graphics/aurora/pltfile.h"

#include "src/graphics/types.h"
#include "src/graphics/graphics.h"
#include "src/graphics/images/txi.h"
#include "src/graphics/images/decoder.h"
#include "src/graphics/images/cubemapcombiner.h"
#include "src/graphics/images/tga.h"
#include "src/graphics/images/dds.h"
#include "src/graphics/images/tpc.h"
#include "src/graphics/images/txb.h"
#include "src/graphics/images/sbm.h"
#include "src/graphics/images/xoreositex.h"

#include "src/events/requests.h"

#include "src/aurora/resman.h"

using Events::RequestID;

namespace Graphics {

namespace Aurora {

Texture::Texture() : _type(::Aurora::kFileTypeNone), _width(0), _height(0), _deswizzle(false) {
}

Texture::Texture(const Common::UString &name, ImageDecoder *image,
                 ::Aurora::FileType type, TXI *txi, bool deswizzle) :
	_name(name), _type(type), _width(0), _height(0), _deswizzle(deswizzle) {

	set(name, image, type, txi, deswizzle);
	addToQueues();
}

Texture::~Texture() {
	removeFromQueues();

	if (_textureID != 0)
		GfxMan.abandon(&_textureID, 1);
}

uint32_t Texture::getWidth() const {
	return _width;
}

uint32_t Texture::getHeight() const {
	return _height;
}

bool Texture::hasAlpha() const {
	if (!_image)
		return false;

	return _image->hasAlpha();
}

bool Texture::isDynamic() const {
	return false;
}

static const TXI kEmptyTXI;
const TXI &Texture::getTXI() const {
	if (_txi)
		return *_txi;

	if (_image)
		return _image->getTXI();

	return kEmptyTXI;
}

const ImageDecoder &Texture::getImage() const {
	assert(_image);

	return *_image;
}

bool Texture::reload() {
	if (_name.empty())
		return false;

	::Aurora::FileType type = ::Aurora::kFileTypeNone;
	ImageDecoder *image = 0;
	TXI *txi = 0;

	try {

		txi   = loadTXI  (_name);
		image = loadImage(_name, type, txi, _deswizzle);

	} catch (Common::Exception &e) {
		delete txi;

		e.add("Failed to reload texture \"%s\" (%d)", _name.c_str(), type);
		throw;
	}

	removeFromQueues();
	set(_name, image, type, txi, _deswizzle);
	addToQueues();

	return true;
}

bool Texture::dumpTGA(const Common::UString &fileName) const {
	if (!_image)
		return false;

	return _image->dumpTGA(fileName);
}

void Texture::doDestroy() {
	if (_textureID == 0)
		return;

	glDeleteTextures(1, &_textureID);

	_textureID = 0;
}

void Texture::doRebuild() {
	if (!_image)
		// No image
		return;

	// Generate the texture ID
	if (_textureID == 0)
		glGenTextures(1, &_textureID);

	if (_image->isCubeMap()) {
		createCubeMapTexture();
		return;
	}

	create2DTexture();
}

void Texture::setWrap(GLenum target, GLint wrapModeX, GLint wrapModeY) {
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapModeX);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapModeY);
}

void Texture::setAlign() {
	/* Set the correct alignment depending on the texture data we have.
	   Can be used for optimized reading of texture data by the driver. */

	int alignment = 1;

	switch (_image->getFormatRaw()) {
		// 4 byte per texel, so always neatly 4-byte aligned
		case kPixelFormatRGBA8:
			alignment = 4;
			break;

		// 2 byte per texel, so always neatly 2-byte aligned
		case kPixelFormatRGB5A1:
		case kPixelFormatRGB5:
			alignment = 2;
			break;

		// All bets are off here
		case kPixelFormatRGB8:
			alignment = 1;
			break;

		// DXT texture rows should always be 4-byte aligned
		case kPixelFormatDXT1:
		case kPixelFormatDXT3:
		case kPixelFormatDXT5:
			alignment = 4;
			break;

		// Unknown
		default:
			alignment = 1;
			break;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
}

void Texture::setFilter(GLenum target) {
	const TXI::Features &features = getTXI().getFeatures();

	if (features.filter) {
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else {
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
}

void Texture::setMipMaps(GLenum target) {
	if (_image->getMipMapCount() == 1) {
		// Texture doesn't specify any mip maps, generate our own

		glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 9);
	} else {
		// Texture does specify mip maps, use these

		glTexParameteri(target, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, _image->getMipMapCount() - 1);
	}
}

void Texture::setMipMapData(GLenum target, size_t layer, size_t mipMap) {
	const ImageDecoder::MipMap &m = _image->getMipMap(mipMap, layer);

	if (_image->isCompressed()) {
		glCompressedTexImage2D(target, mipMap, _image->getFormatRaw(),
		                       m.width, m.height, 0, m.size, m.data.get());
	} else {
		glTexImage2D(target, mipMap, _image->getFormatRaw(),
		             m.width, m.height, 0, _image->getFormat(), _image->getDataType(), m.data.get());
	}
}

void Texture::create2DTexture() {
	// Bind the texture
	glBindTexture(GL_TEXTURE_2D, _textureID);

	// Edge wrap mode
	setWrap(GL_TEXTURE_2D, GL_REPEAT, GL_REPEAT);

	// Pixel row alignment
	setAlign();

	// Filter method
	setFilter(GL_TEXTURE_2D);

	// Mip map parameters
	setMipMaps(GL_TEXTURE_2D);

	// Texture image data
	for (size_t i = 0; i < _image->getMipMapCount(); i++)
		setMipMapData(GL_TEXTURE_2D, 0, i);
}

void Texture::createCubeMapTexture() {
	// Bind the texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);

	// Edge wrap mode
	setWrap(GL_TEXTURE_CUBE_MAP, GL_REPEAT, GL_REPEAT);

	// Pixel row alignment
	setAlign();

	// Filter method
	setFilter(GL_TEXTURE_CUBE_MAP);

	// Mip map parameters
	setMipMaps(GL_TEXTURE_CUBE_MAP);

	assert(_image->getLayerCount() == 6);

	static const GLenum faceTarget[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	// Texture image data
	for (size_t i = 0; i < _image->getLayerCount(); i++)
		for (size_t j = 0; j < _image->getMipMapCount(); j++)
			setMipMapData(faceTarget[i], i, j);
}

Texture *Texture::createPLT(const Common::UString &name, Common::SeekableReadStream *imageStream) {
	Texture *texture = 0;
	try {
		texture = new PLTFile(name, *imageStream);
	} catch (...) {
		delete imageStream;
		throw;
	}

	delete imageStream;
	return texture;
}

Texture *Texture::create(const Common::UString &name, bool deswizzle) {
	::Aurora::FileType type = ::Aurora::kFileTypeNone;
	ImageDecoder *image = 0;
	ImageDecoder *layers[6] = { 0, 0, 0, 0, 0, 0 };
	TXI *txi = 0;

	try {
		txi = loadTXI(name);

		const bool isFileCubeMap = txi && txi->getFeatures().cube && (txi->getFeatures().fileRange == 6);
		if (isFileCubeMap) {
			// A cube map with each side a separate image file

			for (size_t i = 0; i < 6; i++) {
				const Common::UString side = name + Common::composeString(i);
				Common::SeekableReadStream *imageStream = ResMan.getResource(::Aurora::kResourceImage, side, &type);
				if (!imageStream)
					throw Common::Exception("No such cube side image resource \"%s\"", side.c_str());

				layers[i] = loadImage(imageStream, type, txi, deswizzle);
			}

			image = new CubeMapCombiner(layers);

		} else {
			Common::SeekableReadStream *imageStream = ResMan.getResource(::Aurora::kResourceImage, name, &type);
			if (!imageStream)
				throw Common::Exception("No such image resource \"%s\"", name.c_str());

			// PLT needs extra handling, since they're their own Texture class
			if (type == ::Aurora::kFileTypePLT) {
				delete txi;

				return createPLT(name, imageStream);
			}

			image = loadImage(imageStream, type, txi, deswizzle);
		}

	} catch (Common::Exception &e) {
		delete txi;
		delete image;

		for (size_t i = 0; i < ARRAYSIZE(layers); i++)
			delete layers[i];

		e.add("Failed to create texture \"%s\" (%d)", name.c_str(), type);
		throw;
	}

	return new Texture(name, image, type, txi, deswizzle);
}

Texture *Texture::create(ImageDecoder *image, ::Aurora::FileType type, TXI *txi, bool deswizzle) {
	if (!image)
		throw Common::Exception("Can't create a texture from an empty image");

	if (image->getMipMapCount() < 1)
		throw Common::Exception("Texture has no images");

	return new Texture("", image, type, txi, deswizzle);
}

void Texture::set(const Common::UString &name, ImageDecoder *image, ::Aurora::FileType type,
                  TXI *txi, bool deswizzle) {

	_name = name;
	_type = type;

	_image.reset(image);
	_txi.reset(txi);

	_width  = _image->getMipMap(0).width;
	_height = _image->getMipMap(0).height;

	_deswizzle = deswizzle;
}

ImageDecoder *Texture::loadImage(const Common::UString &name, bool deswizzle) {
	::Aurora::FileType type;

	return loadImage(name, type, deswizzle);
}

void Texture::addToQueues() {
	addToQueue(kQueueTexture);
	addToQueue(kQueueNewTexture);
}

void Texture::removeFromQueues() {
	removeFromQueue(kQueueNewTexture);
	removeFromQueue(kQueueTexture);
}

void Texture::refresh() {
	removeFromQueues();
	addToQueues();
}

ImageDecoder *Texture::loadImage(const Common::UString &name, ::Aurora::FileType &type, bool deswizzle) {
	return loadImage(name, type, 0, deswizzle);
}

ImageDecoder *Texture::loadImage(const Common::UString &name, ::Aurora::FileType &type,
                                 TXI *txi, bool deswizzle) {

	const bool isFileCubeMap = txi && txi->getFeatures().cube && (txi->getFeatures().fileRange == 6);
	if (!isFileCubeMap) {
		Common::SeekableReadStream *imageStream = ResMan.getResource(::Aurora::kResourceImage, name, &type);
		if (!imageStream)
			throw Common::Exception("No such image resource \"%s\"", name.c_str());

		return loadImage(imageStream, type, txi, deswizzle);
	}

	ImageDecoder *layers[6] = { 0, 0, 0, 0, 0, 0 };

	try {
		for (size_t i = 0; i < 6; i++) {
			const Common::UString side = name + Common::composeString(i);
			Common::SeekableReadStream *imageStream = ResMan.getResource(::Aurora::kResourceImage, side, &type);
			if (!imageStream)
				throw Common::Exception("No such cube side image resource \"%s\"", side.c_str());

			layers[i] = loadImage(imageStream, type, txi, deswizzle);
		}

		return new CubeMapCombiner(layers);

	} catch (...) {
		for (size_t i = 0; i < ARRAYSIZE(layers); i++)
			delete layers[i];

		throw;
	}
}

ImageDecoder *Texture::loadImage(Common::SeekableReadStream *imageStream, ::Aurora::FileType type,
                                 TXI *txi, bool deswizzle) {

	// Check for a cube map, but only those that don't use a file for each side
	const bool isCubeMap = txi && txi->getFeatures().cube && (txi->getFeatures().fileRange == 0);

	ImageDecoder *image = 0;
	try {
		// Loading the different image formats
		if      (type == ::Aurora::kFileTypeTGA)
			image = new TGA(*imageStream, isCubeMap);
		else if (type == ::Aurora::kFileTypeDDS)
			image = new DDS(*imageStream);
		else if (type == ::Aurora::kFileTypeTPC)
			image = new TPC(*imageStream);
		else if (type == ::Aurora::kFileTypeTXB)
			image = new TXB(*imageStream);
		else if (type == ::Aurora::kFileTypeSBM)
			image = new SBM(*imageStream, deswizzle);
		else if (type == ::Aurora::kFileTypeXEOSITEX)
			image = new XEOSITEX(*imageStream);
		else
			throw Common::Exception("Unsupported image resource type %d", (int) type);

		if (image->getMipMapCount() < 1)
			throw Common::Exception("Texture has no images");

		// Decompress
		if (GfxMan.needManualDeS3TC())
			image->decompress();

	} catch (...) {
		delete image;
		delete imageStream;

		throw;
	}

	delete imageStream;
	return image;
}

TXI *Texture::loadTXI(const Common::UString &name) {
	Common::SeekableReadStream *txiStream = ResMan.getResource(name, ::Aurora::kFileTypeTXI);
	if (!txiStream)
		return 0;

	TXI *txi = 0;
	try {
		txi = new TXI(*txiStream);
	} catch (...) {
		Common::exceptionDispatcherWarning("Failed loading TXI \"%s\"", name.c_str());
	}

	delete txiStream;
	return txi;
}

} // End of namespace Aurora

} // End of namespace Graphics
