/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/text.cpp
 *  A text object.
 */

#include "events/requests.h"

#include "graphics/graphics.h"

#include "graphics/aurora/text.h"
#include "graphics/aurora/font.h"

namespace Graphics {

namespace Aurora {

Text::Text(FontHandle &font, float x, float y, const Common::UString &str) :
	_firstTime(true), _font(font), _x(x), _y(y), _str(str) {

	_distance = -5.0;
}

Text::~Text() {
}

void Text::show() {
	addToQueue();
}

void Text::hide() {
	removeFromQueue();
}

void Text::newFrame() {
}

void Text::render() {
	const Font &font = _font.getFont();

	glTranslatef(_x * (GfxMan.getScreenWidth() / 2.0), _y * (GfxMan.getScreenHeight() / 2.0) - font.getScale(), -1.0);
	glScalef(font.getScale(), font.getScale(), 0.0);

	if (_firstTime) {
		// Sync, to make sure that the texture has finished loading
		RequestMan.sync();
		_firstTime = false;
	}

	font.setTexture();

	const char *str = _str.c_str();
	while (*str) {
		float w = font.drawCharacter(*str++);
		glTranslatef(w + font.getSpaceR(), 0.0, 0.0);
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
