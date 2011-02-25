/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/texturefont.cpp
 *  A texture font, as used by NWN and KotOR/KotOR2.
 */

#include "common/types.h"
#include "common/error.h"
#include "common/ustring.h"
#include "common/util.h"

#include "events/requests.h"

#include "graphics/images/txi.h"

#include "graphics/aurora/texturefont.h"
#include "graphics/aurora/texture.h"

namespace Graphics {

namespace Aurora {

// TODO: Multibyte fonts?
TextureFont::TextureFont(const Common::UString &name) : _height(1.0), _spaceR(0.0), _spaceB(0.0) {
	_texture = TextureMan.get(name);

	load();
}

TextureFont::~TextureFont() {
}

float TextureFont::getWidth(uint32 c) const {
	if (c >= _chars.size())
		return 0.0;

	return (_chars[c].width + _spaceR) / 100.0;
}

float TextureFont::getHeight() const {
	return _height / 100.0;
}

float TextureFont::getLineSpacing() const {
	return _spaceB / 100.0;
}

void TextureFont::draw(uint32 c) const {
	if (c >= _chars.size())
		return;

	TextureMan.set(_texture);

	const Char &cC = _chars[c];

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(cC.tX[i], cC.tY[i]);
		glVertex2f  (cC.vX[i], cC.vY[i]);
	}
	glEnd();

	glTranslatef(cC.width + _spaceR, 0.0, 0.0);
}

void TextureFont::load() {
	// We need to wait for the texture to finish loading
	RequestMan.sync();

	const Texture &texture = _texture.getTexture();
	const TXI::Features &txiFeatures = texture.getTXI().getFeatures();

	// Number of characters
	uint32 charCount = txiFeatures.numChars;
	if (charCount == 0)
		throw Common::Exception("Texture defines no characters");

	// Character coordinates
	const std::vector<TXI::Coords> &uls = txiFeatures.upperLeftCoords;
	const std::vector<TXI::Coords> &lrs = txiFeatures.lowerRightCoords;
	if ((uls.size() < charCount) || (lrs.size() < charCount))
		throw Common::Exception("Texture defines not enough character coordinates");

	if ((texture.getWidth() == 0) || (texture.getHeight() == 0))
		throw Common::Exception("Invalid texture dimensions (%dx%d)", texture.getWidth(), texture.getHeight());

	double textureRatio = ((double) texture.getWidth()) / ((double) texture.getHeight());

	// Get features
	_height = txiFeatures.fontHeight * 100.0;
	_spaceR = txiFeatures.spacingR   * 100.0;
	_spaceB = txiFeatures.spacingB   * 100.0;

	// Build the character texture and vertex coordinates
	_chars.resize(charCount);
	for (uint32 i = 0; i < charCount; i++) {
		TXI::Coords ul = uls[i];
		TXI::Coords lr = lrs[i];
		Char &c = _chars[i];

		// Texture coordinates, directly out of the TXI
		c.tX[0] = ul.x; c.tY[0] = lr.y;
		c.tX[1] = lr.x; c.tY[1] = lr.y;
		c.tX[2] = lr.x; c.tY[2] = ul.y;
		c.tX[3] = ul.x; c.tY[3] = ul.y;

		double height = ABS(lr.y - ul.y);
		double width  = ABS(lr.x - ul.x);
		double ratio  = ((height != 0.0) ? (width / height) : 0.0) * textureRatio;

		// Vertex coordinates. Fixed height, width to fit the texture ratio
		c.vX[0] = 0.00;            c.vY[0] = 0.00;
		c.vX[1] = _height * ratio; c.vY[1] = 0.00;
		c.vX[2] = _height * ratio; c.vY[2] = _height;
		c.vX[3] = 0.00;            c.vY[3] = _height;

		c.width = c.vX[1] - c.vX[0];
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
