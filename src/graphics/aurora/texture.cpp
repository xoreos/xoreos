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

#include "common/types.h"
#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "graphics/aurora/texture.h"

#include "graphics/types.h"
#include "graphics/graphics.h"
#include "graphics/images/txi.h"
#include "graphics/images/decoder.h"
#include "graphics/images/tga.h"
#include "graphics/images/dds.h"
#include "graphics/images/tpc.h"
#include "graphics/images/txb.h"
#include "graphics/images/sbm.h"

#include "events/requests.h"

#include "aurora/resman.h"

using Events::RequestID;

namespace Graphics {

namespace Aurora {

Texture::Texture(const Common::UString &name) : _textureID(0),
	_type(::Aurora::kFileTypeNone), _image(0), _txi(0), _width(0), _height(0) {

	_txi = new TXI();

	try {
		load(name);
	} catch (...) {
		delete _txi;
		delete _image;
		throw;
	}

	addToQueue(kQueueTexture);
	addToQueue(kQueueNewTexture);
}

Texture::Texture(ImageDecoder *image, const TXI *txi) : _textureID(0),
	_type(::Aurora::kFileTypeNone), _image(0), _txi(0), _width(0), _height(0) {

	if (txi)
		_txi = new TXI(*txi);
	else
		_txi = new TXI();

	try {
		load(image);
	} catch (...) {
		delete _txi;
		delete _image;
		throw;
	}

	addToQueue(kQueueTexture);
	addToQueue(kQueueNewTexture);
}

Texture::~Texture() {
	removeFromQueue(kQueueNewTexture);
	removeFromQueue(kQueueTexture);

	if (_textureID != 0)
		GfxMan.abandon(&_textureID, 1);

	delete _txi;
	delete _image;
}

TextureID Texture::getID() const {
	return _textureID;
}

uint32 Texture::getWidth() const {
	return _width;
}

uint32 Texture::getHeight() const {
	return _height;
}

bool Texture::hasAlpha() const {
	if (!_image)
		return false;

	return _image->hasAlpha();
}

ImageDecoder *Texture::loadImage(const Common::UString &name, ::Aurora::FileType *type) {
	::Aurora::FileType iType;
	Common::SeekableReadStream *img = ResMan.getResource(::Aurora::kResourceImage, name, &iType);
	if (!img)
		throw Common::Exception("No such image resource \"%s\"", name.c_str());

	if (type)
		*type = iType;

	ImageDecoder *image = 0;

	try {

		// Loading the different image formats
		if      (iType == ::Aurora::kFileTypeTGA)
			image = new TGA(*img);
		else if (iType == ::Aurora::kFileTypeDDS)
			image = new DDS(*img);
		else if (iType == ::Aurora::kFileTypeTPC)
			image = new TPC(*img);
		else if (iType == ::Aurora::kFileTypeTXB)
			image = new TXB(*img);
		else if (iType == ::Aurora::kFileTypeSBM)
			image = new SBM(*img);
		else
			throw Common::Exception("Unsupported image resource type %d", (int) iType);

	} catch (...) {
		delete img;
		throw;
	}

	delete img;
	return image;
}

void Texture::load(const Common::UString &name) {
	_image = loadImage(name, &_type);
	_name  = name;

	loadTXI(ResMan.getResource(name, ::Aurora::kFileTypeTXI));
	loadImage();
}

void Texture::load(ImageDecoder *image) {
	_image = image;

	loadImage();
}

void Texture::loadTXI(Common::SeekableReadStream *stream) {
	if (!stream)
		return;

	delete _txi;
	_txi = 0;

	try {
		_txi = new TXI(*stream);
	} catch (Common::Exception &e) {
		e.add("Failed loading TXI");
		Common::printException(e);

		_txi = new TXI();
	}

	delete stream;
}

void Texture::loadImage() {
	if (!_image) {
		_width  = 0;
		_height = 0;
		return;
	}

	if (_image->getMipMapCount() < 1)
		throw Common::Exception("Texture has no images");

	// Decompress
	if (GfxMan.needManualDeS3TC())
		_image->decompress();

	// Set dimensions
	_width  = _image->getMipMap(0).width;
	_height = _image->getMipMap(0).height;

	// If we've still got no TXI, look if the image provides TXI data
	loadTXI(_image->getTXI());
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

	// Bind the texture
	glBindTexture(GL_TEXTURE_2D, _textureID);

	// Texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Filter?
	const TXI::Features &features = _txi->getFeatures();
	if (features.filter) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	if (_image->getMipMapCount() == 1) {
		// Texture doesn't specify any mip maps, generate our own

		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 9);
	} else {
		// Texture does specify mip maps, use these

		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, _image->getMipMapCount() - 1);
	}

	// Texture image data
	if (_image->isCompressed()) {
		// Compressed texture data

		for (uint32 i = 0; i < _image->getMipMapCount(); i++) {
			const ImageDecoder::MipMap &mipMap = _image->getMipMap(i);

			glCompressedTexImage2D(GL_TEXTURE_2D, i, _image->getFormatRaw(),
			                       mipMap.width, mipMap.height, 0,
			                       mipMap.size, mipMap.data);
		}

	} else {
		// Uncompressed texture data

		for (uint32 i = 0; i < _image->getMipMapCount(); i++) {
			const ImageDecoder::MipMap &mipMap = _image->getMipMap(i);

			glTexImage2D(GL_TEXTURE_2D, i, _image->getFormatRaw(),
			             mipMap.width, mipMap.height, 0, _image->getFormat(),
			             _image->getDataType(), mipMap.data);
		}

	}

}

const TXI &Texture::getTXI() const {
	return *_txi;
}

const ImageDecoder &Texture::getImage() const {
	assert(_image);

	return *_image;
}

bool Texture::reload(ImageDecoder *image, const TXI *txi) {
	removeFromQueue(kQueueNewTexture);
	removeFromQueue(kQueueTexture);

	if (txi) {
		delete _txi;
		_txi = 0;

		_txi = new TXI(*txi);
	}

	delete _image;

	load(image);

	addToQueue(kQueueTexture);
	addToQueue(kQueueNewTexture);

	return true;
}

bool Texture::reload(const Common::UString &name) {
	if (!name.empty())
		_name = name;

	if (_name.empty())
		// Yeah, we don't know the resource name, so we can't reload the texture
		return false;

	removeFromQueue(kQueueNewTexture);
	removeFromQueue(kQueueTexture);

	delete _txi;
	delete _image;

	_txi = new TXI();

	load(_name);

	addToQueue(kQueueTexture);
	addToQueue(kQueueNewTexture);

	return true;
}

bool Texture::dumpTGA(const Common::UString &fileName) const {
	if (!_image)
		return false;

	return _image->dumpTGA(fileName);
}

} // End of namespace Aurora

} // End of namespace Graphics
