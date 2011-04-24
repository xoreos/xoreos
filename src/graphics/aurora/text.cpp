/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
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
	_font(font), _x(0.0), _y(0.0), _hasColor(false), _align(align) {

	set(str);

	_distance = -FLT_MAX;
}

Text::Text(const FontHandle &font, const Common::UString &str,
		float r, float g, float b, float a, float align) :
	_font(font), _x(0.0), _y(0.0), _hasColor(true), _r(r), _g(g), _b(b), _a(a), _align(align) {

	set(str);

	_distance = -FLT_MAX;
}

Text::~Text() {
	hide();
}

void Text::set(const Common::UString &str) {
	GfxMan.lockFrame();

	_str = str;

	const Font &font = _font.getFont();

	_lineCount = font.getLineCount(_str);

	_height = font.getHeight(_str);
	_width  = font.getWidth (_str);

	GfxMan.unlockFrame();
}

void Text::setColor(float r, float g, float b, float a) {
	GfxMan.lockFrame();

	_hasColor = true;
	_r = r;
	_g = g;
	_b = b;
	_a = a;

	GfxMan.unlockFrame();
}

void Text::unsetColor() {
	GfxMan.lockFrame();

	_hasColor = false;

	GfxMan.unlockFrame();
}

const Common::UString &Text::get() const {
	return _str;
}

void Text::getPosition(float &x, float &y, float &z) const {
	x = _x;
	y = _y;
	z = _distance;
}

void Text::setPosition(float x, float y, float z) {
	GfxMan.lockFrame();

	_x = roundf(x);
	_y = roundf(y);

	_distance = z;
	resort();

	GfxMan.unlockFrame();
}

bool Text::isEmpty() {
	return _str.empty();
}

uint32 Text::getLineCount() const {
	return _lineCount;
}

float Text::getWidth() const {
	return _width;
}

float Text::getHeight() const {
	return _height;
}

void Text::calculateDistance() {
}

void Text::render(RenderPass pass) {
	// Text objects should always be transparent
	if (pass == kRenderPassOpaque)
		return;

	glTranslatef(_x, _y, 0.0);

	if (_hasColor)
		_font.getFont().draw(_str, _r, _g, _b, _a, _align);
	else
		_font.getFont().draw(_str, _align);
}

bool Text::isIn(float x, float y) const {
	if ((x < _x) || (y < _y))
		return false;

	if ((x > (_x + _width)) || (y > (_y + _height)))
		return false;

	return true;
}

} // End of namespace Aurora

} // End of namespace Graphics
