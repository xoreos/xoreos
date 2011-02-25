/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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
#include "graphics/font.h"

#include "graphics/aurora/text.h"

namespace Graphics {

namespace Aurora {

Text::Text(const FontHandle &font, const Common::UString &str, float align) :
	_font(font), _x(0.0), _y(0.0), _hasColor(false), _align(align), _list(0) {

	set(str);

	_distance = -5.0;
}

Text::Text(const FontHandle &font, const Common::UString &str,
		float r, float g, float b, float a, float align) :
	_font(font), _x(0.0), _y(0.0), _hasColor(true), _r(r), _g(g), _b(b), _a(a),
	_align(align), _list(0) {

	set(str);

	_distance = -5.0;
}

Text::~Text() {
	if (_list != 0)
		GfxMan.abandon(_list, 1);
}

void Text::set(const Common::UString &str) {
	GfxMan.lockFrame();

	bool visible = Renderable::isInQueue();

	Renderable::removeFromQueue();

	set_internal(str);

	if (visible)
		Renderable::addToQueue();

	RequestMan.dispatchAndForget(RequestMan.rebuild(*this));

	GfxMan.unlockFrame();
}

void Text::set_internal(const Common::UString &str) {
	_str = str;

	const Font &font = _font.getFont();

	_height = font.getHeight(_str);
	_width  = font.getWidth (_str);
}

void Text::setColor(float r, float g, float b, float a) {
	GfxMan.lockFrame();

	bool visible = Renderable::isInQueue();

	Renderable::removeFromQueue();

	_hasColor = true;
	_r = r;
	_g = g;
	_b = b;
	_a = a;

	if (visible)
		Renderable::addToQueue();

	RequestMan.dispatchAndForget(RequestMan.rebuild(*this));

	GfxMan.unlockFrame();
}

void Text::unsetColor() {
	GfxMan.lockFrame();

	bool visible = Renderable::isInQueue();

	Renderable::removeFromQueue();

	_hasColor = false;

	if (visible)
		Renderable::addToQueue();

	RequestMan.dispatchAndForget(RequestMan.rebuild(*this));

	GfxMan.unlockFrame();
}

void Text::setPosition(float x, float y) {
	GfxMan.lockFrame();

	bool visible = Renderable::isInQueue();

	Renderable::removeFromQueue();

	setPosition_internal(x, y);

	if (visible)
		Renderable::addToQueue();

	GfxMan.unlockFrame();
}

void Text::setPosition_internal(float x, float y) {
	_x = roundf(x);
	_y = roundf(y);
}

void Text::show() {
	Renderable::addToQueue();
}

void Text::hide() {
	Renderable::removeFromQueue();
}

bool Text::isVisible() {
	return Renderable::isInQueue();
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
	glTranslatef(_x, _y, 0.0);

	if (_list > 0)
		glCallList(_list);
}

void Text::doRebuild() {
	const Font &font = _font.getFont();

	_list = glGenLists(1);

	glNewList(_list, GL_COMPILE);
	if (_hasColor)
		font.draw(_str, _r, _g, _b, _a, _align);
	else
		font.draw(_str, _align);
	glEndList();
}

void Text::doDestroy() {
	if (_list == 0)
		return;

	glDeleteLists(_list, 0);

	_list = 0;
}

} // End of namespace Aurora

} // End of namespace Graphics
