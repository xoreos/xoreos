/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/abcfont.cpp
 *  An ABC/SBM font, as used by Jade Empire.
 */

#include "common/ustring.h"
#include "common/error.h"
#include "common/stream.h"

#include "events/requests.h"

#include "aurora/resman.h"

#include "graphics/aurora/texture.h"
#include "graphics/aurora/abcfont.h"

namespace Graphics {

namespace Aurora {

ABCFont::ABCFont(const Common::UString &name) : _base(0) {
	_texture = TextureMan.get(name);

	load(name);
}

ABCFont::~ABCFont() {
}

float ABCFont::getHeight() const {
	return 32.0;
}

float ABCFont::getWidth(uint32 c) const {
	const Char &cC = findChar(c);

	return cC.spaceL + cC.width + cC.spaceR;
}

void ABCFont::draw(uint32 c) const {
	TextureMan.set(_texture);

	const Char &cC = findChar(c);

	glTranslatef(cC.spaceL, 0.0, 0.0);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(cC.tX[i], cC.tY[i]);
		glVertex2f  (cC.vX[i], cC.vY[i]);
	}
	glEnd();

	glTranslatef(cC.width + cC.spaceR, 0.0, 0.0);
}

void ABCFont::load(const Common::UString &name) {
	// We need to wait for the texture to finish loading
	RequestMan.sync();

	Common::SeekableReadStream *abc = ResMan.getResource(name, ::Aurora::kFileTypeABC);
	if (!abc)
		throw Common::Exception("No such font \"%s\"", name.c_str());

	// Init the invalid char
	_invalid.dataX  = 0;
	_invalid.dataY  = 0;
	_invalid.width  = 0;
	_invalid.spaceL = 0;
	_invalid.spaceR = 0;

	_invalid.tX[0] = 0.0; _invalid.tY[0] = 0.0;
	_invalid.tX[1] = 0.0; _invalid.tY[1] = 0.0;
	_invalid.tX[2] = 0.0; _invalid.tY[2] = 0.0;
	_invalid.tX[3] = 0.0; _invalid.tY[3] = 0.0;
	_invalid.vX[0] = 0.0; _invalid.vY[0] = 0.0;
	_invalid.vX[1] = 0.0; _invalid.vY[1] = 0.0;
	_invalid.vX[2] = 0.0; _invalid.vY[2] = 0.0;
	_invalid.vX[3] = 0.0; _invalid.vY[3] = 0.0;

	bool hasInvalid = false;

	try {
		if (abc->size() != 524280)
			throw Common::Exception("Invalid font (%d)", abc->size());

		_base = abc->readByte();

		abc->skip(7); // Probably random garbage

		// Read the ASCII character
		for (int i = 1; i < 128; i++) {
			Char &c = _ascii[i];

			readCharDesc(c, *abc);
			calcCharVertices(c);

			// Points to the "invalid character"
			if (!hasInvalid && (c.dataX == 0) && (c.dataY == 0)) {
				_invalid   = c;
				hasInvalid = true;
			}
		}

		// Read the UTF16 extended characters
		for (int i = 128; i < 65535; i++) {
			Char c;

			readCharDesc(c, *abc);

			// Points to the "invalid character"
			if ((c.dataX == 0) && (c.dataY == 0)) {
				if (!hasInvalid) {
					calcCharVertices(c);
					_invalid   = c;
					hasInvalid = true;
				}

				continue;
			}

			calcCharVertices(c);
			_extended.insert(std::make_pair(Common::UString::fromUTF16((uint16) i), c));
		}

	} catch (...) {
		delete abc;
		throw;
	}
}

void ABCFont::readCharDesc(Char &c, Common::SeekableReadStream &abc) {
	uint32 offset = abc.readUint32LE();
	byte   plane  = abc.readByte();

	c.spaceL = abc.readByte();
	c.width  = abc.readByte();
	c.spaceR = abc.readByte();

	if (((offset % 1024) != 0) || (plane > 3))
		throw Common::Exception("Invalid char data (%d, %d)", offset, plane);

	c.dataX =  plane          * 32;
	c.dataY = (offset / 1024) * 32;
}

void ABCFont::calcCharVertices(Char &c) {
	float w = _texture.getTexture().getWidth();
	float h = _texture.getTexture().getHeight();

	float vW = c.width;
	float vH = 32.0;
	float tW = vW / w;
	float tH = vH / h;

	c.tX[0] = c.dataX / w ; c.tY[0] = c.dataY / h ;
	c.tX[1] = c.tX[0] + tW; c.tY[1] = c.tY[0]     ;
	c.tX[2] = c.tX[0] + tW; c.tY[2] = c.tY[0] + tH;
	c.tX[3] = c.tX[0]     ; c.tY[3] = c.tY[0] + tH;

	c.vX[0] = 0.0         ; c.vY[0] = 0.0     + vH;
	c.vX[1] = 0.0     + vW; c.vY[1] = 0.0     + vH;
	c.vX[2] = 0.0     + vW; c.vY[2] = 0.0         ;
	c.vX[3] = 0.0         ; c.vY[3] = 0.0         ;
}

const ABCFont::Char &ABCFont::findChar(uint32 c) const {
	if (Common::UString::isASCII(c))
		return _ascii[c];

	std::map<uint32, Char>::const_iterator ch = _extended.find(c);
	if (ch == _extended.end())
		return _invalid;

	return ch->second;
}

} // End of namespace Aurora

} // End of namespace Graphics
