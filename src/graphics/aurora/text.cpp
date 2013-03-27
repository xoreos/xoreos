/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
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

Text::Text(const FontHandle &font, const Common::UString &str,
		float r, float g, float b, float a, float align) :
	_r(r), _g(g), _b(b), _a(a), _font(font), _x(0.0), _y(0.0), _align(align) {

	set(str);

	_distance = -FLT_MAX;
}

Text::~Text() {
	hide();
}

void Text::set(const Common::UString &str) {
	GfxMan.lockFrame();

	parseColors(str, _str, _colors);

	Font &font = _font.getFont();

	font.buildChars(str);

	_lineCount = font.getLineCount(_str);

	_height = font.getHeight(_str);
	_width  = font.getWidth (_str);

	GfxMan.unlockFrame();
}

void Text::getColor(float& r, float& g, float& b, float& a) const {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

void Text::setColor(float r, float g, float b, float a) {
	GfxMan.lockFrame();

	_r = r;
	_g = g;
	_b = b;
	_a = a;

	GfxMan.unlockFrame();
}

void Text::unsetColor() {
	setColor(1.0, 1.0, 1.0, 1.0);
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

	_font.getFont().draw(_str, _colors, _r, _g, _b, _a, _align);
}

bool Text::isIn(float x, float y) const {
	if ((x < _x) || (y < _y))
		return false;

	if ((x > (_x + _width)) || (y > (_y + _height)))
		return false;

	return true;
}

void Text::parseColors(const Common::UString &str, Common::UString &parsed,
                       ColorPositions &colors) {

	parsed.clear();
	colors.clear();

	ColorPosition color;

	// Split by text tokens. They will have a strictly interleaving plain/token order
	std::vector<Common::UString> tokens;
	Common::UString::splitTextTokens(str, tokens);

	bool plain = false;
	for (std::vector<Common::UString>::iterator t = tokens.begin(); t != tokens.end(); ++t) {
		plain = !plain;

		if (plain) {
			// Plain text, add it verbatim

			parsed += *t;
			continue;
		}

		if ((t->size() == 11) && t->beginsWith("<c") && t->endsWith(">")) {
			// Color start token

			uint8 colorValue[4];

			Common::UString::iterator it = t->begin();

			// Skip "<c"
			++it;
			++it;

			for (int i = 0; i < 8; i++, ++it) {
				uint32 c = *it;

				// Convert the hex values into true nibble values
				if      ((c >= '0') && (c <= '9'))
					c =  c - '0';
				else if ((c >= 'A') && (c <= 'F'))
					c = (c - 'A') + 10;
				else if ((c >= 'f') && (c <= 'f'))
					c = (c - 'a') + 10;
				else
					c = 15;

				// Merge two nibbles into one color value byte
				uint8 &value = colorValue[i / 2];
				bool  high   = (i % 2) == 0;

				if (high)
					value  = c << 4;
				else
					value |= c;
			}

			// Add the color change

			color.position     = parsed.size();
			color.defaultColor = false;

			color.r = colorValue[0] / 255.0;
			color.g = colorValue[1] / 255.0;
			color.b = colorValue[2] / 255.0;
			color.a = colorValue[3] / 255.0;

			colors.push_back(color);

		} else if (*t == "</c>") {
			// Color end token, add a "uncolor" / default color change

			color.position     = parsed.size();
			color.defaultColor = true;

			colors.push_back(color);

		} else
			// Ignore non-color tokens
			parsed += *t;

	}
}

} // End of namespace Aurora

} // End of namespace Graphics
