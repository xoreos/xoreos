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

Text::Text(FontHandle &font, const Common::UString &str) :
	_firstTime(true), _font(font), _x(0.0), _y(0.0) {

	set(str);

	_distance = -5.0;
}

Text::~Text() {
}

void Text::set(const Common::UString &str) {
	GfxMan.lockFrame();

	bool visible = isInQueue();

	removeFromQueue();

	_str = str;

	const Font &font = _font.getFont();

	_height = font.getScale() / 100.0;
	_width  = 0.0;
	for (Common::UString::iterator c = _str.begin(); c != _str.end() && *c; ++c)
		_width += font.getWidth(*c) + font.getSpaceR();
	_width *= font.getScale() / 100.0;

	if (visible)
		addToQueue();

	GfxMan.unlockFrame();
}

void Text::setPosition(float x, float y) {
	GfxMan.lockFrame();

	bool visible = isInQueue();

	removeFromQueue();

	_x = roundf(x * 100.0) / 100.0;
	_y = roundf(y * 100.0) / 100.0;

	if (visible)
		addToQueue();

	GfxMan.unlockFrame();
}

void Text::show() {
	addToQueue();
}

void Text::hide() {
	removeFromQueue();
}

float Text::getWidth() const {
	return _width;
}

float Text::getHeight() const {
	return _height;
}

void Text::newFrame() {
}

void Text::render() {
	const Font &font = _font.getFont();

	glTranslatef(_x * 100.0, _y * 100.0, 0.0);
	glScalef(font.getScale(), font.getScale(), 0.0);

	if (_firstTime) {
		// Sync, to make sure that the texture has finished loading
		RequestMan.sync();
		_firstTime = false;
	}

	font.setTexture();

	for (Common::UString::iterator c = _str.begin(); c != _str.end() && *c; ++c) {
		float w = font.drawCharacter(*c);
		glTranslatef(w + font.getSpaceR(), 0.0, 0.0);
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
