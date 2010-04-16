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

#include "events/requests.h"

#include "graphics/font.h"
#include "graphics/texture.h"
#include "graphics/images/txi.h"

namespace Graphics {

Font::Font(const std::string &name) : _listStart(0) {
	_texture = new Texture(name);

	load();
}

Font::~Font() {
	delete _texture;
}

void Font::rebuild() {
	// Generate the needed number of lists
	_listStart = glGenLists(_charCount);

	// Build a list for each character
	for (uint32 i = 0; i < _charCount; i++) {
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
		RequestMan.dispatchAndForget(RequestMan.destroyLists(_listStart, _charCount));
}

void Font::load() {
	// We need to wait for the texture to finish loading
	RequestMan.sync();

	if (!_texture->hasTXI())
		throw Common::Exception("Font texture without a TXI");

	const TXI &txi = _texture->getTXI();
	const TXI::Features &txiFeatures = txi.getFeatures();

	// Number of characters
	_charCount = txiFeatures.numChars;
	if (_charCount == 0)
		throw Common::Exception("Texture defines no characters");

	// Character coordinates
	const std::vector<TXI::Coords> &uls = txiFeatures.upperLeftCoords;
	const std::vector<TXI::Coords> &lrs = txiFeatures.lowerRightCoords;
	if ((uls.size() < _charCount) || (lrs.size() < _charCount))
		throw Common::Exception("Texture defines not enough character coordinates");

	// Build the character texture and vector coordinates
	_chars.resize(_charCount);
	for (uint32 i = 0; i < _charCount; i++) {
		const TXI::Coords ul = uls[i];
		const TXI::Coords lr = lrs[i];
		Char &c = _chars[i];

		// Texture coordinates, directly out of the TXI
		c.tX[0] = ul.x; c.tY[0] = ul.y;
		c.tX[1] = lr.x; c.tY[1] = ul.y;
		c.tX[2] = lr.x; c.tY[2] = lr.y;
		c.tX[3] = ul.x; c.tY[3] = lr.y;

		// width / height
		double ratio = (lr.x - ul.x) / (lr.y - ul.y);

		// Vector coordinates. Fixed height of 1.00, width to fit the texture ratio
		c.vX[0] = 0.00;         c.vY[0] = 0.00;
		c.vX[1] = 1.00 * ratio; c.vY[1] = 0.00;
		c.vX[2] = 1.00 * ratio; c.vY[2] = 1.00;
		c.vX[3] = 1.00;         c.vY[3] = 1.00;
	}

	// And build the actual display lists
	RequestMan.dispatchAndForget(RequestMan.buildLists(this));
}

} // End of namespace Graphics
