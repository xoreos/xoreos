/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/cursor.cpp
 *  A cursor as used in the Aurora engines.
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/types.h"
#include "aurora/resman.h"

#include "graphics/images/decoder.h"
#include "graphics/images/txi.h"
#include "graphics/images/tga.h"
#include "graphics/images/dds.h"
#include "graphics/images/winiconimage.h"

#include "graphics/aurora/cursor.h"
#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/texture.h"

namespace Graphics {

namespace Aurora {

Cursor::Cursor(const Common::UString &name) : _name(name), _hotspotX(0), _hotspotY(0) {
	load();
}

Cursor::~Cursor() {
	TextureMan.release(_texture);
}

void Cursor::render() {
	TextureMan.activeTexture(0);
	TextureMan.set(_texture);

	int x, y;
	CursorMan.getPosition(x, y);

	glTranslatef(x - _hotspotX, -y - _height + _hotspotY, 0.0);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex2f(0.0, 0.0);
		glTexCoord2f(0.0, 1.0);
		glVertex2f(0.0, _width);
		glTexCoord2f(1.0, 1.0);
		glVertex2f(_height, _width);
		glTexCoord2f(1.0, 0.0);
		glVertex2f(_height, 0.0);
	glEnd();
}

void Cursor::load() {
	::Aurora::FileType type;

	Common::SeekableReadStream *img = ResMan.getResource(::Aurora::kResourceCursor, _name, &type);
	if (!img)
		throw Common::Exception("No such cursor resource \"%s\"", _name.c_str());

	_hotspotX = 0;
	_hotspotY = 0;

	ImageDecoder *image;
	// Loading the different image formats
	if      (type == ::Aurora::kFileTypeTGA)
		image = new TGA(img);
	else if (type == ::Aurora::kFileTypeDDS)
		image = new DDS(img);
	else if (type == ::Aurora::kFileTypeCUR) {
		WinIconImage *cursor = new WinIconImage(img);

		cursor->load();

		_hotspotX = cursor->getHotspotX();
		_hotspotY = cursor->getHotspotY();

		image = cursor;
	} else {
		delete img;
		throw Common::Exception("Unsupported cursor resource type %d", (int) type);
	}

	image->load();

	_width  = ((const ImageDecoder *) image)->getMipMap(0).width;
	_height = ((const ImageDecoder *) image)->getMipMap(0).height;

	TXI txi;
	txi.getFeatures().filter = false;

	try {
		Texture *texture = new Texture(image, txi);

		image = 0;

		try {
			_texture = TextureMan.add(_name, texture);
		} catch(...) {
			delete texture;
			throw;
		}

	} catch (...) {
		delete image;
		throw;
	}

}

} // End of namespace Aurora

} // End of namespace Graphics
