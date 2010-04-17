/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/font.cpp
 *  A font.
 */

#include "common/types.h"
#include "common/error.h"
#include "common/util.h"

#include "events/requests.h"

#include "graphics/font.h"
#include "graphics/texture.h"
#include "graphics/images/txi.h"

namespace Graphics {

// TODO: Multibyte fonts

Font::Font(const std::string &name) : _listStart(0), _scale(1.0), _spaceR(0.0), _spaceB(0.0) {
	_texture = new Texture(name);

	load();
}

Font::~Font() {
	delete _texture;
}

void Font::setTexture() const {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texture->getID());
}

double Font::drawCharacter(char c) const {
	if (((uint32) c) >= _chars.size())
		return 0.0;

	const Char &cC = _chars[(int) c];

	glCallList(cC.listID);

	return cC.width;
}

void Font::rebuild() {
	uint32 charCount = _chars.size();

	// Generate the needed number of lists
	_listStart = glGenLists(charCount);

	// Build a list for each character
	for (uint32 i = 0; i < charCount; i++) {
		Char &c = _chars[i];

		// The characters own personal list ID
		c.listID = _listStart + i;

		// Each character is one one textured quad
		glNewList(c.listID, GL_COMPILE);
		glBegin(GL_QUADS);
		for (int j = 0; j < 4; j++) {
			glTexCoord2d(c.tX[j], c.tY[j]);
			glVertex3d(c.vX[j], c.vY[j], 0.0);
		}
		glEnd();
		glEndList();
	}
}

void Font::destroy() {
	if (_listStart != 0)
		RequestMan.dispatchAndForget(RequestMan.destroyLists(_listStart, _chars.size()));
}

void Font::load() {
	// We need to wait for the texture to finish loading
	RequestMan.sync();

	const TXI &txi = _texture->getTXI();
	const TXI::Features &txiFeatures = txi.getFeatures();

	// Number of characters
	uint32 charCount = txiFeatures.numChars;
	if (charCount == 0)
		throw Common::Exception("Texture defines no characters");

	// Character coordinates
	const std::vector<TXI::Coords> &uls = txiFeatures.upperLeftCoords;
	const std::vector<TXI::Coords> &lrs = txiFeatures.lowerRightCoords;
	if ((uls.size() < charCount) || (lrs.size() < charCount))
		throw Common::Exception("Texture defines not enough character coordinates");

	if ((_texture->getWidth() == 0) || (_texture->getHeight() == 0))
		throw Common::Exception("Invalid texture dimensions (%dx%d)", _texture->getWidth(), _texture->getHeight());

	double textureRatio = ((double) _texture->getWidth()) / ((double) _texture->getHeight());

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

		// Vertex coordinates. Fixed height of 1.00, width to fit the texture ratio
		c.vX[0] = 0.00;         c.vY[0] = 0.00;
		c.vX[1] = 1.00 * ratio; c.vY[1] = 0.00;
		c.vX[2] = 1.00 * ratio; c.vY[2] = 1.00;
		c.vX[3] = 0.00;         c.vY[3] = 1.00;

		c.width = c.vX[1] - c.vX[0];
	}

	// Get features
	_scale  = txiFeatures.fontHeight * 100;
	_spaceR = txiFeatures.spacingR;
	_spaceB = txiFeatures.spacingB;

	// And build the actual display lists
	RequestMan.dispatchAndForget(RequestMan.buildLists(this));
}

double Font::getScale() const {
	return _scale;
}

double Font::getSpaceR() const {
	return _spaceR;
}

double Font::getSpaceB() const {
	return _spaceB;
}

} // End of namespace Graphics
