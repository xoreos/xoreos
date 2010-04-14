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

#include "events/requests.h"

#include "aurora/resman.h"

using Events::RequestID;

namespace Graphics {

Texture::Texture(const std::string &name) :
	_inTextureList(false), _textureID(0xFFFFFFFF), _type(Aurora::kFileTypeNone),
	_plainImage(0), _ddsImage(0) {

	load(name);

	_textureRef    = GfxMan.registerTexture(*this);
	_inTextureList = true;
}

Texture::~Texture() {
	if (_inTextureList)
		GfxMan.unregisterTexture(_textureRef);

	if (_textureID != 0xFFFFFFFF)
		RequestMan.dispatchAndForget(RequestMan.destroyTexture(_textureID));

	delete _plainImage;
	delete _ddsImage;
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
		_plainImage = new TGA(img);
	else if (_type == Aurora::kFileTypeDDS)
		_ddsImage   = new DDS(img);
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
	glGenTextures(1, &_textureID);

	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (_plainImage) {
		_plainImage->load();

		PixelFormat format = _plainImage->getFormat();

		GLint err = gluBuild2DMipmaps(GL_TEXTURE_2D, getBytesPerPixel(format),
		                              _plainImage->getWidth(), _plainImage->getHeight(),
		                              format, GL_UNSIGNED_BYTE, _plainImage->getData());

		if (err != 0)
			throw Common::Exception("Failed loading texture data: %s", gluErrorString(err));
	}

	if (_ddsImage) {
		_ddsImage->load();

		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, _ddsImage->getMipMapCount() - 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

		for (int i = 0; i < _ddsImage->getMipMapCount(); i++) {
			const DDS::MipMap &mipMap = _ddsImage->getMipMap(i);

			if (_ddsImage->isCompressed()) {
				glCompressedTexImage2D(GL_TEXTURE_2D, i, _ddsImage->getFormatRaw(),
						mipMap.width, mipMap.height, 0, mipMap.size, mipMap.data);
			} else {
				glTexImage2D(GL_TEXTURE_2D, i, _ddsImage->getFormatRaw(),
						mipMap.width, mipMap.height, 0, _ddsImage->getFormat(), _ddsImage->getDataType(),
						mipMap.data);
			}
		}

	}
}

} // End of namespace Graphics
