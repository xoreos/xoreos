/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/texture.cpp
 *  A texture.
 */

#include "common/types.h"
#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "graphics/texture.h"
#include "graphics/graphics.h"
#include "graphics/images/decoder.h"
#include "graphics/images/tga.h"
#include "graphics/images/dds.h"
#include "graphics/images/tpc.h"

#include "events/requests.h"

#include "aurora/resman.h"

using Events::RequestID;

namespace Graphics {

Texture::Texture(const std::string &name) :
	_inTextureList(false), _textureID(0xFFFFFFFF), _type(Aurora::kFileTypeNone), _image(0) {

	load(name);

	_textureRef    = GfxMan.registerTexture(*this);
	_inTextureList = true;
}

Texture::~Texture() {
	if (_inTextureList)
		GfxMan.unregisterTexture(_textureRef);

	if (_textureID != 0xFFFFFFFF)
		RequestMan.dispatchAndForget(RequestMan.destroyTexture(_textureID));

	delete _image;
}

TextureID Texture::getID() const {
	if (_textureID == 0xFFFFFFFF)
		throw Common::Exception("Non-existing texture ID");

	return _textureID;
}

void Texture::load(const std::string &name) {
	Common::SeekableReadStream *img = ResMan.getImage(name, &_type);
	if (!img)
		throw Common::Exception("No such image resource \"%s\"", name.c_str());

	if      (_type == Aurora::kFileTypeTGA)
		_image = new TGA(img);
	else if (_type == Aurora::kFileTypeDDS)
		_image = new DDS(img);
	else if (_type == Aurora::kFileTypeTPC)
		_image = new TPC(img);
	else
		throw Common::Exception("Unsupported image resource type %d", (int) _type);

	RequestMan.dispatchAndForget(RequestMan.loadTexture(this));
}

void Texture::removedFromList() {
	destroy();
}

void Texture::destroy() {
	if (_textureID == 0xFFFFFFFF)
		return;

	glDeleteTextures(1, &_textureID);

	_textureID = 0xFFFFFFFF;
}

void Texture::reload() {
	if (!_image)
		// No image
		return;

	// If we didn't already, let the image load
	_image->load();

	if (_image->getMipMapCount() < 1)
		throw Common::Exception("Texture has no images");

	// Generate the texture ID
	glGenTextures(1, &_textureID);

	// Set some parameters
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (_image->isCompressed()) {
		// Compressed image
		// (No chance of building mip maps ourselves should the image not provide any)

		// TODO: GL_TEXTURE_BASE_LEVEL_SGIS?

		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, _image->getMipMapCount() - 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

		for (int i = 0; i < _image->getMipMapCount(); i++) {
			const DDS::MipMap &mipMap = ((const ImageDecoder *) _image)->getMipMap(i);

			glCompressedTexImage2D(GL_TEXTURE_2D, i, _image->getFormatRaw(),
			                       mipMap.width, mipMap.height, 0,
			                       mipMap.size, mipMap.data);
		}

		return;
	}

	if (_image->getMipMapCount() > 1) {
		// The image gives us mip maps, use them

		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, _image->getMipMapCount() - 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

		for (int i = 0; i < _image->getMipMapCount(); i++) {
			const ImageDecoder::MipMap &mipMap = ((const ImageDecoder *) _image)->getMipMap(i);

			glTexImage2D(GL_TEXTURE_2D, i, _image->getFormatRaw(),
			             mipMap.width, mipMap.height, 0, _image->getFormat(),
			             _image->getDataType(), mipMap.data);

		}

		return;
	}

	// The image does not give us mip maps, build them ourselves

	const ImageDecoder::MipMap &mipMap = ((const ImageDecoder *) _image)->getMipMap(0);

	GLint err = gluBuild2DMipmaps(GL_TEXTURE_2D, _image->getFormatRaw(),
	                              mipMap.width, mipMap.height, _image->getFormat(),
	                              _image->getDataType(), mipMap.data);

	if (err != 0)
		throw Common::Exception("Failed loading texture data: %s", gluErrorString(err));

}

} // End of namespace Graphics
